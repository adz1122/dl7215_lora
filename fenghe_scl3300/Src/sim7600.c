#include "main.h"
#include "sim7600.h"
#include "delay.h"


/* STM32控制SIM7600 IO管脚 */


bool fPoweronSyncTime;
bool fNeedSyncTime;
uint32_t u32gTimeHoursBak;
SyncTimeSrc_t SyncTimeSrc;




/*****************************************************************************
*函数名:ModemReboot
*描述: SIM7600CE模块复位
*参数:
*返回值:
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
