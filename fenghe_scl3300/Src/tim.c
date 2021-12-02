/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "usart.h"

bool fTime1ms;

uint16_t u16TimeUart1RxCpltDly;
uint16_t u16TimeUart2RxCpltDly;
__IO uint16_t u16gTimModemRdyDly;
__IO uint16_t u16gRemainWaitDly;

uint16_t u16gTime1s;
uint32_t u32gTimeSystemRun;


/* USER CODE END 0 */

TIM_HandleTypeDef htim3;

/* TIM3 init function */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 720-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
//  HAL_TIM_Base_Start_IT(&htim3); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE 
//  TIM3->DIER|=1<<0;						//允许更新中断  
 // TIM3->CR1|=0x01;						//使能定时器3
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
  /* USER CODE END TIM3_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
	  
	  /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void StartTimer(TIM_HandleTypeDef* htim){
	HAL_TIM_Base_Start_IT(htim);
}

void StopTimer(TIM_HandleTypeDef* htim)
{
  HAL_TIM_Base_Stop_IT(htim);
  HAL_TIM_Base_MspDeInit(htim);
}


void Time(void)
{
	if (fTime1ms)
	{
		fTime1ms = false;
		
//		if (u16TimeUart1RxCpltDly)
//		{
//		  u16TimeUart1RxCpltDly--;
//		}
//		if (u16TimeUart2RxCpltDly)
//		{
//		  u16TimeUart2RxCpltDly--;
//		}
		if (Uart1Para.fDataReceiving)
		{
			u16TimeUart1RxCpltDly--;
			if (u16TimeUart1RxCpltDly == 0)//to judge whether usart1 receive completed
			{		
				Uart1Para.fDataReceiving = false;
				if (Uart1Para.CurRxCnt > 0)
				{
					Uart1Para.fDataReceived = true;
				}
			}
		}
		if (Uart2Para.fDataReceiving)
		{
			u16TimeUart2RxCpltDly--;
			if (u16TimeUart2RxCpltDly == 0)//to judge whether usart2 receive completed
			{		
				Uart2Para.fDataReceiving = false;
				if (Uart2Para.TotalRxCnt > 0)
				{
					Uart2Para.fDataReceived = true;
				}
			}
		}
		if (Uart4Para.fDataReceiving)
		{
			Uart4Para.RxCpltJudgeDly--;
			if (Uart4Para.RxCpltJudgeDly == 0)//to judge whether usart1 receive completed
			{		
				Uart4Para.fDataReceiving = false;
				if (Uart4Para.CurRxCnt > 0)
				{
//					Uart4CurRxBuf[Uart4Para.CurRxCnt++] = 0;
					Uart4Para.fDataReceived = true;
				}
			}
		}
		if (u16gTimModemRdyDly)
		{
		  u16gTimModemRdyDly--;
		}
		if (u16gRemainWaitDly)
		{
		  u16gRemainWaitDly--;
		}
		
		if (++u16gTime1s >= 1000)
		{
			u16gTime1s = 0;
			if (u32gTimeSystemRun < 0xFFFFFFFF)
			{
				u32gTimeSystemRun++;
			}
			fUART_OutPut = true;
		}

    
	}  
}

//tim3 1ms callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  if (htim->Instance == TIM3)
  {
    fTime1ms = true;
  }
  Time();

}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
