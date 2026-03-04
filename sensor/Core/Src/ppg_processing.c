#include "ppg_processing.h"
#include <math.h>

/* ================= CONFIG ================= */

#define DC_ALPHA        0.01f      // DC tracker
#define HP_ALPHA        0.95f
#define LP_ALPHA        0.15f
#define RESP_HP_ALPHA   0.99f
#define RESP_LP_ALPHA   0.015f    // Low-pass respiratorio (~0. Hz)

/* ================= BUFFERS ================= */

static float ir_raw[PPG_WINDOW_SAMPLES];
static float red_raw[PPG_WINDOW_SAMPLES];
static float ir_filt[PPG_WINDOW_SAMPLES];
static float resp_filt[PPG_WINDOW_SAMPLES];

static uint16_t idx = 0;
static bool buffer_full = false;

/* ================= FILTROS ================= */

static float dc_ir = 0;
static float dc_red = 0;
static float hp_y = 0;
static float hp_x_prev = 0;

static float lp_y = 0;

static float resp_hp_y = 0;
static float resp_hp_x_prev = 0;
static float resp_lp_y = 0;

static uint32_t global_sample_counter = 0;
static uint32_t last_breath_sample = 0;


/* Bandpass correcto */
static float bandpass(float x)
{
    /* DC removal lento */
    dc_ir += DC_ALPHA * (x - dc_ir);
    float x_ac = x - dc_ir;

    /* High pass */
    float hp = HP_ALPHA * (hp_y + x_ac - hp_x_prev);
    hp_x_prev = x_ac;
    hp_y = hp;

    /* Low pass */
    lp_y += LP_ALPHA * (hp - lp_y);

    return lp_y;
}




static float respiratory_filter(float x)
{
    /* HP ~0.05 Hz */
    float hp = RESP_HP_ALPHA * (resp_hp_y + x - resp_hp_x_prev);
    resp_hp_x_prev = x;
    resp_hp_y = hp;

    /* LP ~0.5 Hz */
    resp_lp_y += RESP_LP_ALPHA * (hp - resp_lp_y);

    return resp_lp_y;
}
static float mean(float *x)
{
    float acc = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
        acc += x[i];
    return acc / PPG_WINDOW_SAMPLES;
}

static float rms(float *x)
{
    float acc = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
        acc += x[i] * x[i];
    return sqrtf(acc / PPG_WINDOW_SAMPLES);
}
/* ================= HR ================= */

static bool compute_hr(uint16_t *hr_out)
{
    float threshold = 0.5f * rms(ir_filt);

    uint16_t peaks = 0;
    uint16_t last_peak = 0;
    uint32_t period_sum = 0;

    const uint16_t min_samples = PPG_FS_HZ / 3;

    for (uint16_t i = 1; i < PPG_WINDOW_SAMPLES - 1; i++)
    {
        if (ir_filt[i] > threshold &&
            ir_filt[i] > ir_filt[i-1] &&
            ir_filt[i] > ir_filt[i+1])
        {
            if ((i - last_peak) > min_samples)
            {
                if (last_peak != 0)
                {
                    period_sum += (i - last_peak);
                    peaks++;
                }
                last_peak = i;
            }
        }
    }

    if (peaks < 2)
        return false;

    uint32_t avg_period = period_sum / peaks;
    uint32_t hr = (60u * PPG_FS_HZ) / avg_period;

    if (hr < HR_MIN_BPM || hr > HR_MAX_BPM)
        return false;

    *hr_out = hr;
    return true;
}

/* ================= RR ================= */
/* Respiración desde envolvente */

static bool compute_rr(uint16_t *rr_out, bool *apnea_flag)
{
    /* Memoria histórica */
    static uint32_t ultimo_pico_global = 0;
    static float rr_suavizado = 15.0f;

    /* Máquina de estados */
    static bool buscando_pico = true;
    static float max_local = -9999.0f;
    static float min_local = 9999.0f;

    float rms_acc = 0.0f;
    const uint16_t min_samples = (uint16_t)(PPG_FS_HZ * 2.0f);

    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        rms_acc += resp_filt[i] * resp_filt[i];
    }
    float rms_val = sqrtf(rms_acc / PPG_WINDOW_SAMPLES);

    /* 🔥 AJUSTE 2: Histéresis al 30% (0.3f).
       Más sensible para detectar respiraciones superficiales */
    float histeresis = 0.3f * rms_val;
    if (histeresis < 20.0f) histeresis = 20.0f;

    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        float curr = resp_filt[i];

        if (buscando_pico) {
            if (curr > max_local) {
                max_local = curr;
            }
            if (curr < (max_local - histeresis) && max_local > 0.0f) {

                uint32_t pico_global = (global_sample_counter - PPG_WINDOW_SAMPLES) + i;

                /* 🔥 CORRECCIÓN CLAVE: ¡Reseteamos la apnea SIEMPRE que haya un pico físico! */
                last_breath_sample = pico_global;

                if (ultimo_pico_global != 0) {
                    uint32_t periodo = pico_global - ultimo_pico_global;

                    if (periodo > min_samples) {
                        float rr_inst = (60.0f * PPG_FS_HZ) / (float)periodo;

                        /* Solo actualizamos el valor en pantalla si el RPM tiene sentido médico */
                        if (rr_inst >= RR_MIN_RPM && rr_inst <= RR_MAX_RPM) {
                        	rr_suavizado = (0.5f * rr_suavizado) + (0.5f * rr_inst);
                        }
                        ultimo_pico_global = pico_global;
                    }
                } else {
                    ultimo_pico_global = pico_global;
                }

                buscando_pico = false;
                min_local = curr;
            }
        } else {
            /* Buscando el valle */
            if (curr < min_local) {
                min_local = curr;
            }
            /* Subimos desde el fondo, a buscar otro pico */
            if (curr > (min_local + histeresis)) {
                buscando_pico = true;
                max_local = curr;
            }
        }
    }

    /* ---- Evaluación de Apnea ---- */
    uint32_t samples_since_last = global_sample_counter - last_breath_sample;

    /* Apnea estrictamente por tiempo (10 segundos sin picos) */
    *apnea_flag = (samples_since_last > (PPG_FS_HZ * 6));

    if (*apnea_flag) {
        ultimo_pico_global = 0;

        /* Reset de seguridad de la máquina de estados para que
           pueda volver a arrancar cuando respires de nuevo */
        max_local = -9999.0f;
        min_local = 9999.0f;
        buscando_pico = true;

        return false;
    }

    *rr_out = (uint16_t)rr_suavizado;
    return true;
}


static bool compute_spo2(uint16_t *spo2_out)
{
    float dc_ir_mean = mean(ir_raw);
    float dc_red_mean = mean(red_raw);

    float ac_ir = 0;
    float ac_red = 0;

    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
    {
        float ir_ac = ir_raw[i] - dc_ir_mean;
        float red_ac = red_raw[i] - dc_red_mean;

        ac_ir += ir_ac * ir_ac;
        ac_red += red_ac * red_ac;
    }

    ac_ir = sqrtf(ac_ir / PPG_WINDOW_SAMPLES);
    ac_red = sqrtf(ac_red / PPG_WINDOW_SAMPLES);

    if (dc_ir_mean == 0 || ac_ir == 0)
        return false;

    float R = (ac_red / dc_red_mean) / (ac_ir / dc_ir_mean);

    /* Calibración típica empírica */
    float spo2 = 110.0f - 25.0f * R;

    if (spo2 < 80 || spo2 > 100)
        return false;

    *spo2_out = (uint16_t)spo2;
    return true;
}
void ppg_add_sample(uint32_t ir, uint32_t red)
{
    ir_raw[idx]  = (float)ir;
    red_raw[idx] = (float)red;

    ir_filt[idx] = bandpass((float)ir);
    resp_filt[idx] = respiratory_filter((float)ir);

    idx++;

    if (idx >= PPG_WINDOW_SAMPLES)
    {
        idx = 0;
        buffer_full = true;
    }
    global_sample_counter++;
}
/* ================= INICIALIZACIÓN ================= */
void ppg_init(void)
{
    idx = 0;
    buffer_full = false;

    /* Reset buffers */
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
    {
        ir_raw[i]  = 0.0f;
        red_raw[i] = 0.0f;
        ir_filt[i] = 0.0f;
    }

    /* Reset filtros IR */
    dc_ir = 0.0f;
    dc_red = 0.0f;

    hp_y = 0.0f;
    hp_x_prev = 0.0f;
    lp_y = 0.0f;
}

/* ================= CÁLCULO FINAL ================= */
bool ppg_compute(ppg_results_t *out)
{
    if (!buffer_full || out == NULL)
        return false;

    bool apnea_flag = false;

    bool ok_hr   = compute_hr(&out->hr);
    bool ok_rr   = compute_rr(&out->rr, &apnea_flag);
    bool ok_spo2 = compute_spo2(&out->spo2);

    out->apnea = apnea_flag;

    /* Definición de validez */
    out->valid = ok_hr;

    buffer_full = false;

    return out->valid;
}
