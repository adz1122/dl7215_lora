#include "iap.h"
#include "modem.h"
#include "string.h"
#include "stdio.h"

#ifdef USE_DEBUG_MSG
#define DEBUG(format, ...)												\
	do {																\
			printf("[DEBUG] %s: "format, __func__, ##__VA_ARGS__);		\
	} while (0)
#else
#define DEBUG(format, ...)
#endif


FwUpdateMsg_t	FwUpdateMsg, FwUpdateMsgBak;
updateStep_t updateStep = CHECK_UPDATE_FLAG;
uint32_t NewAppSize;
uint16_t u16gIapBuf[HALF_WORD_PROG_SIZE];   //1024

uint8_t u8gIapBuf[MCU_PAGE_SIZE];


typedef void (*pFunction)(void);		//定义函数指针
static pFunction Jump_To_Application;
static uint32_t  JumpAddress;			//跳转地址

void msg_init(){
	
	strcpy((char *)FwUpdateMsg.items.FtpIp, "120.24.200.242");
	FwUpdateMsg.items.FtpPort = 21;
	strcpy((char *)FwUpdateMsg.items.FtpUserName, "jpsss-ftp");
	strcpy((char *)FwUpdateMsg.items.FtpUserPsw, "Sz9789");
	strcpy((char *)FwUpdateMsg.items.name, "/localuser/jpsss-ftp/test.txt");
	FwUpdateMsg.items.size = 10;
	updateStep = GET_UPDATE_FILE;
}

void UpdateApp(void)
{
	// JumpToapp();
    switch (updateStep)
    {
        case CHECK_UPDATE_FLAG:
//			CheckUpdate();
            break;

        case GET_UPDATE_FILE:
            ModemProcTask();
            break;

        case UPDATE_APP:
//            updateAppProc();
			printf("get data from ftp server successfully\n");
            break;

        case JUMP_TO_APP:
//           JumpToapp();
            break;

        default:
            break;
    }
}

