#include "lcd12864.h"
#include "delay.h"

#define WRITE_CMD	0xF8//写命令  
#define WRITE_DAT	0xFA//写数据
 
/*! 
*  @brief      LCD串行发送一个字节
 *  @since      v1.0
 *  @param  byte   写入字节
 *  @author     Z小旋
 */
void SendByte(uint8_t byte)
{
     uint8_t i; 
	  for(i = 0;i < 8;i++)
    {
        if((byte << i) & 0x80)  //0x80(1000 0000)  只会保留最高位
		{
		    SID = 1;           // 引脚输出高电平，代表发送1
		}
		else
		{
			SID = 0;         // 引脚输出低电平，代表发送0
		}
		/*或		
		SID =	(Dbyte << i) & 0x80;
				
		上面那样为了方便理解
		*/
		SCLK = 0;   //时钟线置低  允许SID变化
		delay_us(5); //延时使数据写入
		SCLK = 1;    //拉高时钟，让从机读SID
	}   
}
 
/*! 
 *  @brief      LCD写指令
 *  @since      v1.0
 *  @param  Cmd   要写入的指令
 *  @author     Z小旋
 */
void Lcd_WriteCmd(uint8_t Cmd )
{
     delay_ms(1);    //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //高四位
     SendByte(Cmd<<4);   //低四位(先执行<<)
}
 
/*! 
 *  @brief      LCD写数据
 *  @since      v1.0
 *  @param  Dat   要写入的数据
 *  @author     Z小旋
 */
void Lcd_WriteData(uint8_t Dat )
{
     delay_ms(1);     //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //高四位
     SendByte(Dat<<4);   //低四位(先执行<<)

}