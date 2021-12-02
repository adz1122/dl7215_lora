#include "lcd12864.h"
#include "delay.h"
/*********************************����12864��������*******************************************/
//����PB12-----RS
//����PB13-----R/W
//SCLK����PB15-----E

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
*������	 ��	void send_command(uint8_t command)	
*����	 �� ���ʹ���ָ��
*���ú�����	void send_byte(uint8_t byte)
*���������	command
*���������	��
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
*������	 ��	void send_data(uint8_t disdata)	
*����	 �� ���ʹ�������
*���ú�����	void send_byte(uint8_t byte)
*���������	disdata
*���������	��
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
*������	 ��	void lcd12864_init()		
*����	 �� lcd12864Һ����ʼ��
*���ú�����	void send_command(uint8_t command)
*���������	��
*���������	��
********************************************************/
void lcd12864_init(void)
{
	delay_ms(1);
	send_command(0x30);	  //�����趨������Ϊ����ָ�
	delay_ms(1);
	send_command(0x0c);	  //��ʾ�������أ����׹�
	delay_ms(1);
	send_command(0x01);	  //����
	delay_ms(1);
	send_command(0x06);	  //��������ã��������
}


/********************************************************
*������	 ��	void lcd12864_display(uint8_t y,uint8_t x,uint8_t *P)		
*����	 �� lcdҺ����ʾ����
*���ú�����	send_command(uint8_t command)��void send_data(uint8_t disdata)��
*���������	x,y,*P;
*���������	��
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
