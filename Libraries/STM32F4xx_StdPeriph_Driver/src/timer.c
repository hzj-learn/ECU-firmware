/**
  ******************************************************************************
  *定时中断的初始化函数
  ******************************************************************************
  */
  
#include "timer.h"
#include "stm32f4xx.h"
#include "misc.h"
 /**
  * @brief  通用定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM2_Mode_Config(void)  //定义static是一个静态变量定义 只能在当前定义文件内调用。 曾
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 
  TIM_TimeBaseStructure.TIM_Period 			  = 10000;       
  TIM_TimeBaseStructure.TIM_Prescaler 	  = 42-1;	 
  TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode   =TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	// 开启定时器更新中断
	TIM_ITConfig(GENERAL_TIM, TIM_IT_Update, ENABLE);
	
	// 使能定时器
	TIM_Cmd(GENERAL_TIM, ENABLE);	
}

/**
  * @brief  初始化高级控制定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIM2_Configuration(void)
{
	TIM2_Mode_Config();
	TIM2_NVIC_Configuration();	

}



/********************************************************************************************************************
 
                                       油门加减速————DAC定时变化。。相关定时器库函数

************************************************************************************************************************/

void DAC_TIM_Init(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure; 
 	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    // 设置中断组为0
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 	
		// 设置抢占优先级
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	
	// 开启TIMx_CLK,x[6,7] 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 100-1;       //计算器0.1ms*10000=1s。1s的定时间隔  80*0.1ms=8ms  周期为8ms
	
	// 高级控制定时器时钟源TIMxCLK = HCLK/2=90MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;	 //0.1ms的周期
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	//TIM_Cmd(TIM3, ENABLE);	
	 
}




























































/*********************************************END OF FILE**********************/
