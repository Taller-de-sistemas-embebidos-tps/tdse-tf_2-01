#ifndef BT_COMM_H
#define BT_COMM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t resp_rate;
    uint32_t apneas;
    uint32_t spo2;
    uint32_t low_spo2_events;
    uint32_t heart_rate;
    uint32_t hr_issues;
} bt_data_t;

void bt_comm_init(void);
void bt_comm_set_connected(bool state);
void bt_comm_send_data(bt_data_t *data);
void bt_comm_update(void);

#endif