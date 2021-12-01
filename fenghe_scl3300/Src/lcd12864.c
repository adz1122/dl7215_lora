#include "lcd12864.h"
#include "delay.h"

#define WRITE_CMD	0xF8//д����  
#define WRITE_DAT	0xFA//д����
 
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
		    SID = 1;           // ��������ߵ�ƽ��������1
		}
		else
		{
			SID = 0;         // ��������͵�ƽ��������0
		}
		/*��		
		SID =	(Dbyte << i) & 0x80;
				
		��������Ϊ�˷������
		*/
		SCLK = 0;   //ʱ�����õ�  ����SID�仯
		delay_us(5); //��ʱʹ����д��
		SCLK = 1;    //����ʱ�ӣ��ôӻ���SID
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
     delay_ms(1);    //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //����λ
     SendByte(Cmd<<4);   //����λ(��ִ��<<)
}
 
/*! 
 *  @brief      LCDд����
 *  @since      v1.0
 *  @param  Dat   Ҫд�������
 *  @author     ZС��
 */
void Lcd_WriteData(uint8_t Dat )
{
     delay_ms(1);     //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //����λ
     SendByte(Dat<<4);   //����λ(��ִ��<<)

}