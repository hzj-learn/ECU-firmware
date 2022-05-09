/*
 ********************************************************************
 * File Name: data_protocol.c
 * Decription:
 ********************************************************************
 */

/* Includes ****************************************************** */
#include "./data_protocol/data_protocol.h"
#include "./usart/bsp_debug_usart.h"

/* Definition of variables *************************************** */																	            
/* 发送数据缓存 */																	
uint8_t data_to_send[50];       	
/* 通讯状态 */															
DataProtocolInfo_t DataProtocolInfo;
DataProtocolInfo_t DriverlessDataProtocolInfo;
/* 控制数据接收 */
CarCtlData_t CarCtlDataFromWireless;
CarCtlData_t CarCtlDataFromPC;

/*
 ***************************************************************
 * @功 能：协议数据发送
 * @描 述：函数是协议中所有发送数据功能使用到的发送函数
 *				 移植时，用户应根据自身应用的情况
 *         根据使用的通信方式，实现此函数
 * @返 回：None
 ***************************************************************
 */
void Serial_Send_Data(uint8_t *dataToSend , uint8_t length)
{
		//HAL_UART_Transmit(&huart1, dataToSend, length, 0xffff);
}

/*
 ***************************************************************
 * @功 能：协议数据发送
 * @描 述：函数是协议中所有发送数据功能使用到的发送函数
 *				 移植时，用户应根据自身应用的情况
 *         根据使用的通信方式，实现此函数
 * @返 回：None
 ***************************************************************
 */
void Serial_Send_Data_2(uint8_t *dataToSend , uint8_t length)
{
   USART1_Put_Buf(dataToSend, length);
}



/*
 ***************************************************************
 * @功    能：add心跳数据累加
 * @描    述：放在周期循环里面进行心跳计算
 * @入口参数：
 * @返    回：None
 ***************************************************************
 */
void addTxHearBeatData(void)
{
	DataProtocolInfo.TxFrameInfo.heart_beat_data ++;
}

/*
 ***************************************************************
 * @功    能：获取《发送》心跳数据
 * @描    述：
 * @入口参数：
 * @返    回：16位心跳数据
 ***************************************************************
 */
uint16_t getTxHearBeatData(void)
{
	return DataProtocolInfo.TxFrameInfo.heart_beat_data;
}

/*
 ***************************************************************
 * @功    能：获取《接收》心跳数据累加
 * @描    述：
 * @入口参数：
 * @返    回：16位心跳数据
 ***************************************************************
 */
uint16_t getRxHearBeatData(void)
{
	return DataProtocolInfo.RxFrameInfo.heart_beat_data;
}

/*
 ***************************************************************
 * @功    能：获取数据协议连接状态
 * @描    述：
 * @入口参数：
 * @返    回：16位心跳数据
 ***************************************************************
 */
uint16_t getDataProtocolConnectState(void)
{
	return DataProtocolInfo.Statue.connect_statue;
}

/*
 ***************************************************************
 * @功 能：《解析》数据协议
 * @描 述：经过数据《预解析》后，进行《解析》
 * @返 回：None
 ***************************************************************
 */
void Data_Receive_Parsed(uint8_t *data_buf, uint8_t num)
{
	uint8_t sum = 0;
	uint8_t i = 0;
	
	/* 求和校验 */
	for(i=0; i<(num-1); i++)
		sum += *(data_buf + i);
	/* 判断sum  校验 */
	if(!(sum==*(data_buf + num - 1)))		
		return;											
	if(!(*(data_buf + 0) == PC_FRAME_HEAD_L && *(data_buf+1) == PC_FRAME_HEAD_H))		
		return;		
	
	/* 获取数据帧类型 */
	DataProtocolInfo.RxFrameInfo.frame_type = *(data_buf + 2);
	/* 获取数据标识符 */
	DataProtocolInfo.RxFrameInfo.identifier = *(data_buf + 3);
	/* 获取数据长度信息 */
	DataProtocolInfo.RxFrameInfo.data_len = *(data_buf + 4);
	
	/* 接收的是数据帧 */
	if ( DataProtocolInfo.RxFrameInfo.frame_type == PC_FRAME_TYPE_DATA )
	{
		CarCtlDataFromWireless.running_mode    = *(data_buf + 5);
		CarCtlDataFromWireless.running_statue  = *(data_buf + 6);
		CarCtlDataFromWireless.gear_position   = *(data_buf + 7);
		CarCtlDataFromWireless.steer_dir       = *(data_buf + 8);
		CarCtlDataFromWireless.brake_state     = *(data_buf + 9);
		CarCtlDataFromWireless.throttle_val    = (uint16_t)(*(data_buf + 11) << 8) | *( data_buf + 10 );
		CarCtlDataFromWireless.torque_val      = (uint16_t)(*(data_buf + 13) << 8) | *( data_buf + 12 );
		CarCtlDataFromWireless.raw_steer_angle     = (uint16_t)(*(data_buf + 15) << 8) | *( data_buf + 14 );
		CarCtlDataFromWireless.heart_beat_data = (uint16_t)(*(data_buf + 17) << 8) | *( data_buf + 16 );
	  DataProtocolInfo.RxFrameInfo.heart_beat_data = CarCtlDataFromWireless.heart_beat_data;
		
		/* 根据标识符获取相应的数据 */
//		if ( DataProtocolInfo.RxFrameInfo.identifier ==  PC_DATA_IDENTIFIER_TEST )
//		{
//			DataProtocolInfo.RxFrameInfo.data_test = (uint16_t)(*(data_buf + 5) << 8) | *( data_buf + 6 );
//			DataProtocolInfo.RxFrameInfo.heart_beat_data = (uint16_t)(*(data_buf + 7) << 8) | *( data_buf + 8 );
//		}
	}
}

/*
 ***************************************************************
 * @功 能：《预解析》数据协议
 * @描 述：串口每接收到一字节数据，则调用函数一次
 *				 数据会自行解析
 * @返 回：None
 ***************************************************************
 */
void DataProtocolPrepareParsed (uint8_t data)
{
	static uint8_t RxBuffer[50];
	static uint8_t _data_len = 0, _data_cnt = 0;
	static uint8_t state = 0;
	 
	if(state == 0 && data == PC_FRAME_HEAD_L)                 /* 帧头-低8位 */
	{
		state = 1;
		RxBuffer[0] = data;
	}
	else if(state == 1 && data == PC_FRAME_HEAD_H)            /* 帧头-高8位 */
	{
		state=2;
		RxBuffer[1] = data;
	}
	else if(state == 2 && data < 0x04)             						 /* 帧类型 */          
	{
		state=3;
		RxBuffer[2]=data;
	}
	else if(state == 3 )             												   /* 数据标识符 */          
	{
		state=4;
		RxBuffer[3]=data;
	}
	else if(state == 4 && data < 50)              						 /* 数据长度LED */
	{
		state = 5;
		RxBuffer[4] = data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state == 5 && _data_len > 0)            					 /* RxBuffer[4]开始保存数据  RxBuffer[]=数据，长度决定保存数据的多少 */
	{
		_data_len--;
		RxBuffer[5 + _data_cnt++] = data;
		if(_data_len == 0)
			state = 6;
	}
	else if(state==6)                                					/* 检验  放在数据帧后面 */
	{
		state = 0;
		RxBuffer[5 + _data_cnt] = data;
		Data_Receive_Parsed(RxBuffer, _data_cnt + 6); 				 /* 一帧数据保存完毕，进入函数进行 数据的保存处理 */
	}
	else
	{
		state = 0;
	}
}

/*
 ***************************************************************
 * @功 能：《发送》协议数据帧---《测试》
 * @描 述：串口每接收到一字节数据，则调用函数一次
 *				 数据会自行解析
 * @返 回：None
 ***************************************************************
 */
void ECU_SendDataFrameTest(int16_t data_test, uint16_t heart_beat_data)
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_SEND_TEST_DATA;
	data_to_send[_cnt++] = 0;
	
	data_to_send[_cnt++] = BYTE0(data_test);
	data_to_send[_cnt++] = BYTE1(data_test);
	data_to_send[_cnt++] = BYTE0(heart_beat_data);
	data_to_send[_cnt++] = BYTE1(heart_beat_data);
	
	data_to_send[4] = _cnt-5;
	
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++] = sum;
	Serial_Send_Data(data_to_send, _cnt);
}
void ECU_SendDataFrameToAnswerPCReqTest(void)
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_ANSWER_PC_REQ_TEST;
	data_to_send[_cnt++] = 0;
	
	data_to_send[_cnt++] = 0xFF;
	data_to_send[_cnt++] = 0xFF;
	data_to_send[_cnt++] = 0xFF;
	data_to_send[_cnt++] = 0xFF;
	
	data_to_send[4] = _cnt-5;
	
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++] = sum;
	Serial_Send_Data(data_to_send, _cnt);
}
void ECU_SendAnswerFrameToAnswerPCReqTest(void)
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_ANSWER;
	data_to_send[_cnt++] = ECU_IDENTIFIER_ANSWER_TEST;
	data_to_send[_cnt++] = 0;
	
	data_to_send[_cnt++] = 0xFF;
	data_to_send[_cnt++] = 0xFF;
	
	data_to_send[4] = _cnt-5;
	
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++] = sum;
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @功 能：《发送》协议数据帧--- 小车数据帧
 * @描 述：串口每接收到一字节数据，则调用函数一次
 *				 数据会自行解析
 * @返 回：None
 ***************************************************************
 */
void ECU_SendDataFrame(int16_t car_moton_dir,         /* 方向 */
													int16_t car_speed,            /* 速度 */
														int16_t turning_radius,     /* 转弯半径 */
															int16_t _heart_beat_data) /* 心跳数据 */
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_SEND_CAR_REAL_DATA;
	data_to_send[_cnt++] = 0;
	
	/* 数据内容 */
	data_to_send[_cnt++] = BYTE1(car_moton_dir);
	data_to_send[_cnt++] = BYTE0(car_moton_dir);
	data_to_send[_cnt++] = BYTE1(car_speed);
	data_to_send[_cnt++] = BYTE0(car_speed);
	data_to_send[_cnt++] = BYTE1(turning_radius);
	data_to_send[_cnt++] = BYTE0(turning_radius);
	data_to_send[_cnt++] = BYTE1(_heart_beat_data);
	data_to_send[_cnt++] = BYTE0(_heart_beat_data);

	/* 数据长度 */
	data_to_send[4] = _cnt-5;
	
	/* 求和校验 */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* 数据发送 */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @功 能：《发送》协议数据帧--- 小车状态信息
 * @描 述：串口每接收到一字节数据，则调用函数一次
 *				 数据会自行解析
 * @返 回：None
 ***************************************************************
 */
void ECU_SendDataFrameForExecutiveAgency(uint8_t gear_position,           /* 档位 */
																							 uint8_t braking_state,          /* 刹车状态 */
																								int16_t throttle_val,          /* 油门大小 */
																								 uint16_t _heart_beat_data)    /* 心跳数据 */
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_SEND_CAR_RAW_DATA;
	data_to_send[_cnt++] = 0;
	
	/* 数据内容 */
	data_to_send[_cnt++] = (uint8_t)gear_position;
	data_to_send[_cnt++] = (uint8_t)braking_state;
	data_to_send[_cnt++] = BYTE1(throttle_val);
	data_to_send[_cnt++] = BYTE0(throttle_val);
	data_to_send[_cnt++] = BYTE1(throttle_val);
	data_to_send[_cnt++] = BYTE0(throttle_val);
	data_to_send[_cnt++] = BYTE1(_heart_beat_data);
	data_to_send[_cnt++] = BYTE0(_heart_beat_data);

	/* 数据长度 */
	data_to_send[4] = _cnt-5;
	
	/* 求和校验 */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* 数据发送 */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @功 能：《发送》协议数据帧--- 小车信息
 * @描 述：主动上传数据
 * @返 回：None
 ***************************************************************
 */
void ECU_SendDataFrameForCarInfo(void)   
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	int16_t aa = 1111;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_ATIVE_UPLOAD_DATA;
	data_to_send[_cnt++] = 0;
	
	/* 数据内容 */
	data_to_send[_cnt++] = BYTE1(aa);
	data_to_send[_cnt++] = BYTE0(aa);

	data_to_send[_cnt++] = (short)(1000)>>8;
	data_to_send[_cnt++] = (short)(1000)&0xff;
	
	data_to_send[_cnt++] = (short)(2000)>>8;
	data_to_send[_cnt++] = (short)(2000)&0xff;
	
	data_to_send[_cnt++] = (short)(3000)>>8;
	data_to_send[_cnt++] = (short)(3000)&0xff;
	
	data_to_send[_cnt++] = (short)(4000)>>8;
	data_to_send[_cnt++] = (short)(4000)&0xff;
	
	data_to_send[_cnt++] = (short)(5000)>>8;
	data_to_send[_cnt++] = (short)(5000)&0xff;
	
	data_to_send[_cnt++] = (short)(6000)>>8;
	data_to_send[_cnt++] = (short)(6000)&0xff;
	
	data_to_send[_cnt++] = (short)(7000)>>8;
	data_to_send[_cnt++] = (short)(7000)&0xff;
	
	data_to_send[_cnt++] = (short)(8000)>>8;
	data_to_send[_cnt++] = (short)(8000)&0xff;
	
	data_to_send[_cnt++] = (short)(9000)>>8;
	data_to_send[_cnt++] = (short)(9000)&0xff;
	
	
	data_to_send[_cnt++] = BYTE1(DataProtocolInfo.RxFrameInfo.heart_beat_data);
	data_to_send[_cnt++] = BYTE0(DataProtocolInfo.RxFrameInfo.heart_beat_data);

	/* 数据长度 */
	data_to_send[4] = _cnt-5;
	
	/* 求和校验 */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* 数据发送 */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @功    能：检测通讯链路是否成功通讯
 * @描    述：当超过最大出错次数，认为通讯出错
 * @入口参数：param_1: 心跳数据
 *            param_2：最大出错次数
 * @返    回：通讯正常-----0x00
 *				    通讯出错-----0xff
 ***************************************************************
 */
uint8_t ConnectCheck(int16_t _heart_beat_data, int16_t max_error_times)
{
  int16_t this_time_heart_beat = 0;
	static  int16_t last_time_heart_beat = 0;
	static  int16_t error_times = 0;
	
	/* 获取本次心跳数据 */
	this_time_heart_beat = _heart_beat_data;
	
	if( this_time_heart_beat == last_time_heart_beat )
	{
		/* 通讯出错,记录 */
		error_times++;
	}
	else
	{
		error_times = 0;
	}
	
	/* 更新心跳数据 */
	last_time_heart_beat = this_time_heart_beat;
	
	/* 超出最大通讯出错值,返回0xff */
	if( error_times > max_error_times)
	{
		error_times = max_error_times + 1;
		return DP_ERROR;
	}
	
	/* 通讯正常,返回0x00 */
	return DP_NORMAL;
	
}

/*
 ***************************************************************
 * @功    能：通讯出错后的任务函数
 * @描    述：None
 * @入口参数：None
 * @返    回：None
 ***************************************************************
 */
void ConnectCheckErrorTask(void)
{
	DataProtocolInfo.RxFrameInfo.frame_type 			  = 0x00;
	DataProtocolInfo.RxFrameInfo.identifier 		 		= 0x00;
	DataProtocolInfo.RxFrameInfo.data_identifier 		= 0x00;
	DataProtocolInfo.RxFrameInfo.request_identifier = 0x00;
	DataProtocolInfo.RxFrameInfo.answer_identifier 	= 0x00; 
}

/*
 ***************************************************************
 * @功    能：自检任务
 * @描    述：None
 * @入口参数：None
 * @返    回：None
 ***************************************************************
 */
void CheckTask(void)
{
		DataProtocolInfo.Statue.connect_statue = ConnectCheck(getRxHearBeatData(), 2);
		/* 自检出错 */
		if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
		{
			ConnectCheckErrorTask();
		}
}
/*
 ***************************************************************
 * @功    能：蓝牙无线控制自检任务
 * @描    述：None
 * @入口参数：None
 * @返    回：None
 ***************************************************************
 */
void BluetoothRemoteCheckTask(void)
{
	if ( DataProtocolInfo.RxFrameInfo.heart_beat_data != 0 )
	{
	  DataProtocolInfo.Statue.connect_statue = ConnectCheck(getRxHearBeatData(), 2);
		/* 自检出错 */
		if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
		{
			ConnectCheckErrorTask();
		}
	}
}
/*
 ***************************************************************
 * @功    能：通讯协议初始化
 * @描    述：None
 * @入口参数：None
 * @返    回：None
 ***************************************************************
 */
void DataProcotolInit(void)
{
	/* 初始化心跳数据 */
	DataProtocolInfo.TxFrameInfo.heart_beat_data = 0;
	DataProtocolInfo.RxFrameInfo.heart_beat_data = 0;
}
	
/*
 ***************************************************************
 * @功    能：通讯接收处理函数
 * @描    述：此函数放在周期循环里面，
 * @入口参数：周期时间（ms） 5ms
 * @返    回：
 ***************************************************************
 */
void DataProtocolCommunicationTask (int16_t cycle_time_ms)
{
	/* 通讯监测 */
	//Serial_Communication_Check(data_from_pc.heart_beat_data, 10);
	static int16_t cycle_times;
	cycle_times++;
	if ( cycle_times > 20000 )
		cycle_times = 0;
	
	/* 每1个周期循环处理的任务 */
	if ( 1 )
	{
	}
	
	/* 每2个周期循环处理的任务 cycle_time_ms == 5ms--->100hz */
	if ( cycle_times%2 == 0 )
	{
		
		/* 测试请求 */
		if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_REQ_IDENTFIER_TEST )
		{
			/* 清空，只返回一次 */
			DataProtocolInfo.RxFrameInfo.request_identifier = 0x00;		
			/* 返回测试数据内容 0xFF */
			ECU_SendAnswerFrameToAnswerPCReqTest();
		}
		
		/* 请求一次数据		*/
		else if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_ONCE )
		{
			DataProtocolInfo.RxFrameInfo.request_identifier = 0x00;
			ECU_SendDataFrameForCarInfo();
		}
		
		/* 10ms主动上传请求		*/
		else if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_TX_EVERY_10MS )
		{
			ECU_SendDataFrameForCarInfo();
		}
		
	}
	
	/* 每4个周期循环处理的任务  cycle_time_ms == 5ms--->50hz  */
	if ( cycle_times%4 == 0 )
	{
		/* add心跳数据 */
		addTxHearBeatData();
		/* 通讯自检 */
		CheckTask();
		
		/* 20ms主动上传请求 */
		if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_TX_EVERY_20MS )
		{
			ECU_SendDataFrameForCarInfo();
		}
			
	}
}









/*
 ***************************************************************
 * @功 能：《解析》数据协议
 * @描 述：经过数据《预解析》后，进行《解析》
 * @返 回：None
 ***************************************************************
 */
void Driverless_Data_Receive_Parsed(uint8_t *data_buf, uint8_t num)
{
	uint8_t sum = 0;
	uint8_t i = 0;
	
	/* 求和校验 */
	for(i=0; i<(num-1); i++)
		sum += *(data_buf + i);
	/* 判断sum  校验 */
	if(!(sum==*(data_buf + num - 1)))		
		return;											
	if(!(*(data_buf + 0) == PC_FRAME_HEAD_L && *(data_buf+1) == PC_FRAME_HEAD_H))		
		return;		
	
	/* 获取数据帧类型 */
	DriverlessDataProtocolInfo.RxFrameInfo.frame_type = *(data_buf + 2);
	/* 获取数据标识符 */
	DriverlessDataProtocolInfo.RxFrameInfo.identifier = *(data_buf + 3);
	/* 获取数据长度信息 */
	DriverlessDataProtocolInfo.RxFrameInfo.data_len   = *(data_buf + 4);
	
	/* 接收的是数据帧 */
//	if ( DriverlessDataProtocolInfo.RxFrameInfo.frame_type == PC_FRAME_TYPE_DATA )
//	{
//		 if ( DriverlessDataProtocolInfo.RxFrameInfo.identifier == 0xff )
//		 {
			 CarCtlDataFromPC.gear_position     = *(data_buf + 5);
			 CarCtlDataFromPC.throttle_position = *(data_buf + 6);
			 CarCtlDataFromPC.raw_steer_angle   = (uint16_t)(*(data_buf + 8) << 8) | *( data_buf + 7 );
			 CarCtlDataFromPC.brake_state       = *(data_buf + 9);
			 CarCtlDataFromPC.heart_beat_data   = (uint16_t)(*(data_buf + 11) << 8) | *( data_buf + 10 );
			 DriverlessDataProtocolInfo.RxFrameInfo.heart_beat_data = CarCtlDataFromWireless.heart_beat_data;
//		 }
//			 

//	}
}

/*
 ***************************************************************
 * @功 能：《预解析》数据协议
 * @描 述：串口每接收到一字节数据，则调用函数一次
 *				 数据会自行解析
 * @返 回：None
 ***************************************************************
 */
void Driverless_DataProtocolPrepareParsed (uint8_t data)
{
	static uint8_t RxBuffer[50];
	static uint8_t _data_len = 0, _data_cnt = 0;
	static uint8_t state = 0;
	 
	if(state == 0 && data == PC_FRAME_HEAD_L)                 /* 帧头-低8位 */
	{
		state = 1;
		RxBuffer[0] = data;
	}
	else if(state == 1 && data == PC_FRAME_HEAD_H)            /* 帧头-高8位 */
	{
		state=2;
		RxBuffer[1] = data;
	}
	else if(state == 2 && data < 0x04)             						 /* 帧类型 */          
	{
		state=3;
		RxBuffer[2]=data;
	}
	else if(state == 3 )             												   /* 数据标识符 */          
	{
		state=4;
		RxBuffer[3]=data;
	}
	else if(state == 4 && data < 50)              						 /* 数据长度LED */
	{
		state = 5;
		RxBuffer[4] = data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state == 5 && _data_len > 0)            					 /* RxBuffer[4]开始保存数据  RxBuffer[]=数据，长度决定保存数据的多少 */
	{
		_data_len--;
		RxBuffer[5 + _data_cnt++] = data;
		if(_data_len == 0)
			state = 6;
	}
	else if(state==6)                                					/* 检验  放在数据帧后面 */
	{
		state = 0;
		RxBuffer[5 + _data_cnt] = data;
		Driverless_Data_Receive_Parsed(RxBuffer, _data_cnt + 6); 				 /* 一帧数据保存完毕，进入函数进行 数据的保存处理 */
	}
	else
	{
		state = 0;
	}
}



