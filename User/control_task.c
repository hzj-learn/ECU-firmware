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
 * @功    能：无线控制任务
 * @描    述：
 * @入口参数：
 * @返    回：None
 ***************************************************************
 */
 
 /*  
																控制数据表
		CarCtlDataFromWireless.
    
   运行模式（running_mode）：
														 0x30 --- 停止
														 0x31 --- 用户遥控模式
						 
	 挡位(gear_position)：
														 0x01 --- 空挡
														 0x02 --- 前进挡
														 0x03 --- 后退档
					
	 转向（steer_dir）；
	
														 0x20 --- 保持	
														 0x21 --- 转左
														 0x22 --- 转右
														 
  油门（throttle_val）：
														 范围：0-100 
														 
	转向扭矩（torque_val）：
														 范围：0-100
 
  
 
 */
void RemoteCtlTask(void)
{
	  /* 模式切换 ******************************************************/
	
	  /* 自动模式 */
	  if ( CarCtlDataFromWireless.running_mode == 0x31 )
		{
			model_change(1);
		/* 转向 --- 闭环 **************************************************/
		/* 计算转向给定值 */
		CarCtlDataFromWireless.real_steer_angle = (CarCtlDataFromWireless.raw_steer_angle - 50)*10;
		}
		/* 手动模式（默认） */
		else
		{
			model_change(2);
			CarCtlDataFromWireless.real_steer_angle = 0;
		}
		
		 GlobalVal.set_steer_angle = CarCtlDataFromWireless.real_steer_angle;
		
		/* 挡位切换 ******************************************************/
		
		/* 前进挡 */
		if ( CarCtlDataFromWireless.gear_position == 0x02 )  
		{
			DW_Front();
		}
		/* 后退挡位 */
		else if ( CarCtlDataFromWireless.gear_position == 0x03 )  //后退
		{
			DW_Back();
		}
		/* 空档位 */
		else  //停止
		{
		  DW_Neutral();
		}
				
		
		/* 油门值 ****************************************************/
		
		
		/* 在空挡下不给油门值 */
		if ( (CarCtlDataFromWireless.gear_position == 0x02) 
			   || (CarCtlDataFromWireless.gear_position == 0x03 ) )
		{
			GlobalVal.set_speed = CarCtlDataFromWireless.throttle_val/10;
			//setThrottleVal(CarCtlDataFromWireless.throttle_val);
		}
		else{
			GlobalVal.set_speed = 0;
		}
		
		/* 刹车档位 */
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
		    //自动模式
				model_change(1);
			
				/* 前进挡 */
				if ( CarCtlDataFromPC.gear_position == 0x01 )  
				{
					DW_Front();
				}
				/* 后退挡位 */
				else if ( CarCtlDataFromPC.gear_position == 0x02 )  //后退
				{
					DW_Back();
				}
				/* 空档位 */
				else  //停止
				{
					DW_Neutral();
				}
				
				/* 油门 */
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
		
				
				/* 转向角度 */
				GlobalVal.set_steer_angle = CarCtlDataFromPC.raw_steer_angle - 500;
				
				/* 刹车 */
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
   报警 --- 所有输出需要复位
 */

void ControlReset(void)
{
	/* 打空档 */
	DW_Neutral();
	/* 电动转向保持 */
	can_Transmit_process(5, 32768);
	/* 油门不输出 */
	setThrottleVal(0);
}

void ControlTask(void)
{
	static uint16_t time_count = 0;
	/* 5ms once */
	time_count ++;
	if ( time_count > 50000 ) 
		time_count = 0;
	
	
	/* 周期为5ms */
	if ( 0 )
	{
	
	}
	
	/* 周期为10ms */
	if ( time_count%2 == 0 )
	{
		TimerFlag.timer_10ms_flag = FLAG_SET;
	}		
	
	/* 周期为20ms */
	if ( time_count%4 == 0 )
	{
		TimerFlag.timer_20ms_flag = FLAG_SET;
		
		if ( CarCtlDataFromWireless.running_mode == 0x31 )
	  {
			/* 电动转向PID */
		  /* 自动模式下  自动 0x31  手动 0x30*/
			/* GlobalVal.SteerFeedback.angle范围  左+500 --- 右-500 */
			/* 取反 -500 --- +500 */
			PID_Steer.ref = GlobalVal.set_steer_angle;
			//PID_Steer.ref = CarCtlDataFromWireless.real_steer_angle;
			PID_Steer.fdb = -GlobalVal.SteerFeedback.angle;
			PID_Cal(&PID_Steer);
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768 + PID_Steer.pid_out );//中点角度、期望
			
		}
		else
	  {
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768);
		}
		
		
		if ( 0&& (CarCtlDataFromPC.raw_steer_angle!=0) )
	  {
			/* 电动转向PID */
		  /* 自动模式下  自动 0x31  手动 0x30*/
			/* GlobalVal.SteerFeedback.angle范围  左+500 --- 右-500 */
			/* 取反 -500 --- +500 */
			PID_Steer.ref = GlobalVal.set_steer_angle;
			//PID_Steer.ref = CarCtlDataFromWireless.real_steer_angle;
			PID_Steer.fdb = -GlobalVal.SteerFeedback.angle;
			PID_Cal(&PID_Steer);
			can_Transmit_process(GlobalVal.MotorDriverFeedback.speed, 32768 + PID_Steer.pid_out );//中点角度、期望
			
		}
		
		
	}	
	
	/* 周期为50ms */
	if ( time_count%10 == 0 )
	{
		TimerFlag.timer_50ms_flag = FLAG_SET;
		
		/* 无线自检任务 */
		BluetoothRemoteCheckTask();
	}	


	/* 周期为100ms */
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
	
	/* 周期为500ms */
	if ( time_count%100 == 0 )
	{
	  TimerFlag.timer_500ms_flag = FLAG_SET;
	}
	
	
	/* 周期为1000ms */
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
