/**
  ******************************************************************************
   各种中断服务函数------曾
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./can/bsp_can.h"
#include "timer.h"
#include "./led/bsp_led.h" 
#include "my_rs485.h"
#include "control_task.h"
#include "autocar.h"
#include "./data_protocol/data_protocol.h"
#include "my_sysTick.h"
#include "my_brake.h"  
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void TimingDelay_Decrement(void);
uint32_t step_count=0;
uint32_t step_count_now=0;
uint8_t free_position_flag=0;
uint8_t one_position_flag=0;
uint8_t two_position_flag=0;
uint8_t three_position_flag=0;
uint8_t RESET_flag=1;
uint32_t step_speed=35;//(范围在30到100)//越小越快
#define ZERO_CLASS       0
#define FIRST_CLASS      2000 
#define SECOND_CLASS     3000
#define THREE_CLASS      3500 //不能超5500	
	
void SysTick_Handler(void)
{
	//节拍函数
	TimingDelay_Decrement();
	
	//刹车步进电机程序
	#if 1
	step_count++;
	if(step_count==step_speed)//100hz,调速用的
 {
	 step_count=0;
//////////////////////////////////////////////////////////////////////////	
   if(RESET_flag==1)//复位	
	 {
		 	PWM_TOGGLE;
			if(GPIO_ReadInputDataBit(KEY_GPIO_PORT,KEY_PIN) == 1) //碰到了限位开关了
			{
				RESET_flag=0;
				EN_ON;
			}		
	 }
///////////////////////////////////////////////////////////////////////////	 
	 if(free_position_flag==1&&RESET_flag==0)//在空档 step_count_now=0
	 {
		 if(step_count_now>ZERO_CLASS)//从一档或者二挡三挡转空挡
		 {
			 DIR_ON;//电机反转
			 PWM_TOGGLE;
			 step_count_now--;
		 }
		  if(step_count_now==ZERO_CLASS)//到达了空挡
			{
				EN_ON;
		    free_position_flag=0;
			}
		 
	 }
////////////////////////////////////////////////////////////////////////	 
	 if(one_position_flag==1&&RESET_flag==0)//在一档  step_count_now=1000
	 {
		 if(step_count_now<FIRST_CLASS)//空挡转一档
		 {
			 DIR_OFF;//电机正转
			 PWM_TOGGLE;
			 step_count_now++;
		 }
		 else if(step_count_now>FIRST_CLASS)//二挡三挡转一档
		 {
			  DIR_ON;//电机反转
			 PWM_TOGGLE;
			 step_count_now--;
		 }
		 	if(step_count_now==FIRST_CLASS)
		 {
			 	EN_ON;
		    one_position_flag=0;
		 }
     		 
	 }
////////////////////////////////////////////////////////////////////////
	 if(two_position_flag==1&&RESET_flag==0)//在二挡 step_count_now=2000
	 {
		  if(step_count_now<SECOND_CLASS)//从一档或者空挡转二挡
			{
		  DIR_OFF;//电机正转
		 	PWM_TOGGLE;
		  step_count_now++;
			}
			else if(step_count_now>SECOND_CLASS)//三挡转二档
		 {
			  DIR_ON;//电机反转
			 PWM_TOGGLE;
			 step_count_now--;
		 }
		  if(step_count_now==SECOND_CLASS)
			{
				EN_ON;
		    two_position_flag=0;
			}
	 }
////////////////////////////////////////////////////////////////////////////////////	 
	 if(three_position_flag==1&&RESET_flag==0)//在三挡 step_count_now=3000
	 {
		  if(step_count_now<THREE_CLASS)//从一档或者空挡转二挡
			{
		  DIR_OFF;//电机正转
		 	PWM_TOGGLE;
		  step_count_now++;
			}
		  if(step_count_now==THREE_CLASS)
			{
				EN_ON;
		    three_position_flag=0;
			}			
	 }
 }
	
	#endif
	
}


extern __IO uint32_t flag ;		 //用于标志是否接收到数据，在中断函数中赋值
extern CanRxMsg RxMessage;				 //接收缓冲区
extern uint8_t speed,Gear_position;
extern uint8_t date[8];
	
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
uint16_t set_speed;
void CAN_RX_IRQHandler(void)
{
	/*从邮箱中读出报文*/
//	CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
   
  if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) 
	{
		    Init_RxMes(&RxMessage);                     //把原本的数据清除掉
        CAN_ClearITPendingBit( CAN1, CAN_IT_FMP0 ); //清标志
		
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);   //读取数据并清除FIFO中的数据
		    can_receive_process();                      //数据处理
		    //第一个字节  0xff
		    //第二个字节  0x55
		    //第三个字节  0x55
		    //第四个字节  故障信息
		    //第五个字节  心跳位
		    //第六个字节  传感器扭矩高位
		    //第七个字节  传感器扭矩低位
		    //第八个字节  前七个字节的异或校验值
		     
  }
	/* 比较ID是否为0x1314 */ 
	if((RxMessage.ExtId==0x18F50505) && (RxMessage.IDE==CAN_ID_EXT) && (RxMessage.DLC==8) )  //EPS的数据帧
	{ 
		can_receive_process();    
		

//		GPIO_SetBits(GPIOH,GPIO_Pin_8);  //蜂鸣器
//		GPIO_ResetBits(GPIOI,GPIO_Pin_11); 
//		GPIO_SetBits(GPIOH,GPIO_Pin_7);   //LED
//		LED_RGBOFF;
//	flag = 1; 					       //接收成功  
	}
	/*else
	{
	flag = 0; 					   //接收失败
	}*/
	 if((RxMessage.ExtId==0x10F8109A) && (RxMessage.IDE==CAN_ID_EXT) && (RxMessage.DLC==8) )  //后轮驱动发数据帧
	{
	//	can_receive_process();    
		//speed=(uint8_t)((date[2]*0xFF+date[1])/100);
		GlobalVal.MotorDriverFeedback.gear_position = RxMessage.Data[0];
		GlobalVal.MotorDriverFeedback.raw_speed = (uint16_t)(RxMessage.Data[2]<<8)|RxMessage.Data[1];
		GlobalVal.MotorDriverFeedback.speed = GlobalVal.MotorDriverFeedback.raw_speed/100;
		
		//Gear_position=RxMessage.Data[0];//date[0];//空挡：8，前进挡：9 ，后退档：10
	//	PID_realize(speed,5.0);//实际速度、期望速度
		simple_speed(GlobalVal.MotorDriverFeedback.speed, GlobalVal.set_speed);
		//Gear_position=date[0];//空挡：8，前进挡：9 ，后退档：10
//		GPIO_ResetBits(GPIOH,GPIO_Pin_8); 
//		GPIO_SetBits(GPIOI,GPIO_Pin_11); 
//		GPIO_ResetBits(GPIOH,GPIO_Pin_7);
		//LED_RED;
	 // flag = 2; 					       //接收成功  
	}
	//flag=0;
/*	else
	{
	flag = 0; 					   //接收失败
	}*/
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

	
	/**
  定时中断服务函数 ，用于定时发送数据，是can的发送周期小于5ms
  */	

extern uint8_t u1,u2,u3,u4,u5,u6,u7;
extern  CanTxMsg TxMessage;			     //发送缓冲区
extern uint16_t final_out;

void  GENERAL_TIM_IRQHandler (void)
{
	
	
	if ( TIM_GetITStatus( GENERAL_TIM, TIM_IT_Update) != RESET ) //检查Tim更新中断发生与否
	{	
		TIM_ClearITPendingBit(GENERAL_TIM , TIM_IT_Update);  		   //清除Tim更新中断标志
		/*********************相应动作************************************/
		 ControlTask();
	
		/******************************************************************/
	}		 	
}

/**************************************************

        中断服务函数------DAC定时变化

***************************************************/
extern uint16_t throttle_laft,throttle;
extern float Kp;
uint16_t iii=0;	
void TIM3_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET ) //检查Tim更新中断发生与否
	{	
		TIM_ClearITPendingBit(TIM3 , TIM_IT_Update);  		   //清除Tim更新中断标志
		/*********************相应动作************************************/
	
		/******************************************************************/
	}		
}
extern uint16_t angle_supaiqi;
extern uint8_t tranmit_flag;
void _485_IRQHandler(void)
{
	  //GPIO_SetBits(GPIOH,GPIO_Pin_8); 
    bsp_485_IRQHandler();  //得到编码器数组
	  if(tranmit_flag==1)
		{
			tranmit_flag=0;
			angle_supaiqi=(uint16_t)get_angle();//获取角度   //最大360*16=5760度
			
			GlobalVal.SteerFeedback.angle = angle_supaiqi-5008;
	   
//			if ( CarCtlDataFromWireless.running_mode == 0x31 )
//			{
//				can_Transmit_process(GlobalVal.MotorDriverFeedback.raw_speed, 
//													angle_process(4988, 5008-CarCtlDataFromWireless.real_steer_angle,angle_supaiqi ));//中点角度、期望
//			}
//			else
//			{
//			  can_Transmit_process(GlobalVal.MotorDriverFeedback.raw_speed, 32768);
//			}
				//	GPIO_ResetBits(GPIOH,GPIO_Pin_8); 
		}
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
