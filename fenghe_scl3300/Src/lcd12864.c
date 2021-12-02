#include "lcd12864.h"
#include "delay.h"


 
/*! 
*  @brief      LCD���з���һ���ֽ�
 *  @since      v1.0
 *  @param  byte   д���ֽ�
 *  @author     ZС��
 */
void SendByte(uint8_t byte)
{
     uint8_t i; 
	  for(i = 0;i < 8;i++)
    {
        if((byte << i) & 0x80)  //0x80(1000 0000)  ֻ�ᱣ�����λ
		{
		    LCD12864_SID_HIGH;           // ��������ߵ�ƽ��������1
		}
		else
		{
			LCD12864_SID_LOW;         // ��������͵�ƽ��������0
		}
		/*��		
		SID =	(Dbyte << i) & 0x80;
				
		��������Ϊ�˷������
		*/
		LCD12864_SCK_HIGH;   //ʱ�����õ�  ����SID�仯
		delay_us(5); //��ʱʹ����д��
		LCD12864_SCK_LOW;    //����ʱ�ӣ��ôӻ���SID
		delay_us(1);
	}   
}
 
/*! 
 *  @brief      LCDдָ��
 *  @since      v1.0
 *  @param  Cmd   Ҫд���ָ��
 *  @author     ZС��
 */
void Lcd_WriteCmd(uint8_t Cmd )
{
	 LCD12864_CS_HIGH;
     delay_ms(1);    //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //����λ
     SendByte(Cmd<<4);   //����λ(��ִ��<<)
	 LCD12864_CS_LOW;
}
 
/*! 
 *  @brief      LCDд����
 *  @since      v1.0
 *  @param  Dat   Ҫд�������
 *  @author     ZС��
 */
void Lcd_WriteData(uint8_t Dat )
{
	 LCD12864_CS_HIGH;
     delay_ms(1);     //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //����λ
     SendByte(Dat<<4);   //����λ(��ִ��<<)
	 LCD12864_CS_LOW;
}

void lcd12864_gpio_init()
{
       
  GPIO_InitTypeDef  GPIO_InitStruct = {0};
 
  GPIO_InitStruct.Pin	= LCD12864_RES_PIN | LCD12864_A0_PIN | LCD12864_SCK_PIN | LCD12864_SID_PIN;       
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    
  GPIO_InitStruct.Pull = GPIO_NOPULL;    
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
  HAL_GPIO_Init(LCD12864_SCK_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin	= LCD12864_CS_PIN;       
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    
  GPIO_InitStruct.Pull = GPIO_NOPULL;    
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
  HAL_GPIO_Init(LCD12864_CS_PORT, &GPIO_InitStruct);
  
  LCD12864_A0_LOW;
  LCD12864_RES_HIGH;
  LCD12864_CS_LOW;
  LCD12864_SID_HIGH;
  LCD12864_SCK_HIGH;

}

void Lcd_Init(void)
{ 
	
    delay_ms(50);   	//�ȴ�Һ���Լ죨��ʱ>40ms��
	Lcd_WriteCmd(0x30);        //�����趨:ѡ�����ָ�  ��ѡ��8bit������
    delay_ms(1);//��ʱ>137us 
    Lcd_WriteCmd(0x0c);        //����ʾ
    delay_ms(1);	//��ʱ>100us
    Lcd_WriteCmd(0x01);        //�����ʾ�������趨��ַָ��Ϊ00H
    delay_ms(30);	//��ʱ>10ms
	Lcd_WriteCmd(0x06);        //ÿ�ε�ַ�Զ�+1����ʼ�����
	
}


/* �ַ���ʾRAM��ַ    4��8�� */
uint8_t LCD_addr[4][8]={
	{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  		//��һ��
	{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//�ڶ���
	{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//������
	{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//������
	};
 
/*! 
 *  @brief      ��ʾ�ַ�����
 *  @since      v1.0
 *  @param  x: row(0~3)
 *  @param  y: line(0~7) 
 *  @param 	str: Ҫ��ʾ���ַ�����
 *  @author     ZС��
 */
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t *str)
{ 
	
	Lcd_WriteCmd(LCD_addr[x][y]); //д��ʼ���λ��
	while(*str>0)
    { 
      Lcd_WriteData(*str);    //д����
      str++;     
    }
	
}

void LCD_Clear(void)
	{
		
		Lcd_WriteCmd(0x01);			//����ָ��
		delay_ms(2);				//��ʱ�Դ�Һ���ȶ�������1.6ms��
		
	}

