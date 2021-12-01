/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "tim.h"

/* USER CODE BEGIN 0 */
bool fUART_OutPut;

#define	Uart1DMA_RecvEn()  HAL_UART_Receive_DMA(&huart1, Uart1CurRxBuf, UART1_RX_BUF_MAX_LEN)

/* 开启uart2串口DMA接收中断使能 */
#define	Uart2DMA_RecvEn()  HAL_UART_Receive_DMA(&huart2, Uart2CurRxBuf, UART2_RX_BUF_MAX_LEN)

/* 与SIM7600CE串口通讯参数 */
bool fUart2RecvedData;//uart2接收到数据
static uint8_t Uart2CurRxBuf[UART2_RX_BUF_MAX_LEN];

uint8_t Uart2TotalRxBuf[UART2_RX_BUF_MAX_LEN];
UartPara_t Uart2Para;
bool fUart2RecvFrameCplt;//uart2接收完成一帧数据



/* 与PC串口通讯参数 */
bool fUart1RecvedData;
static uint8_t Uart1CurRxBuf[UART1_RX_BUF_MAX_LEN];

UartPara_t Uart1Para;
bool fUart1RecvFrameCplt;


/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* UART4 init function */
void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();
  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**UART4 GPIO Configuration    
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */
    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();
  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**UART5 GPIO Configuration    
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();
  
    /**UART4 GPIO Configuration    
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();
  
    /**UART5 GPIO Configuration    
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/*****************************************************************************
*函数名:uartStop
*描述:停止串口外设并清除标志位
*参数:
*返回值:
*****************************************************************************/
void uartStop(UART_HandleTypeDef* huart)
{
	__HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
	__HAL_UART_DISABLE(huart);  
    HAL_UART_DMAStop(huart);

    /* Clears the UART ORE pending flag */
	__HAL_UART_CLEAR_OREFLAG(huart);
	/* Clears the UART IDLE pending flag */
	__HAL_UART_CLEAR_IDLEFLAG(huart);		

	/*Clear the DMA Stream pending flags.*/
	__HAL_DMA_CLEAR_FLAG(huart->hdmarx,__HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmarx));

}


/*****************************************************************************
*函数名:UartUserInit
*描述:uart用户初始化
*参数:
*返回值:
*****************************************************************************/
void UartUserInit(void)
{
	fUART_OutPut = false;
	
	fUart1RecvedData = false;
    fUart1RecvFrameCplt = false;
	fUart2RecvedData = false;
    fUart2RecvFrameCplt = false;

	memset(Uart2CurRxBuf, 0, UART2_RX_BUF_MAX_LEN);
	memset(Uart2TotalRxBuf, 0, UART2_RX_BUF_MAX_LEN);
	memset(&Uart2Para, 0, sizeof(Uart2Para));
	Uart2Para.CurRxBuf = Uart2CurRxBuf;
	Uart2Para.TotalRxBuf = Uart2TotalRxBuf;
	Uart2Para.RxCpltJudgeDlySetting = 100;
	
	memset(Uart1CurRxBuf, 0, UART1_RX_BUF_MAX_LEN);
	memset(&Uart1Para, 0, sizeof(Uart1Para));
	Uart1Para.CurRxBuf = Uart1CurRxBuf;

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);          //使能 USART1 IDLE中断
	

	/* 开启串口DMA接收中断使能 */
	Uart1DMA_RecvEn();	//打开DMA接收，数据存入缓存
#ifndef NOUSART2
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);          //使能 USART2 IDLE中断
	Uart2DMA_RecvEn();
#endif

}


/*****************************************************************************
*函数名:Uart1RxCpltJudge
*描述: uart1 接收完成判断(应对AT命令回复长度不定，而且每个命令回复延时不同)，
*		如果仅用空闲中断判断会出现数据包接收不完整的情况(通过OS定时器延迟判断接收完成)
*参数:
*返回值:
*****************************************************************************/
void Uart1RxCpltJudge(void)
{
	if (fUart1RecvedData)
	{
		if (!u16TimeUart1RxCpltDly)
		{		
			fUart1RecvedData = false;
			if (Uart1Para.CurRxCnt > 0)
			{
				fUart1RecvFrameCplt = true;
			}
		}
	}
}


/*****************************************************************************
*函数名:Uart2RxCpltJudge
*描述: uart2 接收完成判断(应对AT命令回复长度不定，而且每个命令回复延时不同)，
*		如果仅用空闲中断判断会出现数据包接收不完整的情况(通过OS定时器延迟判断接收完成)
*参数:
*返回值:
*****************************************************************************/
void Uart2RxCpltJudge(void)
{
	if (fUart2RecvedData)
	{
		if (!u16TimeUart2RxCpltDly)
		{		
			fUart2RecvedData = false;
			if (Uart2Para.TotalRxCnt > 0)
			{
				fUart2RecvFrameCplt = true;
			}
		}
	}
}

/*****************************************************************************
*函数名:UART_IDLE_Callback
*描述: 串口空闲中断服务回调函数
*参数:
*返回值:
*****************************************************************************/
void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE);

	if ((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		/* set uart state  ready*/
		huart->gState = HAL_UART_STATE_READY;			
		/* Disable the rx  DMA peripheral */
		__HAL_DMA_DISABLE(huart->hdmarx);	//HAL_UART_DMAStop(&huart2);
		/*Clear the DMA Stream pending flags.*/
		__HAL_DMA_CLEAR_FLAG(huart->hdmarx,__HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmarx));
			
		/* Process Unlocked */
		__HAL_UNLOCK(huart->hdmarx);

		if (huart->Instance == USART2)
		{
			/* get rx data len */
			HAL_UART_AbortReceive(huart);
			// HAL_UART_DMAStop(huart);
			// Uart2Para.TotalRxCnt = UART2_RX_BUF_MAX_LEN - huart->hdmarx->Instance->CNDTR;
			Uart2Para.CurRxCnt = UART2_RX_BUF_MAX_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

			u16TimeUart2RxCpltDly = 200;
			fUart2RecvedData = true;
			memcpy(Uart2TotalRxBuf + Uart2Para.TotalRxCnt, Uart2CurRxBuf, Uart2Para.CurRxCnt);
			Uart2Para.TotalRxCnt += Uart2Para.CurRxCnt;
			memset(Uart2CurRxBuf, 0, UART2_RX_BUF_MAX_LEN);
			Uart2Para.CurRxCnt = 0;
			Uart2DMA_RecvEn();
		}
		else if (huart->Instance == USART1)
		{
			/* get rx data len */
			// Uart1Para.CurRxCnt = UART1_RX_BUF_MAX_LEN - huart->hdmarx->Instance->CNDTR;
			HAL_UART_AbortReceive(huart);
			Uart1Para.CurRxCnt = UART1_RX_BUF_MAX_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
			u16TimeUart1RxCpltDly = 200;
			fUart1RecvedData = true;
			Uart1DMA_RecvEn();
		}
	}
}


/*****************************************************************************
*函数名:DMA_Uart1Transmit
*描述: 串口1发送封装
*参数:
*返回值:
*****************************************************************************/
bool DMA_Uart1Transmit(uint8_t *Buf, uint16_t BufLen)
{
	// USER_ASSERT(Buf);
	// USER_ASSERT(BufLen);
	// USER_ASSERT(BufLen < UART1_RX_BUF_MAX_LEN);

	while (Uart1Para.fTxDing);
	if (HAL_UART_Transmit_DMA(&huart1, Buf, BufLen) == HAL_OK)
	{
		Uart1Para.fTxDing = true;
		return true;
	}
	else
	{
		return true;
	}
}

/*****************************************************************************
*函数名:DMA_Uart2Transmit
*描述: 串口2发送封装
*参数:
*返回值:
*****************************************************************************/
bool DMA_Uart2Transmit(uint8_t *Buf, uint16_t BufLen)//串口发送封装
{
	// USER_ASSERT(Buf);
	// USER_ASSERT(BufLen);
	// USER_ASSERT(BufLen < UART2_RX_BUF_MAX_LEN);

	while (Uart2Para.fTxDing);
	if (HAL_UART_Transmit_DMA(&huart2, Buf, BufLen) == HAL_OK)
	{
		Uart2Para.fTxDing = true;
		return true;
	}
	else
	{
		return false;
	}	
}


/*****************************************************************************
*函数名:HAL_UART_TxCpltCallback
*描述: 串口发送完成回调函数
*参数:
*返回值:
*****************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		Uart2Para.fTxDing = false;
		__HAL_DMA_CLEAR_FLAG(huart->hdmarx,__HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmatx));
	}
	else if (huart->Instance == USART1)
	{
		memset(Uart1CurRxBuf, 0, UART1_RX_BUF_MAX_LEN);
		Uart1Para.CurRxCnt = 0;
		Uart1Para.fTxDing = false;
		__HAL_DMA_CLEAR_FLAG(huart->hdmarx,__HAL_DMA_GET_TC_FLAG_INDEX(huart->hdmatx));
	}
}


/*****************************************************************************
*函数名:WrCharToUart
*描述: 串口打印发送函数
*参数:
*返回值:
*****************************************************************************/
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch,FILE *f)
{
  /* Write a character to the USART */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF); 
  return ch;
} 
#endif

/*****************************************************************************
*函数名:UartPorc
*描述: 串口处理
*参数:
*返回值:
*****************************************************************************/
void UartPorc(void)
{
	Uart1RxCpltJudge();
#ifndef NOUSART2
	Uart2RxCpltJudge();
#endif
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
