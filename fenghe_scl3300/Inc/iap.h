#ifndef __IAP_H
#define __IAP_H

#include "main.h"
#include "stdbool.h"



//STM32F105RC Flash(256k) 分区
//bootloader:	  	0x08000000		0x6000	//24k
//app:  		    0x08006000		0x1B800	//110k
//app_bak:  	  	0x08021800		0x1B800	//110k
//data:			    0x0803D000		0x3000	//12k

#define APP_START_ADDR          (0x08006000)
#define APP_BAK_START_ADDR      (0x08021800)
#define DATA_START_ADDR      	  (0x0803D000)

#define MCU_PAGE_SIZE                           (0x800)    /* 2 Kbytes */

//data 区: 用作IAP数据记录（是否更新标记、是否更新成功、版本信息、文件大小、文件CRC32校验信息、IAP错误信息、FTP服务器地址、FTP账号、FTP密码等）


// 半字编程最大为2Kbytes 1024 * 2
#define	HALF_WORD_PROG_SIZE		            (MCU_PAGE_SIZE / 2)//(0x400)  //1024

//FIXME: 以下命令协议与GB32960 远程升级命令格式不一致
//自定义命令格式(需要与bootloader中同步更新)：
//是否需要更新标记
//更新成功标记
//文件名称[16btyes]
//文件大小[4bytes]
//文件校验[4bytes]
//固件版本[4bytes]
//硬件版本[4bytes]
//FTP账号[8bytes]
//FTP密码[8bytes]
//FTP服务器端口号[2bytes]
//FTP服务器地址[预留50bytes]
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
	uint32_t AppSize;		//APP文件大小
	uint32_t RdStartAddr;	//读起始地址
	uint32_t WrStartAddr;	//写起始地址
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
