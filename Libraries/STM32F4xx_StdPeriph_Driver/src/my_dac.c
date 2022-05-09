#include "my_dac.h"


/* 波形数据 ---------------------------------------------------------*/

#define  size  500 

uint16_t DualSine16bit[size];

DAC_InitTypeDef  DAC_InitStructure;

/**
  * @brief  使能DAC的时钟，初始化GPIO
  * @param  无
  * @retval 无
  */
static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//经测试DAC_InitStructure必须为全局变量或静态变量
//	DAC_InitTypeDef  DAC_InitStructure;

  /* 使能GPIOA时钟 */
  RCC_AHB1PeriphClockCmd(DAC_CH1_GPIO_CLK|DAC_CH2_GPIO_CLK, ENABLE);	//  使能时钟
	
	/* 使能DAC时钟 */	
  RCC_APB1PeriphClockCmd(DAC_CLK, ENABLE);                            //使能dac的时钟
  
  /* DAC的GPIO配置，模拟输入 */
  GPIO_InitStructure.GPIO_Pin =  DAC_CH1_GPIO_PIN;                 //pa4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DAC_CH1_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  DAC_CH2_GPIO_PIN;                 //pa5
  GPIO_Init(DAC_CH2_GPIO_PORT, &GPIO_InitStructure);
	
		  /* 配置DAC 通道1 */
  DAC_InitStructure.DAC_Trigger = DAC_TRIGGER;						//使用TIM2作为触发源
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//不使用波形发生器
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;	//不使用DAC输出缓冲
  DAC_Init(DAC_CH1_CHANNEL, &DAC_InitStructure);                 //初始化  通道1

  /* 配置DAC 通道2 */
  DAC_Init(DAC_CH2_CHANNEL, &DAC_InitStructure);
  
	/* 配置DAC 通道1、2 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);                              //使能通道
	
	/* 使能 DAC的DMA请求 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);

}


/**
  * @brief  配置TIM
  * @param  无
  * @retval 无
  */
static void DAC_TIM_Config(void)
{
	
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	/* 使能TIM4时钟，TIM2CLK 为180M */
  RCC_APB1PeriphClockCmd(DAC_TIM_CLK, ENABLE);
	
  /* TIM2基本定时器配置 */
 // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 399;       									//定时周期 400       4毫秒  2秒完成一次加速
  TIM_TimeBaseStructure.TIM_Prescaler = 1799;       							//预分频，不分频 180M / (0+1) = 180M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    						//时钟分频系数
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
  TIM_TimeBaseInit(DAC_TIM, &TIM_TimeBaseStructure);

  /* 配置TIM4触发源 */
  TIM_SelectOutputTrigger(DAC_TIM, TIM_TRGOSource_Update);

	/* 使能TIM4 */
 // TIM_Cmd(DAC_TIM, ENABLE);

}

/**
  * @brief  配置DMA
  * @param  无
  * @retval 无
  */
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* DAC1使用DMA1 通道7 数据流5时钟 */
	RCC_AHB1PeriphClockCmd(DAC_DMA_CLK, ENABLE);
	
	/* 配置DMA2 */
  DMA_InitStructure.DMA_Channel = DAC_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;					//外设数据地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&DualSine16bit ;			//内存数据地址 DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									//数据传输方向内存至外设
  DMA_InitStructure.DMA_BufferSize = size;																	//缓存大小为32字节	
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设数据地址固定	
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//内存数据地址自增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据以字为单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;					//内存数据以字为单位	
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;													//逊模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;											//高DMA通道优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DAC_DMA_STREAM, &DMA_InitStructure);
  
  /* 使能 DMA_Stream */
 // DMA_Cmd(DAC_DMA_STREAM, ENABLE);
 
}


/**
  * @brief  DAC初始化函数
  * @param  无
  * @retval 无
  */
void DAC_Mode_Init(void)
{
	//uint32_t Idx = 0;  

	DAC_Config();
	DAC_TIM_Config();
	DAC_DMA_Config();
	
	/* 填充正弦波形数据，双通道右对齐*/
   
}

/**
  * @brief  开始一次dac输出
  * @param  油门加减速
  * @retval 无
   
   2019.7.29  测试可用  曾
  */
uint16_t throttle_laft=0,throttle=0;
float Kp;
void throttle_change(double V)
{
	uint16_t D_vulue=0,i;
	//TIM_Cmd(TIM3, DISABLE); //关闭定时中断
	//throttle_toZero();
	throttle=(uint16_t)(V*(4096/3.3));
	DAC_SetChannel2Data(DAC_Align_12b_R, throttle);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
 //	if(throttle>=throttle_laft)//加速
	//D_vulue=throttle-throttle_laft;
	//Kp=(D_vulue/200);
	//TIM_Cmd(TIM3, ENABLE); //开始变速

}
void setThrottleVal(uint16_t set_val)
{
	DAC_SetChannel2Data(DAC_Align_12b_R, set_val*4096/100);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
}


/**
  * @brief  开始一次dac输出
  * @param  油门归零
  * @retval 无
    2019.7.29  测试可用
  */

void throttle_toZero(void)
{
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
	throttle_laft=0;
}


void DAC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(DAC_CH1_GPIO_CLK|DAC_CH2_GPIO_CLK, ENABLE);	//  使能时钟
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin =  DAC_CH1_GPIO_PIN|DAC_CH2_GPIO_PIN;                 //pa4
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(DAC_CH1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_Init(DAC_CH2_GPIO_PORT, &GPIO_InitStructure);
}


void MY_DAC_Config(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	DAC_GPIO_Config();
  RCC_APB1PeriphClockCmd(DAC_CLK, ENABLE);                            //使能dac的时钟
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//不使用波形发生器
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;						//使用TIM2作为触发源
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;	//不使用DAC输出缓冲
	DAC_Init(DAC_CH1_CHANNEL, &DAC_InitStructure);                 //初始化  通道1

  /* 配置DAC 通道2 */
  DAC_Init(DAC_CH2_CHANNEL, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
	/*DAC_SetChannel1Data(DAC_Align_12b_R,3100);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);*/
}


























