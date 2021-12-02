/*****************************************************************************************/
/*																						 */
/*                               LCD������������  2008��12��20���� by cihu               */
/*																						 */
/*	ʵ��д8��Һ��Ҫ�ӽ�40ms���ʲ���Ƶ��дҺ����дʱ�ڲ���ϵͳ�����Ҫ����200msˢ��Ƶ��											 */							
/*																						 */
/*****************************************************************************************/
#include "stm32f10x.h"
#include "12864.h" 
						 
#define DLY  25
#define COMM  0
#define CDAT  1

u8 lcd_temp;
u8 h_x[4]={0x80,0x90,0x88,0x98};							//�е�ַ						

u8   flag_LCD;								                //Ϊ0ʱ�ر�Һ����ʾ��1ʱ��


RCC_ClocksTypeDef RCC_ClockFreq;

void RCC_Configuration(void)
{
	SystemInit();//Դ��system_stm32f10x.c�ļ�,ֻ��Ҫ���ô˺���,������RCC������.�����뿴2_RCC

	/**************************************************
	��ȡRCC����Ϣ,������
	��ο�RCC_ClocksTypeDef�ṹ�������,��ʱ��������ɺ�,
	���������ֵ��ֱ�ӷ�ӳ�������������ֵ�����Ƶ��
	***************************************************/
	RCC_GetClocksFreq(&RCC_ClockFreq);
	
	/* ������ÿ�ʹ�ⲿ����ͣ���ʱ��,����һ��NMI�ж�,����Ҫ�õĿ����ε�*/
	//RCC_ClockSecuritySystemCmd(ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* ����PB5,7,9;
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/*��������CLOCK,��ʹ��GPIO֮ǰ����������Ӧ�˵�ʱ��.
	��STM32����ƽǶ���˵,û������Ķ˽�������ʱ��,Ҳ�Ͳ������,
	����STM32���ܵ�һ�ּ���,*/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	/* PB5,7,9��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50Mʱ���ٶ�
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	

}
/*----------------��ʱ����-------------------*/

void ldelay (u32 us)   //delay time
{
  u8 i,temp;  
  for(i=0;i<10;i++)
  for(temp=us;temp>0;temp--);
}


/*-----------write string------------*/

/*------------------��ʼ��-----------------*/
void LCD_Init (void)										//ʹ��ǰ����ô˺�����ʼ��	��
{
  flag_LCD=1;

//  IO2DIR=IO2DIR|CS_IO|STD_IO|SCLK_IO;								//PINSEL2�Ѿ���startup.s�������꣬��ֻ�����÷���
  
  wr_lcd (COMM,0x30);  /*30---����ָ���*/   
  wr_lcd (COMM,0x01);  /*��������ַָ��ָ��00H*/
  ldelay (100);
  wr_lcd (COMM,0x06);  /*�����ƶ�����*/
  wr_lcd (COMM,0x0c);  /*����ʾ�����α�*/
}
/*******************************
**������:ChipHalInit()
**����:Ƭ��Ӳ����ʼ��
*******************************/
void  ChipHalInit(void)
{

	RCC_Configuration();	//��ʼ��ʱ��Դ
	GPIO_Configuration();	//��ʼ��GPIO
}


/*--------------��DDRAM------------------*/
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


void PutString(char *str,u8 line)			//�ڵ�line�����һ���ַ���
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

void PutChar(char *str)					     //��Ĭ��λ�����һ���ַ���		
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

	if (choose==10)                    		//��ʾ10����
	{
   
		while(str[i]!= '\0' )
		{
		  wr_lcd (CDAT,str[i++]);
  		}
    }
   
	else if (choose==16)					//��ʾ16����
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
