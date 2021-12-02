//LCD串行驱动

#ifndef _12864_h
#define _12864_h

//12864定义
#define CS_H    GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define CS_L    GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define CLK_H 	GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define CLK_L   GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define SID_H   GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SID_L   GPIO_ResetBits(GPIOB, GPIO_Pin_7) 


void LCD_Init (void);									//串行液晶初始化
void wr_lcd (u8 dat_comm,u8 content);		
void ldelay (u32 us);
void clrram (void);
void PutChar(char *str);	
void GPIO_Configuration(void);
void RCC_Configuration(void);


void  ChipHalInit(void);//硬件初始化
//用户可调用的应用函数

void PutString(char *str,u8 line);					   //在第line行输出字符串	
void PutIntNum(int value,u8 num,u8 choose);		 //输出整型变量，num为整数的位数(可保守取大点的值，但不能超过16位)，
																               //choose=10输出十进制形式，choose=16输出十六进制形式，数字左对齐
void PutFloatNum(float value,u8 num1,u8 num2); //输出浮点型变量，num表示要保留的小数点位数
		
#endif
