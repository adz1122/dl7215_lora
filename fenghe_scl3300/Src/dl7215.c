#include "dl7215.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "scl3300.h"

static bool fserver_mac_cmd;
static char dl7215_cmd_buf[256];
	
static bool send_cmd_dl7215(char *cmd, char *exp_ans){
	char *p;
	Uart4_sendstr(cmd);
	for(uint16_t i=0; i<50000; ++i){
		delay_ms(1);
		if(Uart4Para.fDataReceived){
			  Uart4Para.fDataReceived = false;
//			  Uart4_sendstr((char *)Uart4Para.CurRxBuf);
			  DMA_Uart1Transmit(Uart4Para.CurRxBuf, Uart4Para.CurRxCnt);
			  if(strstr((char *)Uart4Para.CurRxBuf, "ERROR"))
				  return false;
			  if(strstr((char *)Uart4Para.CurRxBuf, "SRV_MAC_DEV_STATUS_REQ"))
				  fserver_mac_cmd = true;//distinguish mac cmd with ack from server
			  p = strstr((char *)Uart4Para.CurRxBuf, exp_ans);
			  if(fserver_mac_cmd){
				  fserver_mac_cmd = false;
				  p = NULL;				  
			  }
			  delay_ms(20);//delay to let uart1 transmit completed then clear the buf
			  Uart4Para.CurRxCnt = 0;
			  memset(Uart4Para.CurRxBuf, 0, UART4_RX_BUF_MAX_LEN);
			  if(p){
				  printf("cmd sent successfully\n");
				  return true;
			  }
		  }
	}
	return false;
}

bool dl7215_init(){
	fserver_mac_cmd = false;
	memset(dl7215_cmd_buf, 0, sizeof(dl7215_cmd_buf));
	return send_cmd_dl7215("AT+CJOIN=1,0,10,8\r\n", "+CJOIN:OK");
}

bool data_rtx_lora(){
//	send_cmd_dl7215("AT\r\n", "");
	sprintf(dl7215_cmd_buf, "AT+DTRX=1,8,%d,%s\r\n", 28, data_buf);
	return send_cmd_dl7215(dl7215_cmd_buf, "OK+SENT:");
	
} 
