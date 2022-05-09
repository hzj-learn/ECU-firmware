/**
  ******************************************************************************
  *��ʱ�жϵĳ�ʼ������
  ******************************************************************************
  */
  
#include "timer.h"
#include "stm32f4xx.h"
#include "misc.h"
 /**
  * @brief  ͨ�ö�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM2_Mode_Config(void)  //����static��һ����̬�������� ֻ���ڵ�ǰ�����ļ��ڵ��á� ��
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 
  TIM_TimeBaseStructure.TIM_Period 			  = 10000;       
  TIM_TimeBaseStructure.TIM_Prescaler 	  = 42-1;	 
  TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode   =TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	// ������ʱ�������ж�
	TIM_ITConfig(GENERAL_TIM, TIM_IT_Update, ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(GENERAL_TIM, ENABLE);	
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIM2_Configuration(void)
{
	TIM2_Mode_Config();
	TIM2_NVIC_Configuration();	

}



/********************************************************************************************************************
 
                                       ���żӼ��١�������DAC��ʱ�仯������ض�ʱ���⺯��

************************************************************************************************************************/

void DAC_TIM_Init(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure; 
 	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    // �����ж���Ϊ0
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 	
		// ������ռ���ȼ�
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	
	// ����TIMx_CLK,x[6,7] 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 100-1;       //������0.1ms*10000=1s��1s�Ķ�ʱ���  80*0.1ms=8ms  ����Ϊ8ms
	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=90MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	 //0.1ms������
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
	//TIM_Cmd(TIM3, ENABLE);	
	 
}




























































/*********************************************END OF FILE**********************/
