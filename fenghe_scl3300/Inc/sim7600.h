#ifndef __SIM7600_H
#define __SIM7600_H

#include <stdbool.h>

#define MODEM_POWER_OFF()       HAL_GPIO_WritePin(LTE_POWERON_PORT, LTE_POWERON_PIN, GPIO_PIN_RESET)
#define MODEM_POWER_ON()    	HAL_GPIO_WritePin(LTE_POWERON_PORT, LTE_POWERON_PIN, GPIO_PIN_SET)

typedef enum
{
    SYNC_TIME_FROM_GPS = 0x01,
    SYNC_TIME_FROM_CLOUD,
}SyncTimeSrc_t;

extern SyncTimeSrc_t SyncTimeSrc;

extern bool fNeedSyncTime;

void ModemHardReboot(void);

void TboxSyncTime(void);

#endif
