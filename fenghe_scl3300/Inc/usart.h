/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>

/* USER CODE BEGIN Private defines */
#define	UART1_RX_BUF_MAX_LEN		  256
#define	UART2_RX_BUF_MAX_LEN			2048    //FTP接收数据时单次最大为1024bytes (一包数据中还包含其它数据)
#define	UART2_TX_BUF_MAX_LEN			256//512
#define	UART4_RX_BUF_MAX_LEN		  256


typedef struct
{
	uint8_t *CurRxBuf;
	uint8_t *TotalRxBuf;
	volatile uint16_t CurRxCnt;
	uint16_t TotalRxCnt;
	uint16_t RxCpltJudgeDly;
	uint32_t RxCpltJudgeDlySetting;
	volatile bool fDataReceiving;
	volatile bool fDataReceived;
	volatile bool fTxDing;
}UartPara_t;

extern bool fUART_OutPut;

extern UartPara_t Uart2Para;
extern uint8_t Uart2TotalRxBuf[UART2_RX_BUF_MAX_LEN];

extern UartPara_t Uart1Para;

extern UartPara_t Uart4Para;
extern uint8_t Uart4CurRxBuf[UART4_RX_BUF_MAX_LEN];
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void uartStop(UART_HandleTypeDef* huart);
void UartUserInit(void);
void UART_IDLE_Callback(UART_HandleTypeDef *huart);

bool DMA_Uart1Transmit(uint8_t *Buf, uint16_t BufLen);
bool DMA_Uart2Transmit(uint8_t *Buf, uint16_t BufLen);
void Uart4_sendchar(uint8_t c);
void Uart4_sendstr(char *Str);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
