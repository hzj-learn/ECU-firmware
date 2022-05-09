/**
  ******************************************************************************
  *
  ******************************************************************************
  ���˼�ʻ��������
  ******************************************************************************
  */ 
#include "autocar.h"
#include "./can/bsp_can.h"
#include "oled.h"
#include "./usart/bsp_debug_usart.h"
#include "./data_protocol/data_protocol.h"
#include "my_rs485.h"
#include "./key/bsp_key.h" 
#include "./led/bsp_led.h" 
#include "DW_contrl.h"  
#include "timer.h"
#include "my_dac.h"
#include "my_sysTick.h"
#include "my_adc.h"
#include "my_brake.h" 
#include <stdlib.h>

extern __IO uint32_t flag ;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
extern CanRxMsg RxMessage;
extern CanTxMsg TxMessage;
extern uint8_t u1,u2,u3,u4,u5,u6,u7;
/*********************************
��ʼ����������

************************************/
void all_init()
{
	/* LED��ʼ�� */
	LED_GPIO_Config();
	/* OLED IIC��ʼ�� */
	oledGpioConfig();
	/* ������ʼ�� */
	//Key_GPIO_Config();        
	/* ��λ��ʼ�� */
	DW_GPIO_Config();         
	/* ��ʼ��CAN���� */
	CAN_Config();    
  /* DAC��ʼ�� */	
	DAC_Mode_Init();          
	MY_DAC_Config();
  /* �δ�ʱ����ʼ����������ʱ */
	SysTick_Init();
  /*  */
	Adc_Init();
  /* RS485��ʼ�� */
	_485_Config();
	/* �������� */
	BluetoothDebugUsartConfig();
	DriverlessUsartConfig();
	/* ���������ʼ�� */
	step_motor_GPIO_Config();
	/*
	ʹ��
	*/
	TIM2_Configuration();    
	BluetoothDebugUsartEnable();
	DriverlessUsartEnable();
	SysTick_Init();
	PID_ParamInit();
	
	
	/* ͨѶЭ���ʼ�� */
	DataProcotolInit();
	TIM_Cmd(TIM3, ENABLE);
	
	
}



/************************************
��can���յ������ݽ��н��봦��

************************************/

uint8_t date[8]={0};
void can_receive_process()
{
	int i;
	uint8_t *a= RxMessage.Data;

	if(flag==1)
		{		
			for(i=0;i<8;i++)
			{
				date[i]=a[i];
			}
		//	CAN_DEBUG_ARRAY(RxMessage.Data,8); 
			
			flag=0;   //���־λ
		}
		
}
/************************************
      �������ݲ�ͨ��can����

************************************/
void can_Transmit_process(uint8_t u5,uint16_t angle)//�����١�Ť�ظ��ֽڡ�Ť�ص��ֽ�
{
     uint8_t TransmitMailbox=0,u9,u10;
	   u9  = (angle&0xFF00)>>8;  //��λ
	   u10 = angle&0x00FF; 			//��λ
	   CAN_SetMsg(&TxMessage, u5, u9, u10);
		 TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	   //while((CAN_TransmitStatus(CAN1,TransmitMailbox))!=CANTXOK);//�ȴ��������
}



/********************************************
            ת��Ƕȿ��ƴ���
 ˵����������ת��15�ȣ��൱�ڳ���ת��1��
********************************************/		
int angle_process(uint16_t N,uint16_t Ideal_angle,uint16_t Actualangle)   // �е�Ƕ�  ���Ƕ�   ʵ�ʴ�ǽǶ�
{
	
	 uint16_t final_out, angle_err=0;
	 angle_err = Ideal_angle-Actualangle; //�Ƕ�ƫ��
	

	
	 if(Ideal_angle>Actualangle)                  //������߻����ұ�
	 {
		 if(abs(angle_err)>=400)final_out=32768-7500; 
		  else if(abs(angle_err)>=350)final_out=32768-8600; 
		  else if(abs(angle_err)>=300)final_out=32768-8700; 
			else if(abs(angle_err)>=250)final_out=32768-8500; 
		  else if(abs(angle_err)>=200)final_out=32768-7400-500; 
			else if(abs(angle_err)>=150)final_out=32768-7300-500;
			else if(abs(angle_err)>=100)final_out=32768-6000-500;
		  else if(abs(angle_err)>=50)final_out=32768-5800-500;
			else if(abs(angle_err)>=35)final_out=32768-5600-500;
			else if(abs(angle_err)>=20)final_out=32768-5500-500;
		  else if(abs(angle_err)>=5)final_out=32768-4400-500;
			else
				{
					//GPIO_SetBits(GPIOH,GPIO_Pin_8); 
					final_out=32768;
				}	
	 }
	 else
	 {
		 if(abs(angle_err)>=400)final_out=32768+7500; 
		  else if(abs(angle_err)>=350)final_out=32768+8600; 
		  else if(abs(angle_err)>=300)final_out=32768+8700; 
			else if(abs(angle_err)>=250)final_out=32768+8500; 
		  else if(abs(angle_err)>=200)final_out=32768+7400+500; 
			else if(abs(angle_err)>=150)final_out=32768+7300+500;
			else if(abs(angle_err)>=100)final_out=32768+5800+500;
			else if(abs(angle_err)>=50)final_out=32768+5700+500;
			else if(abs(angle_err)>=35)final_out=32768+5600+500;
			else if(abs(angle_err)>=20)final_out=32768+5500+500;
		  else if(abs(angle_err)>=5)final_out=32768+4400+500;
				else
				 {
					 //GPIO_SetBits(GPIOH,GPIO_Pin_8); 
					 final_out=32768;
					}		
	 }//����һ�ߴ�
	 /***************�޷�*************************/
	 if(final_out>=41943)final_out=41943;
	 if(final_out<=23593)final_out=23593;
	 if((Actualangle>=5530)||(Actualangle<=4545))
	 {
		// GPIO_SetBits(GPIOH,GPIO_Pin_8);//����������
		 final_out=32768;
	 }
	 /********************************************/		 
	 
	 return final_out;
}	

float angle_Kp=0.2,angle_Ki=0.0,angle_Kd=0.0;
uint16_t ideal_pwm,incrementSpeed,pid_err_last,pid_err_next,incrementangle;
void PID_realize(uint16_t Actualangle,uint16_t Ideal_angle)
{
  uint16_t pid_Actualangle,pid_Setangle,pid_err;
  pid_Actualangle = Actualangle;
  pid_Setangle = Ideal_angle;
  pid_err = pid_Setangle - pid_Actualangle ;
 
  incrementangle = angle_Kp*(pid_err - pid_err_next)+angle_Ki*pid_err+ angle_Kd*(pid_err-(2*pid_err_next)+pid_err_last);
  ideal_pwm += incrementangle; 
  pid_err_last=pid_err_next;
  pid_err_next = pid_err;
	if(ideal_pwm>=41943)ideal_pwm = 41943;
  if(ideal_pwm<=23593)ideal_pwm = 23593;
	throttle_change(ideal_pwm);
}

void simple_speed(uint8_t Actualspeed,uint8_t Ideal_speed)
{
	uint8_t pid_Actualspeed,pid_Setspeed;
	int pid_err;
  pid_Actualspeed = Actualspeed;
  pid_Setspeed = Ideal_speed;
  pid_err = pid_Setspeed - pid_Actualspeed ;
	
	if(pid_err>=0)
	{
		if (abs(pid_err)>=5)   throttle_change(1.83);
		else if ( abs(pid_err)>= 2) throttle_change(1.35);
		else
		throttle_change(0.83);
		
	}
	else
	{
		if ( abs(pid_err)>= 2) 
			throttle_change(0.53);
		else
			throttle_change(0.63);
		
			
	}
}

























/**************************END OF FILE************************************/
