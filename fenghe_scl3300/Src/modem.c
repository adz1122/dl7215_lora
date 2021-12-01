#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "modem.h"
#include "usart.h"
#include "sim7600.h"
#include "tim.h"
#include "iap.h"


#define	USE_DEBUG_MSG	(1)

#ifdef USE_DEBUG_MSG
#define DEBUG(format, ...)												\
	do {																\
			printf("[DEBUG] %s: "format, __func__, ##__VA_ARGS__);		\
	} while (0)
#else
#define DEBUG(format, ...)
#endif


ATCmdPara_t ATCmdPara;
ModemProcStep_t ModemProcStep;
ModemInitStep_t ModemInitStep;

ModemErrInfo_t ModemErr;
ModemMode_t CurModemMode;
ModemUpdateStatStep_t CheckModemStatStep;
EstablishFtpConnStep_t EstablishFtpConnStep;

static bool fWaittingRely;
static bool fModemPwrOn;
static bool fModemRdy;

static uint8_t u8gLteCheckCregCnt;
static uint8_t u8gModemRebootCnt;

static uint16_t rdCacheCnt = 0;

char CmdStr[100] = {0};

/* AT Commands */
const uint8_t CmdQueryCSQ[] = "AT+CSQ\r\n";
const uint8_t CmdAT[] = "AT\r\n";   
const uint8_t CmdATE0[] = "ATE0\r\n"; 				   			//Echo mode off
//const  uint8_t CmdAutoCSQ[] = "AT+AUTOCSQ=1,1\r\n";				//CSQ automatic report only after <rssi> or <ber> is changed
const uint8_t CmdCheckSIM[] = "AT+CPIN?\r\n"; 					//��������
const uint8_t CmdRdCICCID[] = "AT+CICCID\r\n";					//Read the ICCID from SIM card
const uint8_t CmdSelectLte[] = "AT+CNMP=38\r\n";				////ѡ��4G���� 2 Automatic; 13 GSM Only;14 WCDMA Only;38  LTE Only;59 TDS-CDMA Only
const uint8_t CmdInquireUESysInfo[] = "AT+CPSI?\r\n";			//Inquiring UE system information
const uint8_t CmdCheckREG[] = "AT+CREG?\r\n";					//network registration status
const uint8_t CmdCheckCEREG[] = "AT+CEREG?\r\n";				//����ע���ѯ+ CEREG: 0,1
const uint8_t CmdLteOff[] = "AT+CPOF\r\n";						//�ػ�

//FTP����
const uint8_t CmdSetFtpPortPrefix[] = "AT+CFTPPORT="; 
const uint8_t CmdSetFtpIpPrefix[] = "AT+CFTPSERV="; 
const uint8_t CmdFtpUserNamePrefix[] = "AT+CFTPUN=";
const uint8_t CmdFtpUserPswPrefix[] = "AT+CFTPPW=";
const uint8_t CmdFtpGetFileCachePrefix[] = "AT+CFTPGET=";

const uint8_t CmdSetFtpPort[] = "AT+CFTPPORT=21\r\n"; 
const uint8_t CmdSetFtpIp[] = "AT+CFTPSERV=\"94.193.176.180\"\r\n"; 
const uint8_t CmdSetFtpMode[] = "AT+CFTPMODE=1\r\n"; 		//FTP�����ʹ�ñ���ģʽ���ͻ���ͨ���������ݶ˿���get�ļ����ݣ�
const uint8_t CmdSetFtpType[] = "AT+CFTPTYPE=A\r\n"; 		//I �C binary type   A �C ASCII type.
const uint8_t CmdFtpUserName[] = "AT+CFTPUN=\"uftp\"\r\n";
const uint8_t CmdFtpUserPsw[] = "AT+CFTPPW=\"abc123456\"\r\n";
const uint8_t CmdFtpGetFileCache[] = "AT+CFTPGET=\"/app_test.bin\",0,1\r\n";	//  �ļ�λ�ã� /��ʾFTP�������ĸ�Ŀ¼�� ��һ��0��ʾ������ʼλ��(���ڶϵ�����)�� 1��ʾʹ��cache��ʽ��ȡ����
const uint8_t CmdFtpQueryCache[] = "AT+CFTPCACHERD?\r\n";//��ѯcache�е����ݳ��� (�����͸�ָ���ѯcache�е����ݳ���ʱ�� ����ȵ���һ��ָ����յ�Ӧ��)
const uint8_t CmdFtpRdCache[] = "AT+CFTPCACHERD\r\n";	//����ѯ��cache�е����ݳ��Ⱥ� ÿ����һ�θ����  �����1024�ֽڵ�����


/*****************************************************************************
*����: ���ڷ��Ͷ��η�װ
*����:
*****************************************************************************/
#define ATCmdTxFuncUser(buf, BufLen)		DMA_Uart2Transmit((uint8_t*)buf, BufLen)


/*****************************************************************************
*������:ATCmdProc
*����: SIM7600CEģ��ATָ���
*����:
*����ֵ:
*****************************************************************************/
static ATCmdProcRes_t ATCmdProc(ATCmdPara_t *CmdPara)
{
	bool fProcErr = false;

	// USER_ASSERT(CmdPara);	
	CmdPara->AckStrStartAddr = NULL;	
	if (!fWaittingRely)
	{
		if (ATCmdTxFuncUser(CmdPara->SendCmd, CmdPara->CmdLen) == true)
		{
			fWaittingRely = true;
			u16gRemainWaitDly = CmdPara->RxCpltJudgeDly;//in GET_FILE_BY_CACHE RxCpltJudgeDly is uint32_t =250000 over 16 bit u16gRemainWaitDly
			memset(Uart2Para.TotalRxBuf, 0, UART2_RX_BUF_MAX_LEN);
			Uart2Para.TotalRxCnt = 0;
			DEBUG("cmdstr:%s", CmdPara->SendCmd);
		}
		else
		{
			fProcErr = true;
		}
	}
	else
	{
		if (u16gRemainWaitDly > 0)	//ʣ����ʱ
		{
			if (fUart2RecvFrameCplt)	//uart2���յ�һ֡����
			{
				fUart2RecvFrameCplt = false;
				DEBUG("ģ���յ�������Ϊ: %s",Uart2Para.TotalRxBuf);
				CmdPara->AckStrStartAddr = strstr((const char*)Uart2Para.TotalRxBuf, (const char*)CmdPara->ExpectAckStr);
				if (CmdPara->AckStrStartAddr)
				{
					fWaittingRely = false;
					return SET_OK;
				}
				else
				{
					if ((strstr((char*)Uart2Para.TotalRxBuf, "ERROR")) || (strstr((char*)Uart2Para.TotalRxBuf, "NO CARRIER")))
					{
						fWaittingRely = false;
						DEBUG("AT�������ô����ʧ�ܣ�AT����Ϊ:%s",CmdPara->SendCmd);
						if (CmdPara->fErrNeedMoreJudge)
						{
							return SET_ERROR;
						}
						else
						{
							fProcErr = true;
						}
					}
				}
			}
		}
		else
		{
			DEBUG("�ȴ�ģ��ظ���ʱ���ѷ��͵�����Ϊ:%s",CmdPara->SendCmd);
			fProcErr = true;
		}
	}

	/* ����ʧ�ܻ���ճ�ʱ,�Զ��ط� */
	if (fProcErr)
	{
		if (++CmdPara->ReplyErrCnt >= 3)
		{
			CmdPara->ReplyErrCnt = 0;
			CmdPara->fDeadErr = true;
			return SET_ERROR;
		}
		else
		{
			fWaittingRely = false;
		}
	}

	return WAIT_REPLY;

}


/*****************************************************************************
*������:ModemParaInit
*����: modem������ʼ��
*����:
*����ֵ:
*****************************************************************************/
void ModemParaInit(void)
{
	fWaittingRely = false;
	fModemPwrOn = false;
	fModemRdy = false;

	ModemProcStep = MODEM_INIT;
	ModemInitStep = TEST_AT_CMD;
	ModemErr = NO_ERR;
	CurModemMode = IDEL_MODE;
	CheckModemStatStep = ENTER_AT_CMD;
	u8gLteCheckCregCnt = 0;

	memset(&ATCmdPara, 0, sizeof(ATCmdPara));
	ATCmdPara.RxCpltJudgeDly = Uart2Para.RxCpltJudgeDlySetting;
	// ATCmdPara.BufLen = UART3_RX_BUF_MAX_LEN;
}

/*****************************************************************************
*������:ModemInit
*����: ģ��AT�����ʼ��
*����:
*����ֵ:
*****************************************************************************/
static void ModemInit(void)
{
	ATCmdProcRes_t ATCmdProcRes;
	static bool fConfigPara = false;

	switch (ModemInitStep)
	{
		case TEST_AT_CMD:									//AT��������
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdAT;
				ATCmdPara.CmdLen = sizeof(CmdAT);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				ModemInitStep = DISABLE_AT_ECHO;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = TEST_AT_CMD_ERR;
				DEBUG("AT����ʧ��,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case DISABLE_AT_ECHO:								//�رջ���
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdATE0;
				ATCmdPara.CmdLen = sizeof(CmdATE0);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				ModemInitStep = CHECK_SIM_CARD;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = DISABLE_AT_ECHO_ERR;
				DEBUG("�رջ���ʧ��,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case CHECK_SIM_CARD:								//��ѯ�Ƿ������
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdCheckSIM;
				ATCmdPara.ExpectAckStr = "+CPIN: READY";
				ATCmdPara.CmdLen = sizeof(CmdCheckSIM);
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				ModemInitStep = QUERY_CSQ;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = CHECK_SIM_ERR;
				DEBUG("δ��⵽SIM�����ȡSIM��ʧ��,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;
		
		case QUERY_CSQ:										//��ѯCSQ
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdQueryCSQ;
				ATCmdPara.CmdLen = sizeof(CmdQueryCSQ);
				ATCmdPara.ExpectAckStr = "+CSQ:";				//+CSQ: 22,0
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				ModemProcStep = MODEM_WAIT_CREG;
				DEBUG("SIM7600CEģ���ʼ�����.\r\n ");
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = QUERY_CSQ_ERR;
				DEBUG("������ѯCSQ��Ϣʧ��,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		default:
			break;
	}
}

/*****************************************************************************
*������:WaitCREG
*����:�ȴ�ģ��ע��ɹ�
*����:
*����ֵ:
*****************************************************************************/
static void WaitCREG(void)
{
	ATCmdProcRes_t ATCmdProcRes;
	static bool fConfigPara = false;

	if (!fConfigPara)
	{
		fConfigPara = true;
		ATCmdPara.SendCmd = CmdCheckREG;
		ATCmdPara.CmdLen = sizeof(CmdCheckREG);
		ATCmdPara.ExpectAckStr = "+CREG:";
		ATCmdPara.fErrNeedMoreJudge = false;
		ATCmdPara.RxCpltJudgeDly = 1000;
	}
	ATCmdProcRes = ATCmdProc(&ATCmdPara);
	if (ATCmdProcRes == SET_OK)
	{
		fConfigPara = false;
		ATCmdPara.ReplyErrCnt = 0;
		if ((*(ATCmdPara.AckStrStartAddr + 9) == '1') || (*(ATCmdPara.AckStrStartAddr + 9) == '5'))
		{
			ATCmdPara.ReplyErrCnt = 0;
			ModemProcStep = GET_FILE_FROM_FTP;					/* ���ӷ��������� */
		}
		else
		{
			if (++u8gLteCheckCregCnt >= 12)
			{
				DEBUG("4Gģ������ע��ʧ��,�����ź�ǿ�Ȼ�SIM�Ƿ�Ƿ��.\n");
				u8gLteCheckCregCnt = 0;
				HAL_Delay(60000);	/* ��ʱ1min */		
			}
			else
			{
				HAL_Delay(10000);	/* ��ʱ10s */
			}
		}
	}
	else if (ATCmdProcRes == SET_ERROR)
	{
		ModemErr = NET_REG_ERR;
		DEBUG("����ע��ʧ��,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
	}
}

/*****************************************************************************
*������:GetFileFromFtp
*����: �����������FTP����
*����:
*����ֵ:
*****************************************************************************/
static void GetFileFromFtp(void)
{
	ATCmdProcRes_t ATCmdProcRes;
	static bool fConfigPara = false;
	uint16_t StartAddrOffset = 0;
	static uint32_t offset = 0;
	uint32_t CurRdLen = 0;
	bool ret = false;

	switch (EstablishFtpConnStep)
	{
		case SET_FTP_SERV_PORT:
			if (!fConfigPara)
			{
				fConfigPara = true;
				offset = 0;
				NewAppSize = 0;
				rdCacheCnt = 0;
    			snprintf(CmdStr, sizeof(CmdStr), "%s%d%s", (char*)CmdSetFtpPortPrefix, FwUpdateMsg.items.FtpPort, "\r\n");
				ATCmdPara.SendCmd = (const uint8_t*)CmdStr;
				ATCmdPara.CmdLen = strlen(CmdStr);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = SET_FTP_SERV_IP;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP�������˿ڴ���,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case SET_FTP_SERV_IP:
			if (!fConfigPara)
			{
				fConfigPara = true;
				snprintf(CmdStr, sizeof(CmdStr), "%s\"%s\"%s", (char*)CmdSetFtpIpPrefix, FwUpdateMsg.items.FtpIp, "\r\n");
				ATCmdPara.SendCmd = (const uint8_t*)CmdStr;
				ATCmdPara.CmdLen = strlen(CmdStr);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = SET_FTP_MODE;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP������IP����,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case SET_FTP_MODE:
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdSetFtpMode;
				ATCmdPara.CmdLen = sizeof(CmdSetFtpMode);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = SET_FTP_TYPE;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP������ģʽ����,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case SET_FTP_TYPE:
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdSetFtpType;
				ATCmdPara.CmdLen = sizeof(CmdSetFtpType);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = SET_FTP_USER_NAME;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP���������ʹ���,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;
		
		case SET_FTP_USER_NAME:
			if (!fConfigPara)
			{
				fConfigPara = true;
				snprintf(CmdStr, sizeof(CmdStr), "%s\"%s\"%s", (char*)CmdFtpUserNamePrefix, FwUpdateMsg.items.FtpUserName, "\r\n");
				ATCmdPara.SendCmd = (const uint8_t*)CmdStr;
				ATCmdPara.CmdLen = strlen(CmdStr);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = SET_FTP_PSW;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP�������û�������,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case SET_FTP_PSW:
			if (!fConfigPara)
			{
				fConfigPara = true;
				snprintf(CmdStr, sizeof(CmdStr), "%s\"%s\"%s", (char*)CmdFtpUserPswPrefix, FwUpdateMsg.items.FtpUserPsw, "\r\n");
				ATCmdPara.SendCmd = (const uint8_t*)CmdStr;
				ATCmdPara.CmdLen = strlen(CmdStr);
				ATCmdPara.ExpectAckStr = "OK";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = GET_FILE_BY_CACHE;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("����FTP�������������,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case GET_FILE_BY_CACHE:
			if (!fConfigPara)
			{
				fConfigPara = true;
				snprintf(CmdStr, sizeof(CmdStr), "%s\"%s\"%s", (char*)CmdFtpGetFileCachePrefix, FwUpdateMsg.items.name, ",0,1\r\n");
				ATCmdPara.SendCmd = (const uint8_t*)CmdStr;
				ATCmdPara.CmdLen = strlen(CmdStr);
				ATCmdPara.ExpectAckStr = "+CFTP: RECV EVENT";	//OK\r\n 	+CFTP: RECV EVENT\r\n	+CFTPGET: 0\r\n  \\�����յ� ���Ӧ��� ��ѯcache�������ڵ����ݲ�����Ч��
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 250000;//250000=0x3D090 ��ֵ��16λ�ľ��� 0xD090=53392 ��53.392s
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				EstablishFtpConnStep = QUERY_FTP_CACHE;
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("��FTP��������ȡ�ļ�����,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case QUERY_FTP_CACHE:
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdFtpQueryCache;
				ATCmdPara.CmdLen = sizeof(CmdFtpQueryCache);
				ATCmdPara.ExpectAckStr = "+CFTPCACHERD:";	//+CFTPCACHERD: 63840
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 1000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				sscanf(ATCmdPara.AckStrStartAddr,"+CFTPCACHERD: %d\r\n",&NewAppSize);
				if (NewAppSize != FwUpdateMsg.items.size)
				{
					FwUpdateMsg.items.fUpdateSuccess = false;
					updateStep = JUMP_TO_APP;
					return;
				}
				rdCacheCnt = NewAppSize / 1024;
				if (NewAppSize % 1024)
				{
					rdCacheCnt++;
				}
				if (rdCacheCnt > 0)
				{
					EstablishFtpConnStep = RD_FTP_CACHE;
				}
				else
				{
					/* FTP��������û�ļ� */
					updateStep = JUMP_TO_APP;
					return;
				}
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("��ѯFTP�������,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}
			break;

		case RD_FTP_CACHE:
			if (!fConfigPara)
			{
				fConfigPara = true;
				ATCmdPara.SendCmd = CmdFtpRdCache;
				ATCmdPara.CmdLen = sizeof(CmdFtpRdCache);
				ATCmdPara.ExpectAckStr = "+CFTPGET: DATA";
				ATCmdPara.fErrNeedMoreJudge = false;
				ATCmdPara.RxCpltJudgeDly = 2000;
			}
			ATCmdProcRes = ATCmdProc(&ATCmdPara);
			if (ATCmdProcRes == SET_OK)
			{
				fConfigPara = false;
				ATCmdPara.ReplyErrCnt = 0;
				//�ҵ��������ݵĳ���
				sscanf(ATCmdPara.AckStrStartAddr,"+CFTPGET: DATA,%d\r\n*",&CurRdLen);
				StartAddrOffset = strlen("+CFTPGET: DATA,") + 2;	// +2: \r\n
				//FTP������� 1024 bytes
				if (CurRdLen / 1000)
				{
					StartAddrOffset += 4;	
				}
				else if (CurRdLen / 100)
				{
					StartAddrOffset += 3;	
				}
				else if (CurRdLen / 10)
				{
					StartAddrOffset += 2;	
				}
				else
				{
					StartAddrOffset += 1;
				}
				
				//д�뱸����
				memset(u8gIapBuf, 0, sizeof(u8gIapBuf));
				memcpy(u8gIapBuf, (uint8_t *)(ATCmdPara.AckStrStartAddr+StartAddrOffset), CurRdLen);

				HAL_Delay(10);
			//	ret = IapWrAppBin(APP_BAK_START_ADDR + offset, u8gIapBuf, CurRdLen);
//				if (!ret)
//				{
//					DEBUG("�¹̼����س���д��Flash������ʧ�ܣ�\r\n");
//					updateStep = JUMP_TO_APP;
//					break;
//				}
				offset += CurRdLen;
				rdCacheCnt--;
				//��ȡ���
				if (!rdCacheCnt)
				{
					DEBUG("�¹̼�������ɣ�\r\n");
					updateStep = UPDATE_APP;
				}
			}
			else if (ATCmdProcRes == SET_ERROR)
			{
				ModemErr = GET_FILE_FROM_FTP_ERR;
				DEBUG("��FTP�����ȡ���ݳ���,ATCmdProcRes=0x%x\r\n",ATCmdProcRes);
			}	
			break;

		default:
			break;
	}
}

/*****************************************************************************
*������:ErrMonitor
*����: SIM7600CE���ϼ��
*����:
*����ֵ:
*****************************************************************************/
static void ErrMonitor(void)
{
	if (ModemErr != NO_ERR)
	{
		if (++u8gModemRebootCnt >= 3)
		{
			/* �������£���תAPP */
			DEBUG("4Gģ��δ������\n");
			updateStep = JUMP_TO_APP;
		}
		else
		{
			/* ����4Gģ�� */
			ModemHardReboot();
			fModemRdy = false;
			fModemPwrOn = false;
		}
	}
}


/*****************************************************************************
*������:ModemProcTask
*����: SIM7600CEģ��ҵ����
*����:
*����ֵ:
*****************************************************************************/
void ModemProcTask(void)
{
	if (!fModemRdy)
	{
		if (!fModemPwrOn)
		{
			if (fUart2RecvFrameCplt)	//uart2���յ�һ֡����
			{
				fUart2RecvFrameCplt = false;
				if(strstr((const char*)Uart2Para.TotalRxBuf, "PB DONE"))
					fModemPwrOn = true;
				DEBUG("ģ���յ�������Ϊ: %s",Uart2Para.TotalRxBuf);
				memset(Uart2Para.TotalRxBuf, 0, UART2_RX_BUF_MAX_LEN);
				Uart2Para.TotalRxCnt = 0;
				u16gTimModemRdyDly = 2000; 
				return;
			}
			else
			{
				return;
			}
		}
		else
		{
			if (!u16gTimModemRdyDly)
			{
				fModemRdy = true;
			}
			else
			{
				return;
			}
		}
	}

	switch (ModemProcStep)
	{
		case MODEM_INIT:
			ModemInit();
			break;

		case MODEM_WAIT_CREG:
			WaitCREG();
			break;

		case GET_FILE_FROM_FTP:	
			GetFileFromFtp();
			break;

		default:
			break;
	}
	ErrMonitor();
}


