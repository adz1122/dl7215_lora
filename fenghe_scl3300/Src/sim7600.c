#include "main.h"
#include "sim7600.h"
#include "delay.h"


/* STM32����SIM7600 IO�ܽ� */


bool fPoweronSyncTime;
bool fNeedSyncTime;
uint32_t u32gTimeHoursBak;
SyncTimeSrc_t SyncTimeSrc;




/*****************************************************************************
*������:ModemReboot
*����: SIM7600CEģ�鸴λ
*����:
*����ֵ:
*****************************************************************************/
void ModemHardReboot(void)
{   
    MODEM_POWER_OFF();
    delay_ms(100);
    MODEM_POWER_ON();
    delay_ms(100);
    // ModemProcStep = MODEM_INIT;
    // ModemInitStep = TEST_AT_CMD;
    // u8gTimeModemRun = 0;
	delay_s(13);
}
