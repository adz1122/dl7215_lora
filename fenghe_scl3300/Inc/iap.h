#ifndef __IAP_H
#define __IAP_H

#include "main.h"
#include "stdbool.h"



//STM32F105RC Flash(256k) ����
//bootloader:	  	0x08000000		0x6000	//24k
//app:  		    0x08006000		0x1B800	//110k
//app_bak:  	  	0x08021800		0x1B800	//110k
//data:			    0x0803D000		0x3000	//12k

#define APP_START_ADDR          (0x08006000)
#define APP_BAK_START_ADDR      (0x08021800)
#define DATA_START_ADDR      	  (0x0803D000)

#define MCU_PAGE_SIZE                           (0x800)    /* 2 Kbytes */

//data ��: ����IAP���ݼ�¼���Ƿ���±�ǡ��Ƿ���³ɹ����汾��Ϣ���ļ���С���ļ�CRC32У����Ϣ��IAP������Ϣ��FTP��������ַ��FTP�˺š�FTP����ȣ�


// ���ֱ�����Ϊ2Kbytes 1024 * 2
#define	HALF_WORD_PROG_SIZE		            (MCU_PAGE_SIZE / 2)//(0x400)  //1024

//FIXME: ��������Э����GB32960 Զ�����������ʽ��һ��
//�Զ��������ʽ(��Ҫ��bootloader��ͬ������)��
//�Ƿ���Ҫ���±��
//���³ɹ����
//�ļ�����[16btyes]
//�ļ���С[4bytes]
//�ļ�У��[4bytes]
//�̼��汾[4bytes]
//Ӳ���汾[4bytes]
//FTP�˺�[8bytes]
//FTP����[8bytes]
//FTP�������˿ں�[2bytes]
//FTP��������ַ[Ԥ��50bytes]
typedef union 
{
	uint16_t	u16buf[62];
	struct
	{
		bool        fNeedUpdate;
		bool     	fUpdateSuccess;
		uint8_t 	name[50];
		uint32_t	size;
		uint32_t	crc;
		uint32_t	FwVer;
		uint32_t	HwVer;
		uint8_t		FtpUserName[10];
		uint8_t		FtpUserPsw[12];
		uint16_t	FtpPort;
		uint8_t		FtpIp[50];
	}items;
}FwUpdateMsg_t;

extern FwUpdateMsg_t	FwUpdateMsg;



typedef struct
{
	uint32_t AppSize;		//APP�ļ���С
	uint32_t RdStartAddr;	//����ʼ��ַ
	uint32_t WrStartAddr;	//д��ʼ��ַ
}updateFromBakPara_t;


typedef enum
{
  CHECK_UPDATE_FLAG = 0x0,
  GET_UPDATE_FILE,
  UPDATE_APP,
  JUMP_TO_APP,
}updateStep_t;

extern updateStep_t updateStep;


extern uint8_t u8gIapBuf[MCU_PAGE_SIZE];
extern uint32_t NewAppSize;

void msg_init(void);
bool IapWrAppBin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize);
void UpdateApp(void);


#endif
