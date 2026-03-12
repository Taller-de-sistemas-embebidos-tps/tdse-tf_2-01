#include "ppg_processing.h"
#include <math.h>
#include <stdbool.h>

/* ================= CONFIG ================= */
#define DC_ALPHA        0.01f
#define HP_ALPHA        0.969f
#define LP_ALPHA        0.25f
#define RESP_HP_ALPHA   0.994f
#define RESP_LP_ALPHA   0.03f

// Configuración Apnea por SpO2
#define SPO2_DROP_THRESHOLD  2.0f
#define APNEA_TIME_SAMPLES   (PPG_FS_HZ * 2)

/* ================= BUFFERS ================= */
static float ir_raw[PPG_WINDOW_SAMPLES];
static float red_raw[PPG_WINDOW_SAMPLES];
static float ir_filt[PPG_WINDOW_SAMPLES];
static float resp_filt[PPG_WINDOW_SAMPLES];

static uint16_t idx = 0;
static bool buffer_ready = false;

/* ================= ESTADOS FILTROS ================= */
static float dc_ir = 0, dc_red = 0;
static float hp_y = 0, hp_x_prev = 0, lp_y = 0;
static float resp_hp_y = 0, resp_hp_x_prev = 0, resp_lp_y = 0;

static uint32_t global_sample_counter = 0;

/* ================= HELPERS ================= */
static inline float ir_filt_at(uint16_t i)   { return ir_filt[(idx + i) % PPG_WINDOW_SAMPLES]; }
static inline float resp_filt_at(uint16_t i) { return resp_filt[(idx + i) % PPG_WINDOW_SAMPLES]; }
static inline float ir_raw_at(uint16_t i)    { return ir_raw[(idx + i) % PPG_WINDOW_SAMPLES]; }
static inline float red_raw_at(uint16_t i)   { return red_raw[(idx + i) % PPG_WINDOW_SAMPLES]; }

/* ================= FILTROS ================= */
static float bandpass(float x) {
    dc_ir += DC_ALPHA * (x - dc_ir);
    float x_ac = x - dc_ir;
    float hp = HP_ALPHA * (hp_y + x_ac - hp_x_prev);
    hp_x_prev = x_ac;
    hp_y = hp;
    lp_y += LP_ALPHA * (hp - lp_y);
    return lp_y;
}

static float respiratory_filter(float x) {
    float hp = RESP_HP_ALPHA * (resp_hp_y + x - resp_hp_x_prev);
    resp_hp_x_prev = x;
    resp_hp_y = hp;
    resp_lp_y += RESP_LP_ALPHA * (hp - resp_lp_y);
    return resp_lp_y;
}

/* ================= CÁLCULOS TÉCNICOS ================= */

static bool compute_hr(uint16_t *hr_out) {
    float rms_acc = 0.0f;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        float v = ir_filt_at(i);
        rms_acc += v * v;
    }
    float threshold = 0.5f * sqrtf(rms_acc / PPG_WINDOW_SAMPLES);
    uint16_t peaks = 0, last_peak = 0;
    uint32_t period_sum = 0;
    const uint16_t min_samples = PPG_FS_HZ / 3;

    for (uint16_t i = 1; i < PPG_WINDOW_SAMPLES - 1; i++) {
        if (ir_filt_at(i) > threshold && ir_filt_at(i) > ir_filt_at(i-1) && ir_filt_at(i) > ir_filt_at(i+1)) {
            if ((i - last_peak) > min_samples) {
                if (last_peak != 0) { period_sum += (i - last_peak); peaks++; }
                last_peak = i;
            }
        }
    }
    if (peaks < 2) return false;
    *hr_out = (60u * PPG_FS_HZ) / (period_sum / peaks);
    return (*hr_out >= HR_MIN_BPM && *hr_out <= HR_MAX_BPM);
}

static bool compute_rr(uint16_t *rr_out) {
    static uint32_t ultimo_pico_global = 0;
    static float rr_suavizado = 15.0f;
    static bool buscando_pico = true;
    static float max_local = -9999.0f, min_local = 9999.0f;

    float rms_acc = 0.0f;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        float v = resp_filt_at(i);
        rms_acc += v * v;
    }
    float histeresis = fmaxf(20.0f, 0.3f * sqrtf(rms_acc / PPG_WINDOW_SAMPLES));

    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        float curr = resp_filt_at(i);
        if (buscando_pico) {
            if (curr > max_local) max_local = curr;
            if (curr < (max_local - histeresis) && max_local > 0.0f) {
                uint32_t pico_global = (global_sample_counter - PPG_WINDOW_SAMPLES) + i;
                if (ultimo_pico_global != 0) {
                    uint32_t periodo = pico_global - ultimo_pico_global;
                    if (periodo > (PPG_FS_HZ * 2)) {
                        float rr_inst = (60.0f * PPG_FS_HZ) / (float)periodo;
                        if (rr_inst >= RR_MIN_RPM && rr_inst <= RR_MAX_RPM)
                            rr_suavizado = (0.7f * rr_suavizado) + (0.3f * rr_inst);
                        ultimo_pico_global = pico_global;
                    }
                } else { ultimo_pico_global = pico_global; }
                buscando_pico = false; min_local = curr;
            }
        } else {
            if (curr < min_local) min_local = curr;
            if (curr > (min_local + histeresis)) { buscando_pico = true; max_local = curr; }
        }
    }
    *rr_out = (uint16_t)rr_suavizado;
    return true;
}

// Cálculo de SpO2 y detección de Apnea integrados
static bool compute_spo2(uint16_t *spo2_out, bool *apnea_out) {

    static float spo2_baseline = 99.0f;// se puede modificar
    static uint32_t samples_low_spo2 = 0;

    float dc_ir_m = 0, dc_red_m = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        dc_ir_m += ir_raw_at(i); dc_red_m += red_raw_at(i);
    }
    dc_ir_m /= PPG_WINDOW_SAMPLES; dc_red_m /= PPG_WINDOW_SAMPLES;

    float ac_ir = 0, ac_red = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        float ir_ac = ir_raw_at(i) - dc_ir_m;
        float red_ac = red_raw_at(i) - dc_red_m;
        ac_ir += ir_ac * ir_ac; ac_red += red_ac * red_ac;
    }

    if (dc_ir_m == 0 || ac_ir == 0) {
        *apnea_out = false;
        return false;
    }

    float R = (sqrtf(ac_red/PPG_WINDOW_SAMPLES)/dc_red_m) / (sqrtf(ac_ir/PPG_WINDOW_SAMPLES)/dc_ir_m);
    float val = 110.0f - 25.0f * R;

    if (val < 70 || val > 100) {
        *apnea_out = false;
        return false;
    }

    *spo2_out = (uint16_t)val;

    // --- Lógica de Apnea ---
    if (val > 94.0f) {
        spo2_baseline = (0.80f * spo2_baseline) + (0.2f * val);
    }


    if (val <= (spo2_baseline - SPO2_DROP_THRESHOLD)) {
        samples_low_spo2++;
    }
    else {
        samples_low_spo2 = 0;
    }

    *apnea_out = (samples_low_spo2 > APNEA_TIME_SAMPLES);

    return true;
}



void ppg_init(void) {
    idx = 0;
    buffer_ready = false;
    global_sample_counter = 0;

    dc_ir = 0; dc_red = 0;
    hp_y = 0; hp_x_prev = 0; lp_y = 0;
    resp_hp_y = 0; resp_hp_x_prev = 0; resp_lp_y = 0;
}

void ppg_add_sample(uint32_t ir, uint32_t red) {
    ir_raw[idx] = (float)ir;
    red_raw[idx] = (float)red;
    ir_filt[idx] = bandpass((float)ir);
    resp_filt[idx] = respiratory_filter((float)ir);

    idx = (idx + 1) % PPG_WINDOW_SAMPLES;
    global_sample_counter++;

    if (global_sample_counter >= PPG_WINDOW_SAMPLES) buffer_ready = true;
}

bool ppg_compute(ppg_results_t *out) {
    if (!buffer_ready || out == NULL) return false;

    bool ok_hr = compute_hr(&out->hr);
    bool ok_rr = compute_rr(&out->rr);


    bool ok_spo2 = compute_spo2(&out->spo2, &out->apnea);

    out->valid = ok_hr && ok_spo2;
    return out->valid;
}
