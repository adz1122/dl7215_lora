#ifndef __SCL3300_H
#define __SCL3300_H
#include "main.h"

#define SCL3300_SCK_PIN GPIO_PIN_1
#define SCL3300_SCK_PORT GPIOB
#define SCL3300_MOSI_PIN GPIO_PIN_8
#define SCL3300_MOSI_PORT GPIOB
#define SCL3300_MISO_PIN GPIO_PIN_9
#define SCL3300_MISO_PORT GPIOB
#define SCL3300_CSB_PIN GPIO_PIN_5
#define SCL3300_CSB_PORT GPIOB

#define CSB_HIGH HAL_GPIO_WritePin(SCL3300_CSB_PORT, SCL3300_CSB_PIN, GPIO_PIN_SET)
#define CSB_LOW HAL_GPIO_WritePin(SCL3300_CSB_PORT, SCL3300_CSB_PIN, GPIO_PIN_RESET)
#define SCK_HIGH HAL_GPIO_WritePin(SCL3300_SCK_PORT, SCL3300_SCK_PIN, GPIO_PIN_SET)
#define SCK_LOW HAL_GPIO_WritePin(SCL3300_SCK_PORT, SCL3300_SCK_PIN, GPIO_PIN_RESET)
#define MOSI_HIGH HAL_GPIO_WritePin(SCL3300_MOSI_PORT, SCL3300_MOSI_PIN, GPIO_PIN_SET)
#define MOSI_LOW HAL_GPIO_WritePin(SCL3300_MOSI_PORT, SCL3300_MOSI_PIN, GPIO_PIN_RESET)

#define MISO_IN HAL_GPIO_ReadPin(SCL3300_MISO_PORT, SCL3300_MISO_PIN)
 
extern float temper_value;
extern void scl3300_gpio_init(void);
extern uint16_t SCL3300_GET_TEMP(void);


/******************************************************** 
**SCL3300_D01状态
	SCL3300_D01_Status 
	--SCL3300_D01_OK  = 0
	--SCL3300_D01_RSERR =	1
	--SCL3300_D01_CRCERR =	2
********************************************************/ 	

typedef enum 
{
	SCL3300_D01_OK 			= 0u,
	SCL3300_D01_RSERR 	= 1u,
	SCL3300_D01_CRCERR 	= 2u,
	SCL3300_D01_SPIERR 	= 3u
}SCL3300_D01_Status;

#pragma pack(1)
typedef struct
{
	int16_t ACC_X;
	int16_t ACC_Y;
	int16_t ACC_Z;
	int16_t Temperature;
	int16_t ANG_X;
	int16_t ANG_Y;
	int16_t ANG_Z;
	float Temperature_r;
	float ANG_X_r;
	float ANG_Y_r;
	float ANG_Z_r;
}SCL3300_D01_SensorDataTypeDef;
#pragma pack()

typedef struct
{
	uint8_t RS;
	uint8_t mode;
	uint16_t SCL3300_frame_data;
	SCL3300_D01_Status status;
	SCL3300_D01_SensorDataTypeDef sensor_data_handle;
}SCL3300_D01_HandleTypeDef;



/******************************************************************************
**	                          SCL3300_D01 SPI帧格式
**
**
**	  	RW	|	       ADDR		     |    RS     |		 DATA	   			  |	     CRC
**		  31	| 30	29	28	27	26 |  25   24  | 23	 22	 21..  9  8 | 7	 6	.. 1  0 	
**msb-------------------------------------------------------------------------->lsb
**MOSI	b0	| b4	b3	b2	b1	b0 | '0'  '0'  | b15 b14 b13..b1 b0 | b7 b6 .. b1 b0	
**MISO	b0	| b4	b3	b2	b1	b0 |  b1   b0  | b15 b14 b13..b1 b0 | b7 b6 .. b1 b0	
**
**
**--OP [31:26]	OPERATION CODE RW + ADDR 		OP[5] 	= RW
**  																				OP[4:0] = ADDR
**--RS [25：24]	RETURN STATUS								MISO ‘00’ - Startup in progress					|	MOSI ‘00’ - ALWAYS
**  																						 ‘01’ - Normal operation, no flags	|
**  																						 ‘10’ - Self-test running						|
**  																						 ‘11’ - error												|
*******************************************************************************/ 
#define OP_RW_READ 		0x00u
#define OP_RW_WRITE 	0x80u //READ = 0，WRITE = 1

#define SCL3300_D01_SPI_FRAME_RS_MASK	0x03u
//RS状态
#define RS_STARTUP 		0x00u
#define RS_NORMAL  		0x01u
#define RS_SELFTEST   0x02u
#define RS_ERR  			0x03u
//OPERATION CODE(包括CRC校验)							  BANK|SPI FRAME
#define READ_ACC_X 					0x040000F7u		//0 1 |0000 0100 0000 0000 0000 0000 1111 0111
#define READ_ACC_Y 					0x080000FDu		//0 1 |0000 1000 0000 0000 0000 0000 1111 1101
#define READ_ACC_Z 					0x0C0000FBu		//0 1 |0000 1100 0000 0000 0000 0000 1111 1011
#define READ_STO  					0x100000E9u		//0 1 |0001 0000 0000 0000 0000 0000 1110 1001
#define	ENABLE_ANG_OUTPUTS	0xB0001F6Fu		// 0  |1011 0000 0000 0000 0001 1111 0110 1111
#define READ_ANG_X					0x240000C7u		// 0  |0010 0100 0000 0000 0000 0000 1100 0111
#define READ_ANG_Y					0x280000CDu		// 0	|0010 1000 0000 0000 0000 0000 1100 1101 
#define READ_ANG_Z					0x2C0000CBu		// 0	|0010 1100 0000 0000 0000 0000 1100 1011
#define READ_TEMPERATURE		0x140000EFu		//0 1 |0001 0100 0000 0000 0000 0000 1110 1111
#define READ_STATUS_SUMMARY 0x180000E5u		//0 1 |0001 1000 0000 0000 0000 0000 1110 0101
#define	READ_ERR_FLAG1			0x1C0000E3u		// 0	|0001 1100 0000 0000 0000 0000 1110 0011
#define	READ_ERR_FLAG2			0x200000C1u		// 0	|0010 0000 0000 0000 0000 0000 1100 0001
#define READ_CMD						0x340000DFu		// 0  |0011 0100 0000 0000 0000 0000 1101 1111
#define CHANGE_TO_MODE1			0xB400001Fu		// 0  |1011 0100 0000 0000 0000 0000 0001 1111
#define CHANGE_TO_MODE2			0xB4000102u		// 0  |1011 0100 0000 0000 0000 0001 0000 0010
#define CHANGE_TO_MODE3			0xB4000225u		// 0  |1011 0100 0000 0000 0000 0010 0010 0101
#define CHANGE_TO_MODE4			0xB4000338u		// 0  |1011 0100 0000 0000 0000 0011 0011 1000
#define SET_POWER_DOWN_MODE 0xB400046Bu		// 0  |1011 0100 0000 0000 0000 0100 0110 1011
#define WKUP_FROM_POW_DOWN 	0xB400001Fu		// 0  |1011 0100 0000 0000 0000 0000 0001 1111
#define SW_RESET						0xB4002098u		// 0  |1011 0100 0000 0000 0010 0000 1001 1000
#define READ_WHOAMI  				0x40000091u		// 0	|0100 0000 0000 0000 0000 0000 1001 0001
#define	READ_SERIAL1				0x640000A7u		// 1	|0110 0100 0000 0000 0000 0000 1010 0111
#define READ_SERIAL2				0x680000ADu		// 1	|0110 1000 0000 0000 0000 0000 1010 1101 
#define READ_CURRENT_BANK		0x7C0000B3u		//0 1	|0111 1100 0000 0000 0000 0000 1011 0011
#define	SWITCH_TO_BANK0			0xFC000073u		//0 1	|1111 1100 0000 0000 0000 0000 0111 0011
#define SWITCH_TO_BANK1			0xFC00016Eu		//0 1	|1111 1100 0000 0000 0000 0001 0110 1110



#define WHOAMI_VALUE 0xC1u


/**********************************需要更改配置的宏*********************************************/ 
#define	SCL3300_CS_PIN 			PHout(3)  		//片选信号，根据具体MCU来配置
#define SCL3300_SPI_TIMEOUT 20					//SPI通信方式中，timeout参数，单位ms
#define ERR_CNTR_LIMIT			20							//通信错误次数上限，暂时还未使用



/***************************************************************************************
**灵敏度,单位LSB/g
**mode1: 1.8g	40hz
**mode2: 3.6g	70hz
**mode3: 倾角模式	10hz
**mode4: 倾角模式	10hz 低噪声模式
*******************************************************************************************/ 
#define MODE1_ACC_SENSITIVITY		6000.0
#define MODE2_ACC_SENSITIVITY		3000.0
#define MODE3_4_ACC_SENSITIVITY	12000.0
#define MODE_ANG_SENSITIVITY		182.05
#define TEMPERATURE_SENSITIVITY	18.9

#define MSRMODE1	0x00u
#define MSRMODE2	0x01u
#define MSRMODE3	0x02u
#define MSRMODE4	0x03u
////////////////////////////////////////////////////////////////////////////////////////////
#define DATA_BUF_SIZE 256
extern uint8_t data_buf[DATA_BUF_SIZE];

extern SCL3300_D01_HandleTypeDef SCL3300_D01_Handle;

extern SCL3300_D01_Status	SCL3300_D01_init(uint8_t mode);
extern void SCL3300_D01_SetMeasurementMode(SCL3300_D01_HandleTypeDef *hscl3300, uint8_t mode);
extern uint16_t SCL3300_D01_Read_STATUS(SCL3300_D01_HandleTypeDef *hscl3300);
extern void SCL3300_D01_Enable_Angle_Outputs(SCL3300_D01_HandleTypeDef *hscl3300);
extern uint16_t SCL3300_D01_WHOAMI(SCL3300_D01_HandleTypeDef *hscl3300);
extern void SCL3300_D01_ReadSensorData(SCL3300_D01_HandleTypeDef *hscl3300);
extern uint16_t SCL3300_D01_Read_Err_Flag1(SCL3300_D01_HandleTypeDef *hscl3300);
extern uint16_t SCL3300_D01_Read_Err_Flag2(SCL3300_D01_HandleTypeDef *hscl3300);
extern uint16_t SCL3300_D01_Read_CMD(SCL3300_D01_HandleTypeDef *hscl3300);
extern void SCL3300_D01_PowerDown(SCL3300_D01_HandleTypeDef *hscl3300);
extern void SCL3300_D01_WakeUpFromPowerDown(SCL3300_D01_HandleTypeDef *hscl3300);
extern void SCL3300_D01_SW_RST_CallBack(SCL3300_D01_HandleTypeDef *hscl3300);

#endif
