/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main program body
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
//#include<intrins.h>

/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef   SPI_InitStructure;
u8 SPI1_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
                                 0x1C, 0x1D, 0x1E, 0x1F, 0x20};
u8 SPI2_Buffer_Tx[BufferSize] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
                                 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62,
                                 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
                                 0x6C, 0x6D, 0x6E, 0x6F, 0x70};
u8 SPI1_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
u8 TxIdx = 0, RxIdx = 0, k = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile TestStatus TransferStatus3 = FAILED, TransferStatus4 = FAILED;
ErrorStatus HSEStartUpStatus;

/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);

// Standard SPI requests
#define REQ_READ_ACC_X          0x040000F7
#define REQ_READ_ACC_Y          0x080000FD
#define REQ_READ_ACC_Z          0x0C0000FB
#define REQ_READ_STO            0x100000E9
#define REQ_READ_TEMP           0x140000EF
#define REQ_READ_STATUS         0x180000E5
#define REQ_WRITE_SW_RESET      0xB4002098
#define REQ_WRITE_MODE1         0xB400001F
#define REQ_WRITE_MODE2         0xB4000102
#define REQ_WRITE_MODE3         0xB4000225
#define REQ_WRITE_MODE4         0xB4000338
#define REQ_READ_WHOAMI         0x40000091

// SPI frame field masks
#define OPCODE_FIELD_MASK       0xFC000000
#define RS_FIELD_MASK           0x03000000
#define DATA_FIELD_MASK         0x00FFFF00
#define CRC_FIELD_MASK          0x000000FF






void delayus(u16 xx)	 //1.06us
{
   u16 i=0;
   u8 l=13;
    for(i=0;i<xx;i++)
   {
         do{
		   l--;
		 }while(l);
		 
		 l=13;
   }
}




void  delayms(u16 xxx)	 //0.998ms
{
   u16  i=0,l=12000;
   for(i=0;i<xxx;i++)
   {
        do{
		  l--;
		}while(l);
		l=12000;
   }
}


void usart_init(void) 	   //72MHZ 9600
{ 
/*   USART1->BRR=0x1d4c;
   USART1->CR1=0x002c;
   USART1->CR1|=1<<13; 	 */

   ////////////////
  
   USART1->BRR=0x1d4c;//72MHZ 9600
   USART1->CR3=0x0000;
   USART1->CR2=0x0000;
   USART1->CR1=0x002c;
   USART1->CR1|=1<<13;	 
}



 
void USART_Txbyte(u8 a)
{
    GPIOA->ODR |= 0x0100;  //29脚PA8开
    USART1->DR = a;
    while(!(USART1->SR&(1<<6))) ;
    GPIOA->ODR &= ~(0x0100);  //29脚PA8关	
}

#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 

#define SET_SPI2 GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define CLR_SPI2 GPIO_ResetBits(GPIOB,GPIO_Pin_12);



//#define SET_SPI2 PBout(12)=1
///#define CLR_SPI2 PBout(12)=0

USART_InitTypeDef USART_InitStructure;


//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
{	

     	
	u8 retry=0;	
	;			 
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI2->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}						    
	return SPI2->DR;          //返回收到的数据				    
}


u32  SPI_Send(u32  Request)
{

    u8 retry=0;	
	u32 Response=0;
	CLR_SPI2;
	
				 
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
//		retry++;
//		if(retry>200)return 0;
	}			  
	SPI2->DR=(u16)Request>>16;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
//		retry++;
//		if(retry>200)return 0;
	}
	Response=SPI2->DR;                                        // Read RX buffer (Response high word)	
    Response<<=16;
	///////////////////////////
/*	retry=0;
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}*/
	SPI2->DR=(u16)(Request&0x0000FFFF);	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
//		retry++;
//		if(retry>200)return 0;
	}                                        // Read RX buffer (Response high word)	
    Response|=SPI2->DR;
	SET_SPI2;
	delayus(10);						    
	return Response;          //返回收到的数据

}



// Send string to UART
void Print_String(char *str_ptr)
{
    while (*str_ptr != 0x00)
    {
         USART1->DR = *str_ptr;
         while(!(USART1->SR&(1<<6))) ;
        str_ptr++;
    }
    return;
}




//WHOAMI
// 40000091
u32 SPI_3300_ReadID(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;				    
	SPIx_ReadWriteByte(0x40);   
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x91);
	Temp|=SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF); 
	SET_SPI2;				    
	return Temp;
}   		    
 
 
 
 
 
 u8  retemp[4]={0,0,0,0};
 
   
// 40000091
u32 SPI_3300_ReadACCX(void)
{
	u32 Temp=0;
	
	
		  
	CLR_SPI2;
//	delayns(10);
			    
    retemp[0]=SPIx_ReadWriteByte(0x04);   
	retemp[1]=SPIx_ReadWriteByte(0x00); 	    
	retemp[2]=SPIx_ReadWriteByte(0x00); 	    
	retemp[3]=SPIx_ReadWriteByte(0xF7);
/*
 	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=(u32)SPIx_ReadWriteByte(0xFF);   

 
 	retemp[0]=SPIx_ReadWriteByte(0xFF);
	retemp[1]=SPIx_ReadWriteByte(0xFF);
	retemp[2]=SPIx_ReadWriteByte(0xFF);
	retemp[3]=SPIx_ReadWriteByte(0xFF);*/


	Temp=(u32)(retemp[0]<<24)+(u32)(retemp[1]<<16)+(u32)(retemp[2]<<8)+(u32)retemp[3]; 
// 	delayns(10);
    
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   

   
// 40000091
u32 SPI_3300_ReadACCY(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;
			    
   retemp[0]=SPIx_ReadWriteByte(0x08); 	   
   retemp[0]=SPIx_ReadWriteByte(0x00);	 	    
   retemp[0]=SPIx_ReadWriteByte(0x00); 		    
   retemp[0]=SPIx_ReadWriteByte(0xFD);
/*
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<16;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<8;

	Temp|=(u32)SPIx_ReadWriteByte(0xFF);
*/	
	Temp=(u32)(retemp[0]<<24)+(u32)(retemp[1]<<16)+(u32)(retemp[2]<<8)+(u32)retemp[3];	
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   

// 40000091
u32 SPI_3300_ReadACCZ(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;				    
	SPIx_ReadWriteByte(0x0C);//发送读取ID命令	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0xFB);
/*	Temp|=SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 */
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   		 		 




// 40000091
u32 SPI_3300_ReadStatus(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;
			    
	SPIx_ReadWriteByte(0x18);    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0xE5);
	
/*	
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=(u32)SPIx_ReadWriteByte(0xFF);*/	
 
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   

 






// 40000091
u32 SPI_3300_SWREST(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;				    
	SPIx_ReadWriteByte(0xB4);//发送读取ID命令	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x20); 	    
	SPIx_ReadWriteByte(0x98);
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=(u32)SPIx_ReadWriteByte(0xFF); 
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   		 


// 40000091
u32 SPI_3300_READTEMP(void)
{
	u32 Temp = 0;	  
	CLR_SPI2;				    
	SPIx_ReadWriteByte(0x14);//发送读取ID命令	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0xEF);
/*	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<24;
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<16; 	 			   
	Temp|=(u32)SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=(u32)SPIx_ReadWriteByte(0xFF);  */
	SET_SPI2;
	delayus(10);				    
	return Temp;
}   		 



u8 sendbuf[10]={0};


//COMMENT:    3 bit  decimal
//
//
void  numtostring3(float number)
{
       float pp;
	   
       pp=number;
       if(pp<0)
       {
           pp=-pp;
           sendbuf[0]=0x2d;
       }
	   else 
	   {
	   	   sendbuf[0]=0x2b;
	   
	   }
             //////thousand
                 sendbuf[1]=((u8)(((int)pp%10000)/1000)+0x30);
                 sendbuf[2]=((u8)(((int)pp%1000)/100)+0x30);
                 sendbuf[3]=((((int)pp%100)/10)+0x30);
                 sendbuf[4]=(((int)pp%10))+0x30;

}





//
//
//
void  send_ascii(float temp)
{
      
	 u8 i=0;
     numtostring3(temp);
	 for(i=0;i<=4;i++)
	 {
	    USART_Txbyte(sendbuf[i]);  
	 }
//	 USART_Txbyte(0x0d);
//	 USART_Txbyte(0x0a);
}
//
//
//
void  ascii_sendxy(float xx,float yy)
{
       
 	  USART_Txbyte('X');
      USART_Txbyte(':');
      send_ascii(xx);
      USART_Txbyte('Y');
      USART_Txbyte(':');
      send_ascii(yy);

      USART_Txbyte(0x0d);
      USART_Txbyte(0x0a);
}





//  mode1 3g 88hz  /mode2 6g 88hz /mode3 1.5g 88hz /mdoe4 1.5g 10hz
//
//  
void SPI_3300_SetMode(u8 mode)
{
 	u32 Temp = 0;	  
	CLR_SPI2;	
	switch(mode)
	{
	  case 0x01:           //MODE 1
	  {
	       SPIx_ReadWriteByte(0xB4);// 
	       SPIx_ReadWriteByte(0x00); 	    
	       SPIx_ReadWriteByte(0x00); 	    
	       SPIx_ReadWriteByte(0x1F);
		   break;
	  }
	  case 0x02:           //MODE 2
	  {
	       SPIx_ReadWriteByte(0xB4);// 
	       SPIx_ReadWriteByte(0x00); 	    
	       SPIx_ReadWriteByte(0x01); 	    
	       SPIx_ReadWriteByte(0x02);
		   break;
	  }
	  case 0x03:           //MODE 3
	  {
	       SPIx_ReadWriteByte(0xB4);// 
	       SPIx_ReadWriteByte(0x00); 	    
	       SPIx_ReadWriteByte(0x02); 	    
	       SPIx_ReadWriteByte(0x25);
		   break;
	  }
	  case 0x04:           //MODE 4
	  {
	       SPIx_ReadWriteByte(0xB4);// 
	       SPIx_ReadWriteByte(0x00); 	    
	       SPIx_ReadWriteByte(0x03); 	    
	       SPIx_ReadWriteByte(0x38);
		   break;
	  }
	  default:break;
	
	
	}
//	delay_us(1);
//	SET_SPI2;
				    
 	
//	Temp|=SPIx_ReadWriteByte(0xFF)<<24;
//	Temp|=SPIx_ReadWriteByte(0xFF)<<16; 	 			   
//	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
//	Temp|=SPIx_ReadWriteByte(0xFF);	 
	SET_SPI2;
	delayus(10);				    
//	return Temp;
}   		    


u16 Acc_X;
u16 Acc_Y;
u16 Acc_Z;
u16 Temp;
u16 RSdata;
u16 DataError;
u16 DataReady;

u32  Response_Status=0;
u32  readxx=0,readyy=0,readzz=0,readtemp=0;

char Buffer[80];

 u32 tempimare=0;
float   xxxx=0;
float   yyyy=0;
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{

delayms(100);
  /* System clocks configuration ---------------------------------------------*/
  RCC_Configuration();

  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();




  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	      //SPI_NSS_Hard
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_Cmd(SPI2, ENABLE);   

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);
/*
  SET_SPI2;
  delayms(10);
  SPI_3300_SetMode(3);
  delayms(5);
  SPI_3300_ReadStatus();
  SPI_3300_ReadStatus();
//  SPI_3300_SWREST();
  SPI_3300_READTEMP(); */ 
 



	SET_SPI2;

	delayms(10);
	SPI_Send(REQ_WRITE_MODE3);
	delayms(5);
	SPI_Send(REQ_READ_STATUS);
    SPI_Send(REQ_READ_STATUS);
 

	SPI_Send(REQ_READ_TEMP);  

	while(1)
	{
	    
 

/*		tempimare=SPI_3300_ReadID();
		USART_Txbyte((u8)((tempimare&0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare&0x00ff0000)>>16));
		USART_Txbyte((u8)((tempimare&0x0000ff00)>>8));
		USART_Txbyte((u8)((tempimare&0x000000ff)));
		USART_Txbyte(0x66);
		 //////////////////////////
*/

/*		tempimare=SPI_3300_ReadID();
		USART_Txbyte((u8)((tempimare&0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare&0x00ff0000)>>16));
		USART_Txbyte((u8)((tempimare&0x0000ff00)>>8));
		USART_Txbyte((u8)((tempimare&0x000000ff)));
 
		USART_Txbyte(0x66);
		delayms(10);	 */
 

//		readtemp=SPI_3300_ReadACCX();
//		readxx=SPI_3300_ReadACCY();
//		readyy=SPI_3300_ReadACCZ();
//		readzz=SPI_3300_READTEMP();	

//       readtemp=SPI_Send(REQ_READ_ACC_X);
//	   readxx=SPI_Send(REQ_READ_TEMP);

         readxx=SPI_Send(REQ_READ_ACC_X);
         
/*        
        RSdata = (readxx& RS_FIELD_MASK) >> 24;
        if (RSdata != 1)
		{
		   SPI_Send(REQ_READ_STATUS);
		   Response_Status =SPI_Send(REQ_READ_STATUS);           // Request temperature data to get the measurement loop to
                                                                    // continue correctly after reading the status registers
            sprintf(Buffer, "%08X\r\n", Response_Status);
		}
 		else
		{
		    Acc_X = (readxx& DATA_FIELD_MASK)>>8;
		}
		 */
		tempimare=readxx;
		 
	    USART_Txbyte((u8)((tempimare & 0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare & 0xff0000)>>16));
		USART_Txbyte((u8)((tempimare & 0xff00)>>8));
		USART_Txbyte((u8)((tempimare & 0xff)));
 
		USART_Txbyte(0x68);





//	   sprintf(Buffer, "xx:%5d yy:%5d zz:%5d temp:%5d\r\n", Acc_X, readyy, readzz, readtemp);
//       Print_String(Buffer);
	 
	     
/* 	 
		USART_Txbyte((u8)((tempimare & 0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare & 0xff0000)>>16));
		USART_Txbyte((u8)((tempimare & 0xff00)>>8));
		USART_Txbyte((u8)((tempimare & 0xff)));
 
		USART_Txbyte(0x68);*/
 
/*        tempimare=SPI_3300_ReadACCY();
 	 
		USART_Txbyte((u8)((tempimare&0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare&0x00ff0000)>>16));
		USART_Txbyte((u8)((tempimare&0x0000ff00)>>8));
		USART_Txbyte((u8)((tempimare&0x000000ff)));
 
		USART_Txbyte(0x69);
		delayms(100);
		tempimare=SPI_3300_ReadACCZ();
 	 
		USART_Txbyte((u8)((tempimare&0xff000000)>>24));
	    USART_Txbyte((u8)((tempimare&0x00ff0000)>>16));
		USART_Txbyte((u8)((tempimare&0x0000ff00)>>8));
		USART_Txbyte((u8)((tempimare&0x000000ff)));
 
		USART_Txbyte(0x6A);	 */
  		






/*
		if((u8)((tempimare&0x00ff0000)>>16)&0x80)
		{
		     xxxx=-(0xffff-(u16)((tempimare&0x00ffff00)>>8))+1;
		}
		else
		{
		     xxxx=(u16)((tempimare&0x00ffff00)>>8);
		}
		delayms(10);	   */

/*
		tempimare=SPI_3300_ReadACCY();

		if((u8)((tempimare&0x00ff0000)>>16)>=0x80)
		{
		     yyyy=-(0xffff-(u16)((tempimare&0x00ffff00)>>8))+1;
		}
		else
		{
		     yyyy=(u16)((tempimare&0x00ffff00)>>8);
		}


		yyyy=(u16)((tempimare&0x00ffff00)>>8);	*/
//		ascii_sendxy(((float)xxxx),yyyy);
  

	}



 
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK/2 */
    RCC_PCLK2Config(RCC_HCLK_Div2);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
  }

  /* Enable peripheral clocks --------------------------------------------------*/
  /* GPIOA, GPIOB and SPI1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                         , ENABLE);
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);



}

 











/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
