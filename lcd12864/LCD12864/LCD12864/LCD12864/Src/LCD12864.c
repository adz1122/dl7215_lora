#include "lcd12864.h"
#include "delay.h"
/*********************************设置12864串口引脚*******************************************/
//连至PB12-----RS
//连至PB13-----R/W
//SCLK连至PB15-----E

void send_byte(uint8_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if((byte<<i)&0x80)
		{
			RW_HIGH;
		}
		else
		{
			RW_LOW;
		}
		CLK_HIGH;
		delay_ms(1);
		CLK_LOW;
	}
}


/********************************************************
*函数名	 ：	void send_command(uint8_t command)	
*功能	 ： 发送串口指令
*调用函数：	void send_byte(uint8_t byte)
*输入参数：	command
*输出参数：	无
********************************************************/
void send_command(uint8_t command)
{
	RS_HIGH;
	send_byte(0xf8);
	send_byte(command&0xf0);
	send_byte(command<<4);
	delay_ms(1);
	RS_LOW;
}
/********************************************************
*函数名	 ：	void send_data(uint8_t disdata)	
*功能	 ： 发送串口数据
*调用函数：	void send_byte(uint8_t byte)
*输入参数：	disdata
*输出参数：	无
********************************************************/
void send_data(uint8_t disdata)
{
	RS_HIGH;
	send_byte(0xfa);
	send_byte(disdata&0xf0);
	send_byte((disdata<<4)&0xf0);
	delay_ms(1);
	RS_LOW;
}


/********************************************************
*函数名	 ：	void lcd12864_init()		
*功能	 ： lcd12864液晶初始化
*调用函数：	void send_command(uint8_t command)
*输入参数：	无
*输出参数：	无
********************************************************/
void lcd12864_init(void)
{
	delay_ms(1);
	send_command(0x30);	  //功能设定，设置为基本指令集
	delay_ms(1);
	send_command(0x0c);	  //显示开，光标关，反白关
	delay_ms(1);
	send_command(0x01);	  //清屏
	delay_ms(1);
	send_command(0x06);	  //进入点设置，光标左移
}


/********************************************************
*函数名	 ：	void lcd12864_display(uint8_t y,uint8_t x,uint8_t *P)		
*功能	 ： lcd液晶显示函数
*调用函数：	send_command(uint8_t command)；void send_data(uint8_t disdata)；
*输入参数：	x,y,*P;
*输出参数：	无
********************************************************/
void lcd12864_display(uint8_t y,uint8_t x,uint8_t *P)
{
	switch(y)
		{
			case 1:send_command(0x7f+x);break;
			case 2:send_command(0x8f+x);break;
			case 3:send_command(0x87+x);break;
			case 4:send_command(0x97+x);break;
		}
	while(*P)
	{
		send_data(*P++);
	}
}
