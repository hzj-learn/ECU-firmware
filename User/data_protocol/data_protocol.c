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
/* �������ݻ��� */																	
uint8_t data_to_send[50];       	
/* ͨѶ״̬ */															
DataProtocolInfo_t DataProtocolInfo;
DataProtocolInfo_t DriverlessDataProtocolInfo;
/* �������ݽ��� */
CarCtlData_t CarCtlDataFromWireless;
CarCtlData_t CarCtlDataFromPC;

/*
 ***************************************************************
 * @�� �ܣ�Э�����ݷ���
 * @�� ����������Э�������з������ݹ���ʹ�õ��ķ��ͺ���
 *				 ��ֲʱ���û�Ӧ��������Ӧ�õ����
 *         ����ʹ�õ�ͨ�ŷ�ʽ��ʵ�ִ˺���
 * @�� �أ�None
 ***************************************************************
 */
void Serial_Send_Data(uint8_t *dataToSend , uint8_t length)
{
		//HAL_UART_Transmit(&huart1, dataToSend, length, 0xffff);
}

/*
 ***************************************************************
 * @�� �ܣ�Э�����ݷ���
 * @�� ����������Э�������з������ݹ���ʹ�õ��ķ��ͺ���
 *				 ��ֲʱ���û�Ӧ��������Ӧ�õ����
 *         ����ʹ�õ�ͨ�ŷ�ʽ��ʵ�ִ˺���
 * @�� �أ�None
 ***************************************************************
 */
void Serial_Send_Data_2(uint8_t *dataToSend , uint8_t length)
{
   USART1_Put_Buf(dataToSend, length);
}



/*
 ***************************************************************
 * @��    �ܣ�add���������ۼ�
 * @��    ������������ѭ�����������������
 * @��ڲ�����
 * @��    �أ�None
 ***************************************************************
 */
void addTxHearBeatData(void)
{
	DataProtocolInfo.TxFrameInfo.heart_beat_data ++;
}

/*
 ***************************************************************
 * @��    �ܣ���ȡ�����͡���������
 * @��    ����
 * @��ڲ�����
 * @��    �أ�16λ��������
 ***************************************************************
 */
uint16_t getTxHearBeatData(void)
{
	return DataProtocolInfo.TxFrameInfo.heart_beat_data;
}

/*
 ***************************************************************
 * @��    �ܣ���ȡ�����ա����������ۼ�
 * @��    ����
 * @��ڲ�����
 * @��    �أ�16λ��������
 ***************************************************************
 */
uint16_t getRxHearBeatData(void)
{
	return DataProtocolInfo.RxFrameInfo.heart_beat_data;
}

/*
 ***************************************************************
 * @��    �ܣ���ȡ����Э������״̬
 * @��    ����
 * @��ڲ�����
 * @��    �أ�16λ��������
 ***************************************************************
 */
uint16_t getDataProtocolConnectState(void)
{
	return DataProtocolInfo.Statue.connect_statue;
}

/*
 ***************************************************************
 * @�� �ܣ�������������Э��
 * @�� �����������ݡ�Ԥ�������󣬽��С�������
 * @�� �أ�None
 ***************************************************************
 */
void Data_Receive_Parsed(uint8_t *data_buf, uint8_t num)
{
	uint8_t sum = 0;
	uint8_t i = 0;
	
	/* ���У�� */
	for(i=0; i<(num-1); i++)
		sum += *(data_buf + i);
	/* �ж�sum  У�� */
	if(!(sum==*(data_buf + num - 1)))		
		return;											
	if(!(*(data_buf + 0) == PC_FRAME_HEAD_L && *(data_buf+1) == PC_FRAME_HEAD_H))		
		return;		
	
	/* ��ȡ����֡���� */
	DataProtocolInfo.RxFrameInfo.frame_type = *(data_buf + 2);
	/* ��ȡ���ݱ�ʶ�� */
	DataProtocolInfo.RxFrameInfo.identifier = *(data_buf + 3);
	/* ��ȡ���ݳ�����Ϣ */
	DataProtocolInfo.RxFrameInfo.data_len = *(data_buf + 4);
	
	/* ���յ�������֡ */
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
		
		/* ���ݱ�ʶ����ȡ��Ӧ������ */
//		if ( DataProtocolInfo.RxFrameInfo.identifier ==  PC_DATA_IDENTIFIER_TEST )
//		{
//			DataProtocolInfo.RxFrameInfo.data_test = (uint16_t)(*(data_buf + 5) << 8) | *( data_buf + 6 );
//			DataProtocolInfo.RxFrameInfo.heart_beat_data = (uint16_t)(*(data_buf + 7) << 8) | *( data_buf + 8 );
//		}
	}
}

/*
 ***************************************************************
 * @�� �ܣ���Ԥ����������Э��
 * @�� ��������ÿ���յ�һ�ֽ����ݣ�����ú���һ��
 *				 ���ݻ����н���
 * @�� �أ�None
 ***************************************************************
 */
void DataProtocolPrepareParsed (uint8_t data)
{
	static uint8_t RxBuffer[50];
	static uint8_t _data_len = 0, _data_cnt = 0;
	static uint8_t state = 0;
	 
	if(state == 0 && data == PC_FRAME_HEAD_L)                 /* ֡ͷ-��8λ */
	{
		state = 1;
		RxBuffer[0] = data;
	}
	else if(state == 1 && data == PC_FRAME_HEAD_H)            /* ֡ͷ-��8λ */
	{
		state=2;
		RxBuffer[1] = data;
	}
	else if(state == 2 && data < 0x04)             						 /* ֡���� */          
	{
		state=3;
		RxBuffer[2]=data;
	}
	else if(state == 3 )             												   /* ���ݱ�ʶ�� */          
	{
		state=4;
		RxBuffer[3]=data;
	}
	else if(state == 4 && data < 50)              						 /* ���ݳ���LED */
	{
		state = 5;
		RxBuffer[4] = data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state == 5 && _data_len > 0)            					 /* RxBuffer[4]��ʼ��������  RxBuffer[]=���ݣ����Ⱦ����������ݵĶ��� */
	{
		_data_len--;
		RxBuffer[5 + _data_cnt++] = data;
		if(_data_len == 0)
			state = 6;
	}
	else if(state==6)                                					/* ����  ��������֡���� */
	{
		state = 0;
		RxBuffer[5 + _data_cnt] = data;
		Data_Receive_Parsed(RxBuffer, _data_cnt + 6); 				 /* һ֡���ݱ�����ϣ����뺯������ ���ݵı��洦�� */
	}
	else
	{
		state = 0;
	}
}

/*
 ***************************************************************
 * @�� �ܣ������͡�Э������֡---�����ԡ�
 * @�� ��������ÿ���յ�һ�ֽ����ݣ�����ú���һ��
 *				 ���ݻ����н���
 * @�� �أ�None
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
 * @�� �ܣ������͡�Э������֡--- С������֡
 * @�� ��������ÿ���յ�һ�ֽ����ݣ�����ú���һ��
 *				 ���ݻ����н���
 * @�� �أ�None
 ***************************************************************
 */
void ECU_SendDataFrame(int16_t car_moton_dir,         /* ���� */
													int16_t car_speed,            /* �ٶ� */
														int16_t turning_radius,     /* ת��뾶 */
															int16_t _heart_beat_data) /* �������� */
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_SEND_CAR_REAL_DATA;
	data_to_send[_cnt++] = 0;
	
	/* �������� */
	data_to_send[_cnt++] = BYTE1(car_moton_dir);
	data_to_send[_cnt++] = BYTE0(car_moton_dir);
	data_to_send[_cnt++] = BYTE1(car_speed);
	data_to_send[_cnt++] = BYTE0(car_speed);
	data_to_send[_cnt++] = BYTE1(turning_radius);
	data_to_send[_cnt++] = BYTE0(turning_radius);
	data_to_send[_cnt++] = BYTE1(_heart_beat_data);
	data_to_send[_cnt++] = BYTE0(_heart_beat_data);

	/* ���ݳ��� */
	data_to_send[4] = _cnt-5;
	
	/* ���У�� */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* ���ݷ��� */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @�� �ܣ������͡�Э������֡--- С��״̬��Ϣ
 * @�� ��������ÿ���յ�һ�ֽ����ݣ�����ú���һ��
 *				 ���ݻ����н���
 * @�� �أ�None
 ***************************************************************
 */
void ECU_SendDataFrameForExecutiveAgency(uint8_t gear_position,           /* ��λ */
																							 uint8_t braking_state,          /* ɲ��״̬ */
																								int16_t throttle_val,          /* ���Ŵ�С */
																								 uint16_t _heart_beat_data)    /* �������� */
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	
	data_to_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_to_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_to_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_to_send[_cnt++] = ECU_SEND_CAR_RAW_DATA;
	data_to_send[_cnt++] = 0;
	
	/* �������� */
	data_to_send[_cnt++] = (uint8_t)gear_position;
	data_to_send[_cnt++] = (uint8_t)braking_state;
	data_to_send[_cnt++] = BYTE1(throttle_val);
	data_to_send[_cnt++] = BYTE0(throttle_val);
	data_to_send[_cnt++] = BYTE1(throttle_val);
	data_to_send[_cnt++] = BYTE0(throttle_val);
	data_to_send[_cnt++] = BYTE1(_heart_beat_data);
	data_to_send[_cnt++] = BYTE0(_heart_beat_data);

	/* ���ݳ��� */
	data_to_send[4] = _cnt-5;
	
	/* ���У�� */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* ���ݷ��� */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @�� �ܣ������͡�Э������֡--- С����Ϣ
 * @�� ���������ϴ�����
 * @�� �أ�None
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
	
	/* �������� */
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

	/* ���ݳ��� */
	data_to_send[4] = _cnt-5;
	
	/* ���У�� */
	for(i=0; i<_cnt; i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
	/* ���ݷ��� */
	Serial_Send_Data(data_to_send, _cnt);
}

/*
 ***************************************************************
 * @��    �ܣ����ͨѶ��·�Ƿ�ɹ�ͨѶ
 * @��    �����������������������ΪͨѶ����
 * @��ڲ�����param_1: ��������
 *            param_2�����������
 * @��    �أ�ͨѶ����-----0x00
 *				    ͨѶ����-----0xff
 ***************************************************************
 */
uint8_t ConnectCheck(int16_t _heart_beat_data, int16_t max_error_times)
{
  int16_t this_time_heart_beat = 0;
	static  int16_t last_time_heart_beat = 0;
	static  int16_t error_times = 0;
	
	/* ��ȡ������������ */
	this_time_heart_beat = _heart_beat_data;
	
	if( this_time_heart_beat == last_time_heart_beat )
	{
		/* ͨѶ����,��¼ */
		error_times++;
	}
	else
	{
		error_times = 0;
	}
	
	/* ������������ */
	last_time_heart_beat = this_time_heart_beat;
	
	/* �������ͨѶ����ֵ,����0xff */
	if( error_times > max_error_times)
	{
		error_times = max_error_times + 1;
		return DP_ERROR;
	}
	
	/* ͨѶ����,����0x00 */
	return DP_NORMAL;
	
}

/*
 ***************************************************************
 * @��    �ܣ�ͨѶ������������
 * @��    ����None
 * @��ڲ�����None
 * @��    �أ�None
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
 * @��    �ܣ��Լ�����
 * @��    ����None
 * @��ڲ�����None
 * @��    �أ�None
 ***************************************************************
 */
void CheckTask(void)
{
		DataProtocolInfo.Statue.connect_statue = ConnectCheck(getRxHearBeatData(), 2);
		/* �Լ���� */
		if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
		{
			ConnectCheckErrorTask();
		}
}
/*
 ***************************************************************
 * @��    �ܣ��������߿����Լ�����
 * @��    ����None
 * @��ڲ�����None
 * @��    �أ�None
 ***************************************************************
 */
void BluetoothRemoteCheckTask(void)
{
	if ( DataProtocolInfo.RxFrameInfo.heart_beat_data != 0 )
	{
	  DataProtocolInfo.Statue.connect_statue = ConnectCheck(getRxHearBeatData(), 2);
		/* �Լ���� */
		if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
		{
			ConnectCheckErrorTask();
		}
	}
}
/*
 ***************************************************************
 * @��    �ܣ�ͨѶЭ���ʼ��
 * @��    ����None
 * @��ڲ�����None
 * @��    �أ�None
 ***************************************************************
 */
void DataProcotolInit(void)
{
	/* ��ʼ���������� */
	DataProtocolInfo.TxFrameInfo.heart_beat_data = 0;
	DataProtocolInfo.RxFrameInfo.heart_beat_data = 0;
}
	
/*
 ***************************************************************
 * @��    �ܣ�ͨѶ���մ�����
 * @��    �����˺�����������ѭ�����棬
 * @��ڲ���������ʱ�䣨ms�� 5ms
 * @��    �أ�
 ***************************************************************
 */
void DataProtocolCommunicationTask (int16_t cycle_time_ms)
{
	/* ͨѶ��� */
	//Serial_Communication_Check(data_from_pc.heart_beat_data, 10);
	static int16_t cycle_times;
	cycle_times++;
	if ( cycle_times > 20000 )
		cycle_times = 0;
	
	/* ÿ1������ѭ����������� */
	if ( 1 )
	{
	}
	
	/* ÿ2������ѭ����������� cycle_time_ms == 5ms--->100hz */
	if ( cycle_times%2 == 0 )
	{
		
		/* �������� */
		if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_REQ_IDENTFIER_TEST )
		{
			/* ��գ�ֻ����һ�� */
			DataProtocolInfo.RxFrameInfo.request_identifier = 0x00;		
			/* ���ز����������� 0xFF */
			ECU_SendAnswerFrameToAnswerPCReqTest();
		}
		
		/* ����һ������		*/
		else if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_ONCE )
		{
			DataProtocolInfo.RxFrameInfo.request_identifier = 0x00;
			ECU_SendDataFrameForCarInfo();
		}
		
		/* 10ms�����ϴ�����		*/
		else if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_TX_EVERY_10MS )
		{
			ECU_SendDataFrameForCarInfo();
		}
		
	}
	
	/* ÿ4������ѭ�����������  cycle_time_ms == 5ms--->50hz  */
	if ( cycle_times%4 == 0 )
	{
		/* add�������� */
		addTxHearBeatData();
		/* ͨѶ�Լ� */
		CheckTask();
		
		/* 20ms�����ϴ����� */
		if ( DataProtocolInfo.RxFrameInfo.request_identifier == PC_FRAME_REQ_DATA_TX_EVERY_20MS )
		{
			ECU_SendDataFrameForCarInfo();
		}
			
	}
}









/*
 ***************************************************************
 * @�� �ܣ�������������Э��
 * @�� �����������ݡ�Ԥ�������󣬽��С�������
 * @�� �أ�None
 ***************************************************************
 */
void Driverless_Data_Receive_Parsed(uint8_t *data_buf, uint8_t num)
{
	uint8_t sum = 0;
	uint8_t i = 0;
	
	/* ���У�� */
	for(i=0; i<(num-1); i++)
		sum += *(data_buf + i);
	/* �ж�sum  У�� */
	if(!(sum==*(data_buf + num - 1)))		
		return;											
	if(!(*(data_buf + 0) == PC_FRAME_HEAD_L && *(data_buf+1) == PC_FRAME_HEAD_H))		
		return;		
	
	/* ��ȡ����֡���� */
	DriverlessDataProtocolInfo.RxFrameInfo.frame_type = *(data_buf + 2);
	/* ��ȡ���ݱ�ʶ�� */
	DriverlessDataProtocolInfo.RxFrameInfo.identifier = *(data_buf + 3);
	/* ��ȡ���ݳ�����Ϣ */
	DriverlessDataProtocolInfo.RxFrameInfo.data_len   = *(data_buf + 4);
	
	/* ���յ�������֡ */
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
 * @�� �ܣ���Ԥ����������Э��
 * @�� ��������ÿ���յ�һ�ֽ����ݣ�����ú���һ��
 *				 ���ݻ����н���
 * @�� �أ�None
 ***************************************************************
 */
void Driverless_DataProtocolPrepareParsed (uint8_t data)
{
	static uint8_t RxBuffer[50];
	static uint8_t _data_len = 0, _data_cnt = 0;
	static uint8_t state = 0;
	 
	if(state == 0 && data == PC_FRAME_HEAD_L)                 /* ֡ͷ-��8λ */
	{
		state = 1;
		RxBuffer[0] = data;
	}
	else if(state == 1 && data == PC_FRAME_HEAD_H)            /* ֡ͷ-��8λ */
	{
		state=2;
		RxBuffer[1] = data;
	}
	else if(state == 2 && data < 0x04)             						 /* ֡���� */          
	{
		state=3;
		RxBuffer[2]=data;
	}
	else if(state == 3 )             												   /* ���ݱ�ʶ�� */          
	{
		state=4;
		RxBuffer[3]=data;
	}
	else if(state == 4 && data < 50)              						 /* ���ݳ���LED */
	{
		state = 5;
		RxBuffer[4] = data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state == 5 && _data_len > 0)            					 /* RxBuffer[4]��ʼ��������  RxBuffer[]=���ݣ����Ⱦ����������ݵĶ��� */
	{
		_data_len--;
		RxBuffer[5 + _data_cnt++] = data;
		if(_data_len == 0)
			state = 6;
	}
	else if(state==6)                                					/* ����  ��������֡���� */
	{
		state = 0;
		RxBuffer[5 + _data_cnt] = data;
		Driverless_Data_Receive_Parsed(RxBuffer, _data_cnt + 6); 				 /* һ֡���ݱ�����ϣ����뺯������ ���ݵı��洦�� */
	}
	else
	{
		state = 0;
	}
}



