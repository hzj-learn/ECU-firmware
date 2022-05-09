/**
  ******************************************************************************
  * @file    main.c
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
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
#include "control_task.h"
#include "my_brake.h" 
#define R_X_POS  70

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
uint8_t ii=0,u1=255,u2,u3,u4,u5,u6,u7;
uint16_t final_out;
uint8_t speed=0,Gear_position=0;
extern u16 VAR_1_value[8];
double GG=0.0;
int angle_supaiqi;
extern uint8_t RESET_flag;
int main(void)
{
	uint8_t led_flag = 0x00;
  all_init();
	
  Delay_ms(500);
	
	/* OLED初始化显示 */
	oledDisplayInit();
	oled_clear();
	
  while(1)
	{
		/* 周期为100ms的任务 *********************************************************/
		if ( TimerFlag.timer_100ms_flag == FLAG_SET )
		{
		  TimerFlag.timer_100ms_flag = FLAG_RESET;
			
			/* LED闪烁任务 */
	    led_flag = ~led_flag;
			if ( led_flag )  { GPIO_SetBits(GPIOH,GPIO_Pin_7); }
			else             { GPIO_ResetBits(GPIOH,GPIO_Pin_7); }
			
			
			
			/* 发送数据 --- 通讯正常时才发送数据 */
			if ( DataProtocolInfo.Statue.connect_statue == DP_NORMAL )
				sendCarInfoInBluetooth();
		}
		
		
		/* 周期为500ms的任务 *********************************************************/
		if ( TimerFlag.timer_500ms_flag == FLAG_SET )
		{
		  TimerFlag.timer_500ms_flag = FLAG_RESET;
			
			/* 自检出错报警 */
			if ( DataProtocolInfo.Statue.connect_statue == DP_ERROR )
			{
				/* OLED显示报警 */
				oled_showString(40,5,"DP ERROR",6,12);
				/* 蜂鸣器长鸣报警 */
				GPIO_SetBits(GPIOH, GPIO_Pin_8);
			}
			else
			{
				oled_showString(40,5,"CAR DATA",6,12);
				GPIO_ResetBits(GPIOH, GPIO_Pin_8);
			}
			
//			oled_showString(0,20,"gear:",6,8);
//			oled_showRealNum(30, 20, CarCtlDataFromPC.gear_position, 4, 6, 8);
//			oled_showString(0,30,"thro:",6,8);
//			oled_showRealNum(30, 30, CarCtlDataFromPC.throttle_position, 4, 6, 8);
//			oled_showString(0,40,"stee:",6,8);
//			oled_showRealNum(30, 40, CarCtlDataFromPC.raw_steer_angle, 4, 6, 8);
//			oled_showString(0,50,"brak:",6,8);
//			oled_showRealNum(30, 50, CarCtlDataFromPC.brake_state, 4, 6, 8);
//			
//			oled_showString(0 + R_X_POS,20,"H__B:",6,8);
//			oled_showRealNum(30 + R_X_POS, 20, CarCtlDataFromPC.heart_beat_data, 4, 6, 8);
//			oled_showString(0 + R_X_POS,30,"torq:",6,8);
//			oled_showRealNum(30 + R_X_POS, 30, CarCtlDataFromWireless.torque_val, 4, 6, 8);
//			oled_showString(0 + R_X_POS,40,"hear:",6,8);
//			oled_showRealNum(30 + R_X_POS, 40, CarCtlDataFromWireless.heart_beat_data, 4, 6, 8);
//			oled_showString(0 + R_X_POS,50,"DFB:",6,8);
//			oled_showRealNum(30 + R_X_POS-10, 50, GlobalVal.MotorDriverFeedback.speed, 5, 6, 8);
			
			/* 更新OLED任务 */
			/*
			oled_showString(0,20,"mode:",6,8);
			oled_showRealNum(30, 20, CarCtlDataFromWireless.running_mode, 4, 6, 8);
			oled_showString(0,30,"stat:",6,8);
			oled_showRealNum(30, 30, CarCtlDataFromWireless.running_statue, 4, 6, 8);
			oled_showString(0,40,"gear:",6,8);
			oled_showRealNum(30, 40, CarCtlDataFromWireless.gear_position, 4, 6, 8);
			oled_showString(0,50,"stee:",6,8);
			oled_showRealNum(30, 50, CarCtlDataFromWireless.brake_state, 4, 6, 8);
			
			oled_showString(0 + R_X_POS,20,"thro:",6,8);
			oled_showRealNum(30 + R_X_POS, 20, CarCtlDataFromWireless.throttle_val, 4, 6, 8);
			oled_showString(0 + R_X_POS,30,"torq:",6,8);
			oled_showRealNum(30 + R_X_POS, 30, CarCtlDataFromWireless.torque_val, 4, 6, 8);
			oled_showString(0 + R_X_POS,40,"hear:",6,8);
			oled_showRealNum(30 + R_X_POS, 40, CarCtlDataFromWireless.heart_beat_data, 4, 6, 8);
			oled_showString(0 + R_X_POS,50,"DFB:",6,8);
			oled_showRealNum(30 + R_X_POS-10, 50, GlobalVal.MotorDriverFeedback.speed, 5, 6, 8);
			*/
			/* 更新 */
			//oled_refreshGram();
		}
		
		oled_showString(0,20,"gear:",6,8);
			oled_showRealNum(30, 20, CarCtlDataFromPC.gear_position, 4, 6, 8);
			oled_showString(0,30,"thro:",6,8);
			oled_showRealNum(30, 30, CarCtlDataFromPC.throttle_position, 4, 6, 8);
			oled_showString(0,40,"stee:",6,8);
			oled_showRealNum(30, 40, CarCtlDataFromPC.raw_steer_angle, 4, 6, 8);
			oled_showString(0,50,"brak:",6,8);
			oled_showRealNum(30, 50, CarCtlDataFromPC.brake_state, 4, 6, 8);
			
			oled_showString(0 + R_X_POS,20,"H__B:",6,8);
			oled_showRealNum(30 + R_X_POS, 20, CarCtlDataFromPC.heart_beat_data, 4, 6, 8);
		oled_refreshGram();
	  Delay_ms(10);
			
	 
	}

}
//void main()
//{
//	step_motor_GPIO_Config();
//	SysTick_Init();
//	while(1)
//	{
//		Delay_ms(2000);
//	  posotion_choose(1);
//		Delay_ms(3000);
//		posotion_choose(2);
//		Delay_ms(3000);
//	  posotion_choose(3);
//		Delay_ms(5000);
//		posotion_choose(0);
//	}
//}



/*********************************************END OF FILE**********************/

