/*****************************************************************************************/
/*																						 */
/*                               LCD串行驱动程序  2008年12月20日晚 by cihu               */
/*																						 */
/*	实测写8条液晶要接近40ms，故不能频繁写液晶，写时在操作系统里最好要大于200ms刷新频率											 */							
/*																						 */
/*****************************************************************************************/
#include "stm32f10x.h"
#include "12864.h" 
						 
#define DLY  25
#define COMM  0
#define CDAT  1

u8 lcd_temp;
u8 h_x[4]={0x80,0x90,0x88,0x98};							//行地址						

u8   flag_LCD;								                //为0时关闭液晶显示，1时打开


RCC_ClocksTypeDef RCC_ClockFreq;

void RCC_Configuration(void)
{
	SystemInit();//源自system_stm32f10x.c文件,只需要调用此函数,则可完成RCC的配置.具体请看2_RCC

	/**************************************************
	获取RCC的信息,调试用
	请参考RCC_ClocksTypeDef结构体的内容,当时钟配置完成后,
	里面变量的值就直接反映了器件各个部分的运行频率
	***************************************************/
	RCC_GetClocksFreq(&RCC_ClockFreq);
	
	/* 这个配置可使外部晶振停振的时候,产生一个NMI中断,不需要用的可屏蔽掉*/
	//RCC_ClockSecuritySystemCmd(ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* 设置PB5,7,9;
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/*允许总线CLOCK,在使用GPIO之前必须允许相应端的时钟.
	从STM32的设计角度上说,没被允许的端将不接入时钟,也就不会耗能,
	这是STM32节能的一种技巧,*/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	/* PB5,7,9输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M时钟速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	

}
/*----------------延时函数-------------------*/

void ldelay (u32 us)   //delay time
{
  u8 i,temp;  
  for(i=0;i<10;i++)
  for(temp=us;temp>0;temp--);
}


/*-----------write string------------*/

/*------------------初始化-----------------*/
void LCD_Init (void)										//使用前须调用此函数初始化	　
{
  flag_LCD=1;

//  IO2DIR=IO2DIR|CS_IO|STD_IO|SCLK_IO;								//PINSEL2已经在startup.s中配置完，现只需配置方向
  
  wr_lcd (COMM,0x30);  /*30---基本指令动作*/   
  wr_lcd (COMM,0x01);  /*清屏，地址指针指向00H*/
  ldelay (100);
  wr_lcd (COMM,0x06);  /*光标的移动方向*/
  wr_lcd (COMM,0x0c);  /*开显示，关游标*/
}
/*******************************
**函数名:ChipHalInit()
**功能:片内硬件初始化
*******************************/
void  ChipHalInit(void)
{

	RCC_Configuration();	//初始化时钟源
	GPIO_Configuration();	//初始化GPIO
}


/*--------------清DDRAM------------------*/
void clrram (void)
{
  wr_lcd (COMM,0x30);
  wr_lcd (COMM,0x01);
   ldelay (180);
}
/*---------------------------------------*/

void wr_lcd(u8 dat_COMM,u8 content)
{
  u8 a,i,j;
  ldelay (30);
  lcd_temp=(1<<7);
  a=content;
  CS_H;
  CLK_L;
  SID_H;
  for(i=0;i<5;i++)
  {
    CLK_H;
    ldelay(DLY);
    CLK_L;
  }
  SID_L;
  CLK_H;
  ldelay(DLY);
  CLK_L;
  if(dat_COMM)
    SID_H;   //data
  else
    SID_L;   //COMMand
   
  CLK_H;
  ldelay(DLY);
  CLK_L;
  SID_L;
  CLK_H;
  ldelay(DLY);
  CLK_L;
  for(j=0;j<2;j++)
  {
    for(i=0;i<4;i++)
    {
      if(lcd_temp&a)  SID_H;
      else SID_L;
      lcd_temp=lcd_temp>>1;
      CLK_H;
      ldelay(DLY);
      CLK_L;
    }
    SID_L;
    for(i=0;i<4;i++)
    {
      CLK_H;
      ldelay(DLY);
      CLK_L;
    }
  }
}


void PutString(char *str,u8 line)			//在第line行输出一个字符串
{
	u8 i=0;
	
	if(!flag_LCD)
	{
		return;
	}	

 	wr_lcd (COMM,0x30);
	wr_lcd (COMM,h_x[line-1]);

	while(str[i] != '\0' )
	{
	   	if(str[i]=='\n')	
			{
				wr_lcd (COMM,h_x[line]);	
				i++;
				continue;
			}    	
		if(str[i]=='\t')	
			{
				wr_lcd (COMM,h_x[line-1]+8);
				i++;	
				continue;
			}

		wr_lcd (CDAT,str[i++]);
  	}
}

void PutChar(char *str)					     //在默认位置输出一个字符串		
{
	u8 i=0;
	if(!flag_LCD)
	{
		return;
	}
	while(str[i] != '\0' )
	{
		wr_lcd (CDAT,str[i++]);
  	}
}

/*-----------write number------------*/

void PutIntNum(int value,u8 num,u8 choose)
{
	char string[20],*str,i=0;
	
	if(!flag_LCD)
	{
		return;
	}

	str=string;

	if(value<0)
	{
		wr_lcd(CDAT,'-');
		value=0-value;	
	} 

	if (choose==10)                    		//显示10进制
	{
   
		while(str[i]!= '\0' )
		{
		  wr_lcd (CDAT,str[i++]);
  		}
    }
   
	else if (choose==16)					//显示16进制
	{

		while(str[i]!='\0' )
		{
		  wr_lcd (CDAT,str[i++]);
  		}
	}
}

void PutFloatNum(float value,u8 num1,u8 num2)
{
	char string[20],*str,i=0;

	str=string;
	
	if(!flag_LCD)
	{
		return;
	}

	if(value<0)
	{
		wr_lcd(CDAT,'-');
		value=0-value;	
	}
	
	while(str[i]!='\0' )
	{
		wr_lcd (CDAT,str[i++]);
  	}
}
