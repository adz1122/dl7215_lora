#include "sediment.h"
#include "usart.h"
#include "string.h"

uint16_t sediment_data[2];

void handle_sediment_data(){
	  if(Uart2Para.fDataReceived){
		  Uart2Para.fDataReceived = false;
		  if((Uart2Para.TotalRxBuf[0]==0x7E) && (Uart2Para.TotalRxBuf[24]==0xE7)){
			  sediment_data[0] = Uart2Para.TotalRxBuf[17] << 8 | Uart2Para.TotalRxBuf[18];//高字节在前，低字节在后
			  sediment_data[1] = Uart2Para.TotalRxBuf[19] << 8 | Uart2Para.TotalRxBuf[20];
		  Uart2Para.TotalRxCnt = 0;
		  memset(Uart2Para.TotalRxBuf, 0, UART2_RX_BUF_MAX_LEN);
	  }
	  }
}

//char PrepareSettlingData(void)
//{
//		int i;
//		//g_nPktLen = 16+(1+g_nSensorNum*2+2)+2;
//		g_nPktLen = 16+(1+2*2+2)+2;
//		aPktBuf[0] = 0x7E;
//		aPktBuf[1] = g_nPktLen>>8;
//		aPktBuf[2] = g_nPktLen&0xff;	
//	
//		aPktBuf[3] = 0xA1; //STSP_PKT_TYPE_SNRPT;
//		aPktBuf[4] = strDeviceID[0];        //self node ID
//		aPktBuf[5] = strDeviceID[1];       //cluster ID
//		aPktBuf[6] = strDeviceID[2];       //self node ID
//		aPktBuf[7] = strDeviceID[3];       //cluster ID
//		aPktBuf[8] = strDeviceID[4];       //dest node ID
//		aPktBuf[9] = strDeviceID[5];       //dest node ID
//		aPktBuf[10] = time_now.tm_year%100;
//		aPktBuf[11] = time_now.tm_mon;
//		aPktBuf[12] = time_now.tm_mday;
//		aPktBuf[13] = time_now.tm_hour;
//		aPktBuf[14] = time_now.tm_min;
//		aPktBuf[15] = g_nFrameNo;  //	作为帧序列号
//		g_nFrameNo++;
//	
//		aPktBuf[16] = 2; // g_nSensorNum;  //Number of Sesnors
//		
//    //for(i=0; i<g_nSensorNum; i++)
//		for(i=0; i<2; i++)
//		{		
//				aPktBuf[17+i*2] = (unsigned char)(aSettling[i]>>8);
//				aPktBuf[18+i*2] = (unsigned char)(aSettling[i] & 0xff);
//		}
//	
//		
//    aPktBuf[g_nPktLen-4] = g_nRssi;
//		aPktBuf[g_nPktLen-3] = g_nGwBattVolt;           //voltage
//		aPktBuf[g_nPktLen-2] = CalcCheckSum(aPktBuf, g_nPktLen-2);
//		aPktBuf[g_nPktLen-1] = 0xE7;
//		return S_OK;
//}

//void HandleSensorUdpRptDataTilt(string strSubID, byte[] aRecBuf)  //
//        {
//            bool bRet;
//            short wVal;
//            DateTime dateTime = new DateTime();
//            float fSettling = 0;
//            float fSettling1 = 0;
//            float fTemp1 = 0;
//            float fTemp2 = 0;
//            float fSettling2 = 0;345
//            float fSettling3 = 0;
//            float fTemp3 = 0;
//            float fSettling4 = 0;
//            float fTemp4 = 0;
//            float fSettling5 = 0;
//            float fTemp5 = 0;
//            float fBattVolt = 0;
//            byte nRssi = 0;
//            int nVal;
//            int year, mon, day, hour, min;
//            ushort uwPktLen;
//            //try{    
//            //    year = 2000 + aRecBuf[10];
//            //    mon = aRecBuf[11];
//            //    day = aRecBuf[12];
//            //    hour = aRecBuf[13];
//            //    min = aRecBuf[14];
//            //   dateTime = new DateTime(year, mon & 0x0f, day & 0x1f, hour, min, 0);
//            //}
//            //catch (Exception ex)
//            //{
//            //    Funclib.WriteLog(ex.Message);
//            //    return;
//            //}
//            dateTime = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, DateTime.Now.Hour, DateTime.Now.Minute, 0);
//            //dateTime = DateTime.Now;  //日?期ú采é用?PC机ú时骸?间?
//            uwPktLen = (ushort)(aRecBuf[1] * 256 + aRecBuf[2]);
//            //wVal = Convert.ToInt16(aRecBuf[16] * 256 + aRecBuf[17]);  //错洙?
//            wVal =(short)(aRecBuf[16] * 256 + aRecBuf[17]);
//            fSettling = Convert.ToSingle(wVal)/10;

//            wVal = (short)(aRecBuf[18] * 256 + aRecBuf[19]);
//            fSettling1 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[20] * 256 + aRecBuf[21]);
//            fTemp1 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[22] * 256 + aRecBuf[23]);
//            fSettling2 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[24] * 256 + aRecBuf[25]);
//            fTemp2 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[26] * 256 + aRecBuf[27]);
//            fSettling3 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[28] * 256 + aRecBuf[29]);
//            fTemp3 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[30] * 256 + aRecBuf[31]);
//            fSettling4 = Convert.ToSingle(wVal) / 10;

//            wVal = (short)(aRecBuf[32] * 256 + aRecBuf[33]);
//            fTemp4 = Convert.ToSingle(wVal) / 10;

//            if (uwPktLen == 38)  //V1.2
//            {
//                nRssi = aRecBuf[34];
//                fBattVolt = Convert.ToSingle(aRecBuf[35]) / 10;
//            }
//            else if (uwPktLen == 42)    //v1.3
//            {
//                wVal = (short)(aRecBuf[34] * 256 + aRecBuf[35]);
//                fSettling5 = Convert.ToSingle(wVal) / 10;

//                wVal = (short)(aRecBuf[36] * 256 + aRecBuf[37]);
//                fTemp5 = Convert.ToSingle(wVal) / 10; 
//                nRssi = aRecBuf[38];
//                fBattVolt = Convert.ToSingle(aRecBuf[39]) / 10;
//            }
//            int nIndex = 0;
//            //if (myDictionary.ContainsKey(m_entNodeThb.SubID) == true) //found
//            if (SearchNodeCfg(strSubID, ref nIndex) == true) //found
//            {
//                aNodeTTL[nIndex] = MAX_NODE_TTL;
//                aFrameSno[nIndex] = aRecBuf[15];
//            }
//            if (m_bDbConnOk)                         //check data connection
//            {
//                m_entNodeThb.dateTime = dateTime;
//                m_entNodeThb.SubID = strSubID;
//                m_entNodeThb.Settling = fSettling;
//                m_entNodeThb.Settling1 = fSettling1;
//                m_entNodeThb.Temp1 = fTemp1;
//                m_entNodeThb.Settling2 = fSettling2;
//                m_entNodeThb.Temp2 = fTemp2;
//                m_entNodeThb.Settling3 = fSettling3;
//                m_entNodeThb.Temp3 = fTemp3;
//                m_entNodeThb.Settling4 = fSettling4;
//                m_entNodeThb.Temp4 = fTemp4;
//                m_entNodeThb.Settling5 = fSettling5;
//                m_entNodeThb.Temp5 = fTemp5;
//                m_entNodeThb.Rssi = nRssi;
//                m_entNodeThb.BattVolt = fBattVolt;
//                EntNodeTHBDbHelper.Insert(m_entNodeThb);     //插?入?到?历え?史骸?数簓据Y表括?中D

//                //if ((fSettling1 < lstStationCfgs[nIndex].XL) || (fSettling1 > lstStationCfgs[nIndex].XH) || (fTemp2 < lstStationCfgs[nIndex].YL) || (fTemp2 > lstStationCfgs[nIndex].YH))
//                //{
//                //    m_entNodeAlarm.dateTime = dateTime;              //超?界?，?则ò记?录?
//                //    m_entNodeAlarm.SubID = strSubID;
//                //    m_entNodeAlarm.Settling = fSettling1;
//                //    m_entNodeAlarm.fSettling1 = fSettling;
//                //    m_entNodeAlarm.fTemp1 = fTemp1;
//                //    m_entNodeAlarm.fSettling2 = fTemp2;
//                //    m_entNodeAlarm.fTemp2 = fSettling2;
//                //    m_entNodeAlarm.fSettling3 = fSettling3;
//                //    m_entNodeAlarm.fTemp3 = fTemp3;
//                //    m_entNodeAlarm.fSettling4 = fSettling4;
//                //    m_entNodeAlarm.fTemp4 = fTemp4;
//                //    m_entNodeAlarm.fSettling5 = fSettling5;
//                //    m_entNodeAlarm.fTemp5 = fTemp5;  
//                //    m_entNodeAlarm.Rssi = nRssi;
//                //    m_entNodeAlarm.BattVolt = fBattVolt;
//                //    m_entNodeAlarm.Status = 0;
//                //    m_entNodeAlarm.Description = "-";
//                //    EntNodeAlarmDbHelper.Insert(m_entNodeAlarm);
//                //}  

//                
//                //if (myDictionary.ContainsKey(m_entNodeThb.SubID) == true) //found
//                if (SearchNodeCfg(m_entNodeThb.SubID, ref nIndex) == true) //found
//                {
//                    m_entRealTimeThb.SubID = m_entNodeThb.SubID;   //插?入?到?实害?时骸?数簓据Y表括?中D,用?于?显?示?
//                    m_entRealTimeThb.TowerName = lstStationCfgs[nIndex].TowerName;
//                    m_entRealTimeThb.dateTime = m_entNodeThb.dateTime;
//                    m_entRealTimeThb.Settling = m_entNodeThb.Settling;
//                    m_entRealTimeThb.Settling1 = m_entNodeThb.Settling1;
//                    m_entRealTimeThb.Temp1 = m_entNodeThb.Temp1;
//                    m_entRealTimeThb.Settling2 = m_entNodeThb.Settling2;
//                    m_entRealTimeThb.Temp2 = m_entNodeThb.Temp2;
//                    m_entRealTimeThb.Settling3 = m_entNodeThb.Settling3;
//                    m_entRealTimeThb.Temp3 = m_entNodeThb.Temp3;
//                    m_entRealTimeThb.Settling4 = m_entNodeThb.Settling4;
//                    m_entRealTimeThb.Temp4 = m_entNodeThb.Temp4;
//                    m_entRealTimeThb.Settling5 = m_entNodeThb.Settling5;
//                    m_entRealTimeThb.Temp5 = m_entNodeThb.Temp5;
//                    m_entRealTimeThb.Rssi = m_entNodeThb.Rssi;
//                    m_entRealTimeThb.BattVolt = m_entNodeThb.BattVolt;
//				}
//			}
//		}
//		
//		
//char SendCmd_Sag_42H(unsigned char nUnitId)  // 
//{
//    int i = 0;
//		char ret;
//    static unsigned char nFrameFlag = 0;
//    PKT_WAVE pktData;

//    //forge pkt
//    pktData.startcode = FLAG_BEGIN;
//    for(i = 0; i < DEVICE_NAME_LEN; i++)
//    {
//        pktData.devicenumber[i] = cfgGwInfo.strDeviceName[i];
//    }
//    pktData.cmd = CMD_RPT_WAVE;
//    pktData.aDataLen[0] =0;  //fixed ref to the protocol
//    pktData.aDataLen[1] =24;  //7+17
//		
//    for(i = 0; i < DEVICE_PASSWORD_LEN; i++)
//		{
//				pktData.verify[i] = cfgGwInfo.strPassword[0];
//		}
//    pktData.frameflag = nFrameFlag++;
//    pktData.count = 1;
//    pktData.UnitId = nUnitId;
// 

//		if((nUnitId&0xF0) == 0x10)
//		{
//				pktData.year   =  recSag[0].Year;   //?
//				pktData.month  =  recSag[0].Month;       //?
//				pktData.day    =  recSag[0].Day;          //?
//				pktData.hour   =   recSag[0].Hour;          //?
//				pktData.minute =   recSag[0].Minute;           //?
//				pktData.second =  recSag[0].Second;           //?
//				pktData.aSag[0] = recSag[0].uwSag / 256;
//				pktData.aSag[1] = recSag[0].uwSag % 256;
//				pktData.aHeight[0] = recSag[0].uwHeight / 256;
//				pktData.aHeight[1] = recSag[0].uwHeight % 256;
//				pktData.SensorVoltage = recSag[0].SensorVoltage;
//    }
//		else if((nUnitId&0xF0) == 0x20)
//		{
//				pktData.year   =  recSag[1].Year;   //?
//				pktData.month  =  recSag[1].Month;       //?
//				pktData.day    =  recSag[1].Day;          //?
//				pktData.hour   =   recSag[1].Hour;          //?
//				pktData.minute =   recSag[1].Minute;           //?
//				pktData.second =  recSag[1].Second;           //?
//				pktData.aSag[0] = recSag[1].uwSag / 256;
//				pktData.aSag[1] = recSag[1].uwSag % 256;
//				pktData.aHeight[0] = recSag[1].uwHeight / 256;
//				pktData.aHeight[1] = recSag[1].uwHeight % 256;
//			
//				pktData.SensorVoltage = recSag[1].SensorVoltage;
//    }
//		else if((nUnitId&0xF0) == 0x30)
//		{
//				pktData.year   =  recSag[2].Year;   //?
//				pktData.month  =  recSag[2].Month;       //?
//				pktData.day    =  recSag[2].Day;          //?
//				pktData.hour   =   recSag[2].Hour;          //?
//				pktData.minute =   recSag[2].Minute;           //?
//				pktData.second =  recSag[2].Second;           //?
//				pktData.aSag[0] = recSag[2].uwSag / 256;
//				pktData.aSag[1] = recSag[2].uwSag % 256;
//				pktData.aHeight[0] = recSag[2].uwHeight / 256;
//				pktData.aHeight[1] = recSag[2].uwHeight % 256;
//				pktData.SensorVoltage = recSag[1].SensorVoltage;
//    }
//    pktData.sumcode = CalcChecksumSouthGrid((unsigned char *)&pktData, sizeof(PKT_WAVE) - 2);
//    pktData.tail = FLAG_END;

//		ret = GsmSendPkt((unsigned char *)&pktData, sizeof(PKT_WAVE), (unsigned short)GPRS_RET_WAITTIME_SHORT, 3);
//		
//    return ret;
//}


