#include "ppg_processing.h"
#include <math.h>

/* ================= BUFFER ================= */

static float ir_buf[PPG_WINDOW_SAMPLES];
static float red_buf[PPG_WINDOW_SAMPLES];

static uint16_t sample_idx = 0;
static bool buffer_full = false;

/* ================= FILTROS ================= */

/* IIR pasa banda simple para HR (~0.5–4 Hz) */
static float hp_state = 0;
static float lp_state = 0;

static float bandpass_filter(float x)
{
    /* High pass ~0.5 Hz */
    float hp = x - hp_state + 0.95f * hp_state;
    hp_state = x;

    /* Low pass ~4 Hz */
    lp_state = lp_state + 0.2f * (hp - lp_state);

    return lp_state;
}

/* ================= UTILIDADES ================= */

static float mean(float *x)
{
    float acc = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
        acc += x[i];

    return acc / PPG_WINDOW_SAMPLES;
}

static float rms(float *x, float dc)
{
    float acc = 0;
    float v;

    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
    {
        v = x[i] - dc;
        acc += v * v;
    }

    return sqrtf(acc / PPG_WINDOW_SAMPLES);
}

/* ================= HR ================= */

static bool compute_hr(uint16_t *hr_out)
{
    uint16_t peaks = 0;
    uint16_t last_peak = 0;
    uint32_t period_sum = 0;

    const uint16_t min_samples_between_beats = PPG_FS_HZ / 3; // 200 bpm max

    float dc = mean(ir_buf);

    for (uint16_t i = 1; i < PPG_WINDOW_SAMPLES - 1; i++)
    {
        if (ir_buf[i] > dc &&
            ir_buf[i] > ir_buf[i - 1] &&
            ir_buf[i] > ir_buf[i + 1])
        {
            if ((i - last_peak) > min_samples_between_beats)
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

    *hr_out = (uint16_t)hr;
    return true;
}

/* ================= RR ================= */
/* Respiración desde envolvente */

static bool compute_rr(uint16_t *rr_out, bool *apnea_flag)
{
    float envelope[PPG_WINDOW_SAMPLES];
    float dc = mean(ir_buf);

    /* Envolvente */
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
        envelope[i] = fabsf(ir_buf[i] - dc);

    /* Filtro pasa bajo respiratorio */
    float lp = 0;
    for (uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++)
    {
        lp += 0.05f * (envelope[i] - lp);
        envelope[i] = lp;
    }

    float env_dc = mean(envelope);

    uint16_t peaks = 0;
    uint16_t last_peak = 0;
    uint32_t period_sum = 0;

    const uint16_t apnea_threshold_samples = PPG_FS_HZ * 10;  // 10 segundos
    const uint16_t min_samples_between_breaths = PPG_FS_HZ * 2;

    uint16_t last_detected_peak = 0;

    for (uint16_t i = 1; i < PPG_WINDOW_SAMPLES - 1; i++)
    {
        if (envelope[i] > env_dc &&
            envelope[i] > envelope[i - 1] &&
            envelope[i] > envelope[i + 1])
        {
            if ((i - last_peak) > min_samples_between_breaths)
            {
                if (last_peak != 0)
                {
                    period_sum += (i - last_peak);
                    peaks++;
                }

                last_peak = i;
                last_detected_peak = i;
            }
        }
    }

    /* -------- Detección de apnea -------- */

    if ((PPG_WINDOW_SAMPLES - last_detected_peak) > apnea_threshold_samples)
    {
        *apnea_flag = true;
        return false;
    }
    else
    {
        *apnea_flag = false;
    }

    if (peaks < 1)
        return false;

    uint32_t avg_period = period_sum / peaks;
    uint32_t rr = (60u * PPG_FS_HZ) / avg_period;

    if (rr < RR_MIN_RPM || rr > RR_MAX_RPM)
        return false;

    *rr_out = (uint16_t)rr;
    return true;
}

void ppg_add_sample(uint32_t ir, uint32_t red)
{
    float filtered = bandpass_filter((float)ir);

    ir_buf[sample_idx]  = filtered;
    red_buf[sample_idx] = (float)red;

    sample_idx++;

    if (sample_idx >= PPG_WINDOW_SAMPLES)
    {
        sample_idx = 0;
        buffer_full = true;
    }
}

/* ================= INICIALIZACIÓN ================= */
void ppg_init(void)
{
    sample_idx = 0;
    buffer_full = false;
    hp_state = 0.0f;
    lp_state = 0.0f;

    for(uint16_t i = 0; i < PPG_WINDOW_SAMPLES; i++) {
        ir_buf[i] = 0.0f;
        red_buf[i] = 0.0f;
    }
}

/* ================= CÁLCULO FINAL ================= */
bool ppg_compute(ppg_results_t *out)
{
    if (!buffer_full || out == NULL)
        return false;

    bool apnea_flag = false;

    // Calculamos HR y RR (Descomentamos RR para quitar el warning)
    bool ok_hr = compute_hr(&out->hr);
    bool ok_rr = compute_rr(&out->rr, &apnea_flag);

    // TODO: Cuando implementes SpO2, agrégalo aquí
    out->spo2 = 0;

    out->apnea = apnea_flag;

    // Es válido si al menos pudimos sacar el ritmo cardíaco
    out->valid = ok_hr;

    // IMPORTANTE: Reiniciamos la bandera para esperar la siguiente ventana de datos
    buffer_full = false;

    return out->valid;
}
