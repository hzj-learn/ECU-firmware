#include "control_task.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "my_sysTick.h"
#include "./data_protocol/data_protocol.h"
#include "./oled/oled.h"
#include "autocar.h"
#include "my_dac.h"
#include "DW_contrl.h"   
#include "stm32f4xx_usart.h"
#include "my_brake.h"   
#include "pid.h"
#include "timer.h"
#include "stm32f4xx.h"
#include "my_dac.h"
#include "stm32f4xx_dac.h"

TimerFlag_t TimerFlag;
GlobalVal_t GlobalVal;

/*
 ***************************************************************
 * @��    �ܣ����߿�������
 * @��    ����
 * @��ڲ�����
 * @��    �أ�None
 ***************************************************************
 */
 
 /*  
																�������ݱ�
		CarCtlDataFromWireless.
    
   ����ģʽ��running_mode����
														 0x30 --- ֹͣ
														 0x31 --- �û�ң��ģʽ
						 
	 ��λ(gear_position)��
														 0x01 --- �յ�
														 0x02 --- ǰ����
														 0x03 --- ���˵�
					
	 ת��steer_dir����
	
														 0x20 --- ����	
														 0x21 --- ת��
														 0x22 --- ת��
														 
  ���ţ�throttle_val����
														 ��Χ��0-100 
														 
	ת��Ť�أ�torque_val����
														 ��Χ��0-100
 
  
 
 */
void RemoteCtlTask(void)
{
	  /* ģʽ�л� ******************************************************/
	
	  /* �Զ�ģʽ */
	  if ( CarCtlDataFromWireless.running_mode == 0x31 )
		{
			model_change(1);
		/* ת�� --- �ջ� **************************************************/
		/* ����ת�����ֵ */
		CarCtlDataFromWireless.real_steer_angle = (CarCtlDataFromWireless.raw_steer_angle - 50)*10;
		}
		/* �ֶ�ģʽ��Ĭ�ϣ� */
		else
		{
			model_change(2);
			CarCtlDataFromWireless.real_steer_angle = 0;
		}
		
		 GlobalVal.set_steer_angle = CarCtlDataFromWireless.real_steer_angle;
		
		/* ��λ�л� ******************************************************/
		
		/* ǰ���� */
		if ( CarCtlDataFromWireless.gear_position == 0x02 )  
		{
			DW_Front();
		}
		/* ���˵�λ */
		else if ( CarCtlDataFromWireless.gear_position == 0x03 )  //����
		{
			DW_Back();
		}
		/* �յ�λ */
		else  //ֹͣ
		{
		  DW_Neutral();
		}
				
		
		/* ����ֵ ****************************************************/
		
		
		/* �ڿյ��²�������ֵ */
		if ( (CarCtlDataFromWireless.gear_position == 0x02) 
			   || (CarCtlDataFromWireless.gear_position == 0x03 ) )
		{
			GlobalVal.set_speed = CarCtlDataFromWireless.throttle_val/10;
			//setThrottleVal(CarCtlDataFromWireless.throttle_val);
		}
		else{
			GlobalVal.set_speed = 0;
		}
		
		/* ɲ����λ */
		if ( CarCtlDataFromWireless.brake_state == 0x01 )
		{
			 posotion_choose(1);
		}
		else if ( CarCtlDataFromWireless.brake_state == 0x02 )
		{
			 posotion_choose(2);
		}
		else if ( CarCtlDataFromWireless.brake_state == 0x03 )
	  {
			 posotion_choose(3);
		}
		else
		{
		   posotion_choose(0);
		}
		
}

void DriverlessMode(void)
{
		    //�Զ�ģʽ
				model_change(1);
			
				/* ǰ���� */
				if ( CarCtlDataFromPC.gear_position == 0x01 )  
				{
					DW_Front();
				}
				/* ���˵�λ */
				else if ( CarCtlDataFromPC.gear_position == 0x02 )  //����
				{
					DW_Back();
				}
				/* �յ�λ */
				else  //ֹͣ
				{
					DW_Neutral();
				}
				
				/* ���� */
				if ( CarCtlDataFromPC.throttle_position == 3 )
				{
					  GlobalVal.set_speed = 3;
				}
				else if ( CarCtlDataFromPC.throttle_position == 5 )
				{
				    GlobalVal.set_speed = 5;
				}
				else if ( CarCtlDataFromPC.throttle_position == 10 )
				{
						GlobalVal.set_speed = 10;
				}
				else
				{
						GlobalVal.set_speed = 0;
				}
		
				
				/* ת��Ƕ� */
				GlobalVal.set_steer_angle = CarCtlDataFromPC.raw_steer_angle - 500;
				
				/* ɲ�� */
				if ( CarCtlDataFromPC.brake_state == 0x01 )
				{
				    posotion_choose(1);
				}
				else if ( CarCtlDataFromPC.brake_state ==0x02 )
				{
				     posotion_choose(2);
				}
				else if ( CarCtlDataFromPC.brake_state ==0x03 )
				{
				     posotion_choose(3);
				}
				else
				{
					   posotion_choose(0);
				}
}
/*
   ���� --- ���������Ҫ��λ
 */

void ControlReset(void)
{
	/* ��յ� */
	DW_Neutral();
	/* �綯ת�򱣳� */
	can_Transmit_process(5, 32768);
	/* ���Ų���� */
	setThrottleVal(0);
}

void ControlTask(void)
{
	static uint16_t time_count = 0;
	/* 5ms once */
	time_count ++;
	if ( time_count > 50000 ) 
		time_count = 0;
	
	
	/* ����Ϊ5ms */
	if ( 0 )
	{
	
	}
	
	/* ����Ϊ10ms */
	if ( time_count%2 == 0 )
	{
		TimerFlag.timer_10ms_flag = FLAG_SET;
	}		
	
	/* ����Ϊ20ms */
	if ( time_count%4 == 0 )
	{
		TimerFlag.timer_20ms_flag = FLAG_SET;
		
		if ( CarCtlDataFromWireless.running_mode == 0x31 )
	  {
			/* �綯ת��PID */
		  /* �Զ�ģʽ��  �Զ� 0x31  �ֶ� 0x30*/
			/* GlobalVal.SteerFeedback.angle��Χ  ��+500 --- ��-500 */
			/* ȡ�� -500 --- +500 */
			PID_Steer.ref = GlobalVal.set_steer_angle;
			//PID_Steer.ref = CarCtlDataFromWireless.real_steer_angle;
			PID_Steer.fdb = -GlobalVal.SteerFeedback.angle;
			PID_Cal(&PID_Steer);
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768 + PID_Steer.pid_out );//�е�Ƕȡ�����
			
		}
		else
	  {
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768);
		}
		
		
		if ( 0&& (CarCtlDataFromPC.raw_steer_angle!=0) )
	  {
			/* �綯ת��PID */
		  /* �Զ�ģʽ��  �Զ� 0x31  �ֶ� 0x30*/
			/* GlobalVal.SteerFeedback.angle��Χ  ��+500 --- ��-500 */
			/* ȡ�� -500 --- +500 */
			PID_Steer.ref = GlobalVal.set_steer_angle;
			//PID_Steer.ref = CarCtlDataFromWireless.real_steer_angle;
			PID_Steer.fdb = -GlobalVal.SteerFeedback.angle;
			PID_Cal(&PID_Steer);
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768 + PID_Steer.pid_out );//�е�Ƕȡ�����
			
		}
		
		
	}	
	
	/* ����Ϊ50ms */
	if ( time_count%10 == 0 )
	{
		TimerFlag.timer_50ms_flag = FLAG_SET;
		
		/* �����Լ����� */
		BluetoothRemoteCheckTask();
	}	


	/* ����Ϊ100ms */
	if ( time_count%20 == 0 )
	{
		TimerFlag.timer_100ms_flag = FLAG_SET;
		
		if ( 0 )
		{
			DriverlessMode();
		}
		else
		{
			if ( DataProtocolInfo.Statue.connect_statue == DP_NORMAL )
			{
				RemoteCtlTask();
			}
			else if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
			{
				ControlReset();
			}
		}
		
	}	
	
	/* ����Ϊ500ms */
	if ( time_count%100 == 0 )
	{
	  TimerFlag.timer_500ms_flag = FLAG_SET;
	}
	
	
	/* ����Ϊ1000ms */
	if ( time_count%200 == 0 )
	{
		TimerFlag.timer_1000ms_flag = FLAG_SET;
		DataProtocolInfo.TxFrameInfo.heart_beat_data ++ ;
	}		
	
}

void sendCarInfoInBluetooth(void)
{
	uint8_t _cnt =0;
	uint8_t i;
	uint8_t sum = 0;
	uint8_t data_send[20];
	uint16_t temp;
	
	data_send[_cnt++] = ECU_FRAME_HEAD_L;
	data_send[_cnt++] = ECU_FRAME_HEAD_H;
	data_send[_cnt++] = ECU_FRAME_TYPE_DATA;
	data_send[_cnt++] = 0x00;
	data_send[_cnt++] = 0;
	
	temp = (uint16_t)PID_Steer.pid_out; 
	
	data_send[_cnt++] = (uint8_t) (GlobalVal.MotorDriverFeedback.speed&0x00ff);
	data_send[_cnt++] = (uint8_t) (GlobalVal.MotorDriverFeedback.speed>>8);
	data_send[_cnt++] = (uint8_t) (GlobalVal.SteerFeedback.angle&0x00ff);
	data_send[_cnt++] = (uint8_t) (GlobalVal.SteerFeedback.angle>>8);
	
//	data_send[_cnt++] = (uint8_t) ((5000-CarCtlDataFromWireless.real_steer_angle)&0x00ff);
//	data_send[_cnt++] = (uint8_t) ((5000-CarCtlDataFromWireless.real_steer_angle)>>8);
	data_send[_cnt++] = (uint8_t) ((temp)&0x00ff);
	data_send[_cnt++] = (uint8_t) ((temp)>>8);
  
	data_send[_cnt++] = 0x03;
	data_send[_cnt++] = 0x04;
	
	data_send[4] = _cnt-5;
	
	for(i=0; i<_cnt; i++)
		sum += data_send[i];
	
	data_send[_cnt++] = sum;
	
  Serial_Send_Data_2(data_send, _cnt);
	
}
