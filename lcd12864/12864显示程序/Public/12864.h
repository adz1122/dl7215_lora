//LCD��������

#ifndef _12864_h
#define _12864_h

//12864����
#define CS_H    GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define CS_L    GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define CLK_H 	GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define CLK_L   GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define SID_H   GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SID_L   GPIO_ResetBits(GPIOB, GPIO_Pin_7) 


void LCD_Init (void);									//����Һ����ʼ��
void wr_lcd (u8 dat_comm,u8 content);		
void ldelay (u32 us);
void clrram (void);
void PutChar(char *str);	
void GPIO_Configuration(void);
void RCC_Configuration(void);


void  ChipHalInit(void);//Ӳ����ʼ��
//�û��ɵ��õ�Ӧ�ú���

void PutString(char *str,u8 line);					   //�ڵ�line������ַ���	
void PutIntNum(int value,u8 num,u8 choose);		 //������ͱ�����numΪ������λ��(�ɱ���ȡ����ֵ�������ܳ���16λ)��
																               //choose=10���ʮ������ʽ��choose=16���ʮ��������ʽ�����������
void PutFloatNum(float value,u8 num1,u8 num2); //��������ͱ�����num��ʾҪ������С����λ��
		
#endif
