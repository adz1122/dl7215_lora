#ifndef	__MODEM_H
#define	__MODEM_H

#include "main.h"
#include <stdbool.h>

#ifndef _PACK_START
	#define _PACK_START
#endif
#ifndef _PACK_END
	#define _PACK_END	__attribute__((packed))
#endif




typedef enum
{
	SET_OK = 0x00,
	SET_ERROR,
	SET_TIMEOUT,
	SEND_FAILED,
	WAIT_REPLY,
}ATCmdProcRes_t;


typedef enum
{
	MODEM_INIT = 0x00,
    MODEM_WAIT_CREG,
	GET_FILE_FROM_FTP,
	MODEM_ERR_PROC,
	MODEM_IDLE,
}ModemProcStep_t;

typedef enum
{
	TEST_AT_CMD = 0x00,
	DISABLE_AT_ECHO,
	QUERY_CSQ,
	CLOSE_GNSS,
	OPEN_GNSS,
	CHECK_SIM_CARD,
	RD_SIM_ICCID,
}ModemInitStep_t;



typedef enum
{
	SET_FTP_SERV_PORT = 0x00,
	SET_FTP_MODE,
	SET_FTP_TYPE,
	SET_FTP_SERV_IP,
	SET_FTP_USER_NAME,
	SET_FTP_PSW,
	GET_FILE_BY_CACHE,
	QUERY_FTP_CACHE,
	RD_FTP_CACHE

}EstablishFtpConnStep_t;


typedef enum
{
	NO_ERR = 0x00,
	TEST_AT_CMD_ERR,
	DISABLE_AT_ECHO_ERR,
	CHECK_SIM_ERR,
	CLOSE_GNSS_ERR,
	OPEN_GNSS_ERR,
	ENABLE_AUTO_CSQ_ERR,
	QUERY_CSQ_ERR,
	NET_REG_ERR,	
	GET_FILE_FROM_FTP_ERR,
}ModemErrInfo_t;

typedef enum
{
	NO_CARE = 0x00,
	RECONNECT,
	REBOOT,
	SHUTDOWN,
}ModemErrProcMethod_t;

typedef enum
{
	ENTER_AT_CMD = 0x00,
	UPDATE_CSQ,
	UPDATE_GPS,
	QUERY_CONN_STAT,
	RETURN_DATA_MODE,
}ModemUpdateStatStep_t;

typedef enum
{
	IDEL_MODE = 0x00,
	CMD_MODE = 0x01,
	DATA_MODE = 0x02,
}ModemMode_t;


struct _PACK_START _ATCmdProcPara
{
	bool 				fErrNeedMoreJudge;			//错误信息需要进一步解析判断
	bool 				fDeadErr;
	char 				*ExpectAckStr;				//expected reply string
	char				*AckStrStartAddr;			//reply string start address
	uint8_t				*RxBuf;
	uint32_t			RxCpltJudgeDly;
	const uint8_t		*SendCmd;
	uint16_t 			CmdLen;	
	uint8_t 			ReplyErrCnt;
	
}_PACK_END;

typedef struct _ATCmdProcPara 	ATCmdPara_t;


extern ATCmdPara_t ATCmdPara;
extern ModemProcStep_t ModemProcStep;
extern ModemInitStep_t ModemInitStep;
extern ModemErrInfo_t ModemErr;
extern ModemMode_t CurModemMode;
extern ModemUpdateStatStep_t CheckModemStatStep;


void ModemParaInit(void);
void CheckModemStat(void);
void ModemProcTask(void);

#endif
