#include "lcd12864.h"
#include "delay.h"


 
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
		    LCD12864_SID_HIGH;           // 引脚输出高电平，代表发送1
		}
		else
		{
			LCD12864_SID_LOW;         // 引脚输出低电平，代表发送0
		}
		/*或		
		SID =	(Dbyte << i) & 0x80;
				
		上面那样为了方便理解
		*/
		LCD12864_SCK_HIGH;   //时钟线置低  允许SID变化
		delay_us(5); //延时使数据写入
		LCD12864_SCK_LOW;    //拉高时钟，让从机读SID
		delay_us(1);
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
	 LCD12864_CS_HIGH;
     delay_ms(1);    //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //高四位
     SendByte(Cmd<<4);   //低四位(先执行<<)
	 LCD12864_CS_LOW;
}
 
/*! 
 *  @brief      LCD写数据
 *  @since      v1.0
 *  @param  Dat   要写入的数据
 *  @author     Z小旋
 */
void Lcd_WriteData(uint8_t Dat )
{
	 LCD12864_CS_HIGH;
     delay_ms(1);     //由于我们没有写LCD正忙的检测，所以直接延时1ms，使每次写入数据或指令间隔大于1ms 便可不用写忙状态检测
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //高四位
     SendByte(Dat<<4);   //低四位(先执行<<)
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
	
    delay_ms(50);   	//等待液晶自检（延时>40ms）
	Lcd_WriteCmd(0x30);        //功能设定:选择基本指令集  ，选择8bit数据流
    delay_ms(1);//延时>137us 
    Lcd_WriteCmd(0x0c);        //开显示
    delay_ms(1);	//延时>100us
    Lcd_WriteCmd(0x01);        //清除显示，并且设定地址指针为00H
    delay_ms(30);	//延时>10ms
	Lcd_WriteCmd(0x06);        //每次地址自动+1，初始化完成
	
}


/* 字符显示RAM地址    4行8列 */
uint8_t LCD_addr[4][8]={
	{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  		//第一行
	{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//第二行
	{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//第三行
	{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//第四行
	};
 
/*! 
 *  @brief      显示字符或汉字
 *  @since      v1.0
 *  @param  x: row(0~3)
 *  @param  y: line(0~7) 
 *  @param 	str: 要显示的字符或汉字
 *  @author     Z小旋
 */
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t *str)
{ 
	
	Lcd_WriteCmd(LCD_addr[x][y]); //写初始光标位置
	while(*str>0)
    { 
      Lcd_WriteData(*str);    //写数据
      str++;     
    }
	
}

void LCD_Clear(void)
	{
		
		Lcd_WriteCmd(0x01);			//清屏指令
		delay_ms(2);				//延时以待液晶稳定【至少1.6ms】
		
	}

