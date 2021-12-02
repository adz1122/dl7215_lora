/************************************************************
**实验名称:12864显示
**功能:实现12864的串行显示
**注意事项:具体管脚连接在12864.h
*************************************************************/

#include "stm32f10x.h"
#include "12864.h"


extern  void PutString(char *str,u8 line);					//在第line行输出字符串

int main(void)
{
	
	ChipHalInit();			//片内硬件初始化
	LCD_Init ();		    //LCD初始化
	
	for(;;)
	{ 
		PutString("花间一壶酒",1);
		PutString("独酌无相亲",2);
		PutString("举杯邀明月",3);
		PutString("对影成三人",4);
	}
	
	
}



