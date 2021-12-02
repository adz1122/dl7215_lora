#include "scl3300.h"
#include "delay.h"
#include "string.h"

float temper_value;
uint8_t data_buf[DATA_BUF_SIZE];
void ByteToHexStr(const unsigned char* source, uint8_t* dest, int sourceLen);

void scl3300_gpio_init(){	
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin	= SCL3300_SCK_PIN | SCL3300_MOSI_PIN | SCL3300_CSB_PIN;       
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    
	GPIO_InitStruct.Pull = GPIO_PULLUP;    
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	HAL_GPIO_Init(SCL3300_SCK_PORT, &GPIO_InitStruct);		
	
 
	GPIO_InitStruct.Pin = SCL3300_MISO_PIN;       
	GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;  
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed	= GPIO_SPEED_FREQ_HIGH;     
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);		
		
	CSB_HIGH;

}

//SPI���ͼ���������
uint8_t scl3300_send_recv_byte(uint8_t data)
{
	uint8_t recv_data = 0, i;
 
	//�˴�����
	SCK_LOW;
	
	for(i=0; i<8; i++)
	{
		
		//׼�����͵�����
		if(data & (1<<(7-i)))
		{
			MOSI_HIGH;
		}
		else
		{
			MOSI_LOW;
		}
		
		
		delay_us(2);
		SCK_HIGH;
		delay_us(2);
		
		//��λ���ݺϳ��ֽ�
		if(MISO_IN)
		{
			recv_data |= 1<<(7-i);
		
		}
		
		SCK_LOW;
	}
	
	
 
	return recv_data;
}

static uint8_t CRC8(uint8_t BitValue, uint8_t CRC_)
{
	uint8_t Temp;
	Temp = (uint8_t)(CRC_ & 0x80);
	if (BitValue == 0x01)
	{
		Temp ^= 0x80;
	}
	CRC_ <<= 1;
	if (Temp > 0)
	{
		CRC_ ^= 0x1D;
	}
	return CRC_;
}

uint8_t CalculateCRC(uint32_t Data)
{
	uint8_t BitIndex;
	uint8_t BitValue;
	uint8_t CRC_;
	CRC_ = 0xFF;
	for (BitIndex = 31; BitIndex > 7; BitIndex--)
	{
		BitValue = (uint8_t)((Data >> BitIndex) & 0x01);
		CRC_ = CRC8(BitValue, CRC_);
	}
	CRC_ = (uint8_t)~CRC_;
	return CRC_;
}

//�¶ȣ�0x140000EFh
uint16_t SCL3300_GET_TEMP(void)
{
	uint8_t rs;
    uint8_t tmp_rcv_idx = 0;
	uint8_t tmp_rcv_data[4];
    uint16_t result = 0;
	uint32_t rx_data = 0;
	uint8_t crc8;
	
	delay_us(20);
	CSB_LOW;
	tmp_rcv_idx = 0;
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x14);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x00);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x00);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0xEF);
	CSB_HIGH;
	
	rs = tmp_rcv_data[0] & 0x03;//ȷ��״̬��
//	if(rs != 0x01) 
//		return -1;
	
	rx_data = (tmp_rcv_data[0]<<24) | (tmp_rcv_data[1]<<16) | (tmp_rcv_data[2]<<8) | tmp_rcv_data[3];
	crc8 = CalculateCRC(rx_data);//ȷ��CRC
	if(crc8 != tmp_rcv_data[3]) 
		return -1;
	
	//��дһ������
	delay_us(20);
	CSB_LOW;
	tmp_rcv_idx = 0;
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x14);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x00);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0x00);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(0xEF);
	CSB_HIGH;
	
	rs = tmp_rcv_data[0] & 0x03;
	if(rs != 0x01) 
		return -1;
	
	rx_data = (tmp_rcv_data[0]<<24) | (tmp_rcv_data[1]<<16) | (tmp_rcv_data[2]<<8) | tmp_rcv_data[3];
	crc8 = CalculateCRC(rx_data);
	if(crc8 != tmp_rcv_data[3]) 
		return -1;
	
	result = tmp_rcv_data[1]<<8 | tmp_rcv_data[2];
	temper_value = ((int16_t)result) * 1.0/ 18.9 - 273;
	
	return result;
}




static uint8_t CalculateCRC(uint32_t Data);
static SCL3300_D01_Status SCL3300_TransmitReceive(SCL3300_D01_HandleTypeDef *hscl3300,uint32_t SCL3300_Command);
SCL3300_D01_HandleTypeDef SCL3300_D01_Handle;


/************************************************************************************************************** 
**SCL3300_D01_Status	SCL3300_D01_init(void)
**��ʼ������
**
****---����mode: 
**-------				MSRMODE1
**-------				MSRMODE2
**-------				MSRMODE3
**-------				MSRMODE4
****--����SPI_HandleTypeDef��
**-------main.c�����е�spi������ݽ������ɣ�֮ǰҪ��spi�����ʼ����
**ע��:1.SCL3300��ʼ��Ҫ��SPI��ʼ����delay��ʼ���������
**		 2.���߷���
				Pin 				|		Pin Name 									|Communication
				--------------------------------------------------------------------
				CSB 				|		Chip Select (active low) 	|MCU				->		SCL3300
				SCK 				|		Serial Clock 							|MCU				->		SCL3300
				MOSI 				|		Master Out Slave In 			|MCU 				->		SCL3300
				MISO 				|		Master In Slave Out 			|SCL3300		->		MCU
**************************************************************************************************************/ 
SCL3300_D01_Status	SCL3300_D01_init (uint8_t mode)
{ 
  SCL3300_D01_Handle.RS = 0u;									//RSΪ��ǰ����֡����������RS״̬
	SCL3300_D01_Handle.mode = MSRMODE1;					//����ģʽΪģʽ1
	SCL3300_D01_Handle.SCL3300_frame_data = 0u;				//SCL3300_DataΪ��ǰ��������16bit����
	SCL3300_D01_Handle.status = SCL3300_D01_OK;	//statusΪ���ݵ�ǰRS��CRC״̬�жϵ�����״̬

	
	SCL3300_D01_Handle.sensor_data_handle.ACC_X = 0;
	SCL3300_D01_Handle.sensor_data_handle.ACC_Y = 0;
	SCL3300_D01_Handle.sensor_data_handle.ACC_Z = 0;
	SCL3300_D01_Handle.sensor_data_handle.Temperature = 0;
	SCL3300_D01_Handle.sensor_data_handle.ANG_X = 0;
	SCL3300_D01_Handle.sensor_data_handle.ANG_Y = 0;
	SCL3300_D01_Handle.sensor_data_handle.ANG_Z = 0;//��ʼ��SCL3300_D01_Handle��sensor_data_handle������

	temper_value = -99.9;
	memset(data_buf, 0, DATA_BUF_SIZE);
	
	//*********SCL3300_D01  Start-Up Sequence***************************	
	//-																																	//1.ϵͳ�ϵ磬VDD 	3.0V~3.6V
	//-																																								DVIO  3.0V~3.6V
	
  delay_ms(10);																										  	//2.�ȴ�10ms
	 
	SCL3300_D01_SetMeasurementMode(&SCL3300_D01_Handle, mode);			  	//3.���ò���ģʽ	
	 
	delay_ms(5);															  												//4.�ȴ�5ms
	 
	do
   {
   	SCL3300_D01_Read_STATUS(&SCL3300_D01_Handle);
   } while (SCL3300_D01_Handle.status != SCL3300_D01_OK);							//5.6.7.���յ�RS = ��01����ʾ������������
   
	SCL3300_D01_Enable_Angle_Outputs(&SCL3300_D01_Handle);							//8.ʹ�ܽǶ����
	return SCL3300_D01_OK;																							//9.�����������
}  




/************************************************************************************************************************ 
**void SCL3300_D01_SetMeasurementMode(SCL3300_D01_HandleTypeDef *hscl3300, uint8_t mode)
**�л�����ģʽ
**mode1: 1.8g	40hz
**mode2: 3.6g	70hz
**mode3: ���ģʽ	10hz
**mode4: ���ģʽ	10hz ������ģʽ
**---����mode: 
**-------MSRMODE1
**-------MSRMODE2
**-------MSRMODE3
**-------MSRMODE4
**ע��:1.ģʽ˵��
					Mode	|	                Acceleration output 						|Inclination output |	Acceleration and
								|																									|										|	Inclination output
					������������|������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
								|Full-scale |	Sensitivity LSB/g |Sensitivity ��/g  |	Sensitivity ��/g 	|	1st pass filter st order low
								|������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
						1 	|�� 1.8 g 		|	6000 							|			105 				|				182 				|								40 Hz
								|������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
						2 	|�� 3.6 g 		|	3000 							|			52 					|				182 				|								70 Hz
								|������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
						3 	|Inclination|
								|	mode 			|	2000 							|			209 				|				182 				|								10 Hz
								|������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
						4 	|Inclination|
								|	mode 			|	12000 						|			209 				|				182 				|								10 Hz
******************************************************************************************************************************/ 
void SCL3300_D01_SetMeasurementMode(SCL3300_D01_HandleTypeDef *hscl3300, uint8_t mode)
{
	uint32_t tmp_mode = CHANGE_TO_MODE1;
	switch (mode)
  {
  	case MSRMODE1:
			tmp_mode = CHANGE_TO_MODE1;
  		break;
  	case MSRMODE2:
			tmp_mode = CHANGE_TO_MODE2;
  		break;
		case MSRMODE3:
			tmp_mode = CHANGE_TO_MODE3;
  		break;
		case MSRMODE4:
			tmp_mode = CHANGE_TO_MODE4;
  		break;
  	default:
			tmp_mode = CHANGE_TO_MODE1;
  		break;
  }
	do
  {
  	SCL3300_TransmitReceive(hscl3300,tmp_mode);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR);
	
	hscl3300->mode = mode;
}





/********************************************************************************************************************************** 
**uint16_t SCL3300_D01_Read_STATUS(SCL3300_D01_HandleTypeDef *hscl3300)
**��ȡSTATUS �Ĵ���
**
**---����������uint16_t  ΪSTATUS �Ĵ����� ����
**
**ע��:1.�Ĵ�������
**			D15 D14 D13 D12 D11 D10 |	D9		|		D8 	|	D7 	|	D6 	|			D5 		|	D4 	|	D3 	|	D2 	|			D1 			|					D0			|		Bit
--------------------------------------------------------------------------------------------------------
								Reserved				|DIGI1	|	DIGI2	|	CLK	|	SAT	|	TEMP_SAT	|	PWR	|	MEM	|	PD	|	MODE_CHANGE	|	PIN_CONTINUITY	|		Read

**********************************************************************************************************************************/ 
uint16_t SCL3300_D01_Read_STATUS(SCL3300_D01_HandleTypeDef *hscl3300)
{
	SCL3300_TransmitReceive(hscl3300,READ_STATUS_SUMMARY);
	do
  {
  	SCL3300_TransmitReceive(hscl3300,READ_STATUS_SUMMARY);
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	return hscl3300->SCL3300_frame_data;
}


/********************************************************************************************************************************** 
**uint16_t SCL3300_D01_Read_CMD(SCL3300_D01_HandleTypeDef *hscl3300)
**��ȡCMD �Ĵ���
**
**---����������uint16_t CMD �Ĵ�������
**
**ע��:1.�Ĵ�������
**			D15 D14 D13 D12 D11 D10 D9 D8 	|			D7 			|		D6 			  |		D5 		|	    D4 			|			D3 		  |			D2 	  |			D1 				D0			|		Bit
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
											Reserved			  	|	factory use	|	factory use	|	SW_RST	|	factory use	|	factory use	|		  PD		|					MODE					|		Read


				Bit 					Name 							Description
---------------------------------------------------------------------------------------
				15:8 					Reserved 					Reserved
					7 					Factory use 			Factory use
					6 					Factory use 			Factory use
					5 					SW_RST 						Software (SW) Reset
					4 					Factory use 			Factory use
					3 					Factory use 			Factory use
					2 					PD 								Power Down
			   1:0 					MODE 							Operation Mode
**********************************************************************************************************************************/ 
uint16_t SCL3300_D01_Read_CMD(SCL3300_D01_HandleTypeDef *hscl3300)
{
	SCL3300_TransmitReceive(hscl3300,READ_CMD);
	do
  {
  	SCL3300_TransmitReceive(hscl3300,READ_CMD);
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	return hscl3300->SCL3300_frame_data;
}





/********************************************************************************************************************************** 
**uint16_t SCL3300_D01_Read_Err_Flag1(SCL3300_D01_HandleTypeDef *hscl3300)
**��ȡERR_FLAG1 �Ĵ���
**
**---����������uint16_t ERR_FLAG1 �Ĵ�������
**
**ע��:1.�Ĵ�������
**			D15 D14 D13 D12 | 	D11 	| D10 	D9		D8 		D7 		D6 		D5 		D4 		D3 		D2 		D1 			|	D0	|		Bit
-----------------------------------------------------------------------------------------------------------------
					Reserved 			|	ADC_SAT |															AFE_SAT														|	MEM	|		Read


				Bit 					Name 							Description
---------------------------------------------------------------------------------------
				15:12				  Reserved 					Reserved
					11 					ADC_SAT 					Signal saturated at A2D
				10:1 					AFE_SAT 					Signal saturated at C2V
					0 					MEM 							Error in non-volatile memory
**********************************************************************************************************************************/ 
uint16_t SCL3300_D01_Read_Err_Flag1(SCL3300_D01_HandleTypeDef *hscl3300)
{
	SCL3300_TransmitReceive(hscl3300,READ_ERR_FLAG1);
	do
  {
  	SCL3300_TransmitReceive(hscl3300,READ_ERR_FLAG1);
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	return hscl3300->SCL3300_frame_data;
}

/********************************************************************************************************************************** 
**uint16_t SCL3300_D01_Read_Err_Flag2(SCL3300_D01_HandleTypeDef *hscl3300)
**��ȡERR_FLAG2 �Ĵ���
**
**---����������uint16_t ERR_FLAG2 �Ĵ�������
**
**ע��:1.�Ĵ�������
**			D15 	 | 		D14 		 | 	D13 	  |		 D12 	 | 		D11 	|    	D10 	  | 	   D9	  	  |	 D8 	|  
------------------------------------------------------------------------------------------------------
		 Reserved  |		D_EXT_C	 |	A_EXT_C |		AGND	 |		VDD		|		Reserved	|		MODE_CHANGE	|	 PD		|		

  	   D7	 	   | 		 D6 		 |   D5 	  |   	D4 	 |	  D3  	|	     D2  	  |			 D1 		  |	  D0	|		Bit
--------------------------------------------------------------------------------------------------------
		MEMORY_CRC |	Reserved   |	APWR    |	  DPWR   |	  VREF  |	    APWR_2  |		TEMP_SAT    |	  CLK	|		Read
			


				Bit 					Name 							Description
---------------------------------------------------------------------------------------
				15 						Reserved 					Reserved
				14						D_EXT_C 					External capacitor connection error
				13 						A_EXT_C 					External capacitor connection error
				12 						AGND 							Analog ground connection error
				11 						VDD 							Supply voltage error
				10 						Reserved 					Reserved
				9 						MODE_CHANGE 			Operation mode changed by user
				8 						PD 								Device in power down mode
				7 						MEMORY_CRC 				Memory CRC check failed
				6 						Reserved 					Reserved
				5 						APWR 							Analog power error
				4 						DPWR 							Digital power error
				3 						VREF 							Reference voltage error
				2					 		APWR_2 						Analog power error
				1 						TEMP_SAT 					Temperature signal path saturated
				0 						CLK 							Clock error
**********************************************************************************************************************************/ 
uint16_t SCL3300_D01_Read_Err_Flag2(SCL3300_D01_HandleTypeDef *hscl3300)
{
	SCL3300_TransmitReceive(hscl3300,READ_ERR_FLAG2);
	do
  {
  	SCL3300_TransmitReceive(hscl3300,READ_ERR_FLAG2);
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	return hscl3300->SCL3300_frame_data;
}



/********************************************************************************************************************************** 
**uint16_t SCL3300_D01_WHOAMI(SCL3300_D01_HandleTypeDef *hscl3300)
**����SPI���߹����Ƿ�����
**
**
**ע��:
************************************************************************************************************************************/ 
uint16_t SCL3300_D01_WHOAMI(SCL3300_D01_HandleTypeDef *hscl3300)
{
	SCL3300_TransmitReceive(hscl3300,READ_WHOAMI);
	do
  {
  	SCL3300_TransmitReceive(hscl3300,READ_WHOAMI);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	return hscl3300->SCL3300_frame_data;
}

/********************************************************************************************************************************** 
**void SCL3300_D01_ReadSensorData(SCL3300_D01_HandleTypeDef *hscl3300)
**��ȡSCL3300�ļ��ٶ�����ͽǶȼĴ������
**
**
**ע��:1.��ȡ����������ֻ��ȫ����ȷ��hscl3300->status��ΪOK
************************************************************************************************************************************/ 
void SCL3300_D01_ReadSensorData(SCL3300_D01_HandleTypeDef *hscl3300)
{	
	SCL3300_D01_Status tmp_status[7];
	uint8_t tmp_i;
		
	do
	{
  	SCL3300_TransmitReceive(hscl3300,READ_ACC_X);	
		
		SCL3300_TransmitReceive(hscl3300,READ_ACC_Y);
		hscl3300->sensor_data_handle.ACC_X =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[0] = hscl3300->status;
		
		SCL3300_TransmitReceive(hscl3300,READ_ACC_Z);
		hscl3300->sensor_data_handle.ACC_Y =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[1] = hscl3300->status;
	
		SCL3300_TransmitReceive(hscl3300,READ_TEMPERATURE);
		hscl3300->sensor_data_handle.ACC_Z =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[2] = hscl3300->status;
	
		SCL3300_TransmitReceive(hscl3300,READ_ANG_X);
		hscl3300->sensor_data_handle.Temperature =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[3] = hscl3300->status;
	
		SCL3300_TransmitReceive(hscl3300,READ_ANG_Y);
		hscl3300->sensor_data_handle.ANG_X =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[4] = hscl3300->status;
		
		SCL3300_TransmitReceive(hscl3300,READ_ANG_Z);
		hscl3300->sensor_data_handle.ANG_Y =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[5] = hscl3300->status;
		
		SCL3300_TransmitReceive(hscl3300,READ_WHOAMI);
		hscl3300->sensor_data_handle.ANG_Z =(int16_t) hscl3300->SCL3300_frame_data;
		tmp_status[6] = hscl3300->status;
		
		hscl3300->sensor_data_handle.Temperature_r = -273 + hscl3300->sensor_data_handle.Temperature / 18.9;
		hscl3300->sensor_data_handle.ANG_X_r = (90.0 / 16384) * hscl3300->sensor_data_handle.ANG_X;
		hscl3300->sensor_data_handle.ANG_Y_r = (90.0 / 16384) * hscl3300->sensor_data_handle.ANG_Y;
		hscl3300->sensor_data_handle.ANG_Z_r = (90.0 / 16384) * hscl3300->sensor_data_handle.ANG_Z;
		
		uint8_t *data_ptr = (uint8_t *) (&(hscl3300->sensor_data_handle));
		ByteToHexStr(data_ptr, data_buf, 14);
		
		for( tmp_i = 0 ; tmp_i < 7 ; tmp_i++ )
		{
			if(tmp_status[tmp_i] == SCL3300_D01_SPIERR)
			{
				hscl3300->status = SCL3300_D01_SPIERR;
				return;
			}
			else if(tmp_status[tmp_i] == SCL3300_D01_CRCERR)
						{
							hscl3300->status = SCL3300_D01_CRCERR;
							return;
						}
						else if(tmp_status[tmp_i] == SCL3300_D01_RSERR)
									{
										hscl3300->status = SCL3300_D01_RSERR;
										return;
									}
		}
	}while(hscl3300->status != SCL3300_D01_OK);
		hscl3300->status = SCL3300_D01_OK;
	
	
		
		
		return;
}



/********************************************************************************************************************************** 
**void SCL3300_D01_Enable_Angle_Outputs(SCL3300_D01_HandleTypeDef *hscl3300)
**ʹ�ܽǶȼĴ������
**
**
**ע��:1.ֻд�Ĵ���
************************************************************************************************************************************/ 
void SCL3300_D01_Enable_Angle_Outputs(SCL3300_D01_HandleTypeDef *hscl3300)
{
	do
  {
  	SCL3300_TransmitReceive(hscl3300,ENABLE_ANG_OUTPUTS);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
}

/********************************************************************************************************************************** 
**void SCL3300_D01_PowerDown(SCL3300_D01_HandleTypeDef *hscl3300)
**����POWER Down
**
**
**ע��
************************************************************************************************************************************/ 
void SCL3300_D01_PowerDown(SCL3300_D01_HandleTypeDef *hscl3300)
{
	do
  {
  	SCL3300_TransmitReceive(hscl3300,SET_POWER_DOWN_MODE);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
}

/********************************************************************************************************************************** 
**void SCL3300_D01_WakeUpFromPowerDown(SCL3300_D01_HandleTypeDef *hscl3300)
**����
**
**
**ע��:1.���Ѻ���Ҫ���³�ʼ��SCL3300_D01
************************************************************************************************************************************/ 
void SCL3300_D01_WakeUpFromPowerDown(SCL3300_D01_HandleTypeDef *hscl3300)
{
	do
  {
  	SCL3300_TransmitReceive(hscl3300,WKUP_FROM_POW_DOWN);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
}


/********************************************************************************************************************************** 
**void SCL3300_D01_SW_RST(SCL3300_D01_HandleTypeDef *hscl3300)
**����
**
**
**ע��:1.RST��,���ûص�����void SCL3300_D01_SW_RST_CallBack(SCL3300_D01_HandleTypeDef *hscl3300)���ص������б��������ʼ������
************************************************************************************************************************************/ 
void SCL3300_D01_SW_RST(SCL3300_D01_HandleTypeDef *hscl3300)
{
	do
  {
  	SCL3300_TransmitReceive(hscl3300,SW_RESET);	
  } while (hscl3300->status == SCL3300_D01_CRCERR || hscl3300->status == SCL3300_D01_SPIERR); 
	SCL3300_D01_SW_RST_CallBack(hscl3300);
}

__weak void SCL3300_D01_SW_RST_CallBack(SCL3300_D01_HandleTypeDef *hscl3300)
{
	(void*)hscl3300;
								//����������Ĺ��������û�app��ʵ��
}
/********************************************************************************************************************************** 
**static SCL3300_D01_Status SCL3300_TransmitReceive(SCL3300_D01_HandleTypeDef *hscl3300,uint32_t SCL3300_Command)
**spiͨ�ź���
**������SCL3300_Command
**--�����
					READ_ACC_X 						0x040000F7u		
					READ_ACC_Y 						0x080000FDu		
					READ_ACC_Z 						0x0C0000FBu		
					READ_STO  						0x100000E9u		
					ENABLE_ANG_OUTPUTS		0xB0001F6Fu		
					READ_ANG_X						0x240000C7u		
					READ_ANG_Y						0x280000CDu		 
					READ_ANG_Z						0x2C0000CBu		
					READ_TEMPERATURE			0x140000EFu		
					READ_STATUS_SUMMARY 	0x180000E5u		
					READ_ERR_FLAG1				0x1C0000E3u		
					READ_ERR_FLAG2				0x200000C1u		
					READ_CMD							0x340000DFu		
					CHANGE_TO_MODE1				0xB400001Fu		
					CHANGE_TO_MODE2				0xB4000102u	
					CHANGE_TO_MODE3				0xB4000225u		
					CHANGE_TO_MODE4				0xB4000338u		
					SET_POWER_DOWN_MODE 	0xB400046Bu		
					WKUP_FROM_POW_DOWN 		0xB400001Fu		
					SW_RESET							0xB4002098u		
					READ_WHOAMI  					0x40000091u		
					READ_SERIAL1					0x640000A7u		
					READ_SERIAL2					0x680000ADu		
					READ_CURRENT_BANK			0x7C0000B3u		
					SWITCH_TO_BANK0				0xFC000073u		
					SWITCH_TO_BANK1				0xFC00016Eu	
**ע��:1.�����ڲ�ʹ��
			 2.SPI������ʱ���������  �� SCL3300_SPI_TIMEOUT
*****************************************************************************************************************************************/ 
static SCL3300_D01_Status SCL3300_TransmitReceive(SCL3300_D01_HandleTypeDef *hscl3300,uint32_t SCL3300_Command)
{
	uint8_t tmp_rcv_idx;
	uint8_t tmp_rcv_data[4];
	uint8_t tmp_tsmt_data[4];
	uint32_t tmp_crc;
	
	hscl3300->status = SCL3300_D01_OK;//��ʼ������״̬
	//------------------------------------------------------------------------------
	tmp_tsmt_data[0] = (uint8_t) (SCL3300_Command>>24);
	tmp_tsmt_data[1] = (uint8_t) (SCL3300_Command>>16);
	tmp_tsmt_data[2] = (uint8_t) (SCL3300_Command>>8);
	tmp_tsmt_data[3] = (uint8_t) SCL3300_Command;//��Ҫ���͵�����д��tmp_tsmt_data
	//------------------------------------------------------------------------------
	delay_us(20);
	CSB_LOW;
	tmp_rcv_idx = 0;
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(tmp_tsmt_data[0]);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(tmp_tsmt_data[1]);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(tmp_tsmt_data[2]);
	tmp_rcv_data[tmp_rcv_idx++] = scl3300_send_recv_byte(tmp_tsmt_data[3]);
	CSB_HIGH;
			
	delay_us(10);
	//---------------------------------CRCУ��---------------------------------------------
	tmp_crc = ((uint32_t)tmp_rcv_data[0]<<24) + ((uint32_t)tmp_rcv_data[1]<<16)+((uint32_t)tmp_rcv_data[2]<<8);
	if(CalculateCRC(tmp_crc) != tmp_rcv_data[3])
	{
		hscl3300->status = SCL3300_D01_CRCERR;
		return hscl3300->status;				//CRC У��ʧ��ֱ�ӷ��أ�����״̬Ϊ  SCL3300_D01_CRCERR
	}
	//---------------------------------------------------------------------------------------

	hscl3300->RS = tmp_rcv_data[0] & SCL3300_D01_SPI_FRAME_RS_MASK;
	if(hscl3300->RS == RS_ERR)
		hscl3300->status = SCL3300_D01_RSERR;//�õ�RS״̬��RS ==��11����״̬����ΪSCL3300_D01_RSERR
	else
		hscl3300->status = SCL3300_D01_OK;	
	
	hscl3300->SCL3300_frame_data = ((uint16_t)tmp_rcv_data[1]<<8) + tmp_rcv_data[2];//ȡ��16bit������
	
	return hscl3300->status;
}


///************************************************************************************************************************ 
//**CRCУ��
//	static uint8_t CRC8(uint8_t BitValue, uint8_t pCRC)
//	static uint8_t CalculateCRC(uint32_t Data)

//		Parameter 		|	Value
//---------------------------------------------------------
//		Name 					|	CRC-8
//		Width 				|	8 bit
//		Poly 					|	1Dh (generator polynom: X8+X4+X3+X2+1)
//		Init 					|	FFh (initialization value)
//		XOR out 			|	FFh (inversion of CRC result)
//************************************************************************************************************************/ 
//static uint8_t CRC8(uint8_t BitValue, uint8_t pCRC)
//{
//	uint8_t Temp;
//	Temp = (uint8_t)(pCRC & 0x80);
//	if (BitValue == 0x01)
//	{
//		Temp ^= 0x80;
//	}
//		pCRC <<= 1;
//	if (Temp > 0)
//	{
//		pCRC ^= 0x1D;
//	}
//	return pCRC;
//}
//static uint8_t CalculateCRC(uint32_t Data)
//{
//	uint8_t BitIndex;
//	uint8_t BitValue;
//	uint8_t tmp_CRC;
//	tmp_CRC = 0xFF;
//	for (BitIndex = 31; BitIndex > 7; BitIndex--)
//	{
//		BitValue = (uint8_t)((Data >> BitIndex) & 0x01);
//		tmp_CRC = CRC8(BitValue, tmp_CRC);
//	}
//		tmp_CRC = (uint8_t)~tmp_CRC;
//		return tmp_CRC;
//}

void ByteToHexStr(const unsigned char* source, uint8_t* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
 
    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;
 
        highByte += 0x30;
 
        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;
 
        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}
