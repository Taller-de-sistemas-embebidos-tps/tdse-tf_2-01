#ifndef __PPG_PROCESSING_H__
#define __PPG_PROCESSING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ================= CONFIGURACIÓN ================= */

#define PPG_FS_HZ              50u
#define PPG_WINDOW_SEC         10u
#define PPG_WINDOW_SAMPLES     (PPG_FS_HZ * PPG_WINDOW_SEC)

/* Rangos fisiológicos */
#define HR_MIN_BPM             30u
#define HR_MAX_BPM             200u

#define RR_MIN_RPM             6u
#define RR_MAX_RPM             30u

#define SPO2_MIN               70u
#define SPO2_MAX               100u

/* ================= TIPOS ================= */


typedef struct
{
    uint16_t spo2;
    uint16_t hr;
    uint16_t rr;

    bool apnea;
    bool valid;

} ppg_results_t;
/* ================= API ================= */

void ppg_init(void);
void ppg_add_sample(uint32_t ir, uint32_t red);
bool ppg_compute(ppg_results_t *out);

#ifdef __cplusplus
}
#endif

#endif
