#include "my_dac.h"


/* �������� ---------------------------------------------------------*/

#define  size  500 

uint16_t DualSine16bit[size];

DAC_InitTypeDef  DAC_InitStructure;

/**
  * @brief  ʹ��DAC��ʱ�ӣ���ʼ��GPIO
  * @param  ��
  * @retval ��
  */
static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//������DAC_InitStructure����Ϊȫ�ֱ�����̬����
//	DAC_InitTypeDef  DAC_InitStructure;

  /* ʹ��GPIOAʱ�� */
  RCC_AHB1PeriphClockCmd(DAC_CH1_GPIO_CLK|DAC_CH2_GPIO_CLK, ENABLE);	//  ʹ��ʱ��
	
	/* ʹ��DACʱ�� */	
  RCC_APB1PeriphClockCmd(DAC_CLK, ENABLE);                            //ʹ��dac��ʱ��
  
  /* DAC��GPIO���ã�ģ������ */
  GPIO_InitStructure.GPIO_Pin =  DAC_CH1_GPIO_PIN;                 //pa4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DAC_CH1_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  DAC_CH2_GPIO_PIN;                 //pa5
  GPIO_Init(DAC_CH2_GPIO_PORT, &GPIO_InitStructure);
	
		  /* ����DAC ͨ��1 */
  DAC_InitStructure.DAC_Trigger = DAC_TRIGGER;						//ʹ��TIM2��Ϊ����Դ
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//��ʹ�ò��η�����
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;	//��ʹ��DAC�������
  DAC_Init(DAC_CH1_CHANNEL, &DAC_InitStructure);                 //��ʼ��  ͨ��1

  /* ����DAC ͨ��2 */
  DAC_Init(DAC_CH2_CHANNEL, &DAC_InitStructure);
  
	/* ����DAC ͨ��1��2 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);                              //ʹ��ͨ��
	
	/* ʹ�� DAC��DMA���� */
  DAC_DMACmd(DAC_Channel_1, ENABLE);

}


/**
  * @brief  ����TIM
  * @param  ��
  * @retval ��
  */
static void DAC_TIM_Config(void)
{
	
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	/* ʹ��TIM4ʱ�ӣ�TIM2CLK Ϊ180M */
  RCC_APB1PeriphClockCmd(DAC_TIM_CLK, ENABLE);
	
  /* TIM2������ʱ������ */
 // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 399;       									//��ʱ���� 400       4����  2�����һ�μ���
  TIM_TimeBaseStructure.TIM_Prescaler = 1799;       							//Ԥ��Ƶ������Ƶ 180M / (0+1) = 180M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    						//ʱ�ӷ�Ƶϵ��
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(DAC_TIM, &TIM_TimeBaseStructure);

  /* ����TIM4����Դ */
  TIM_SelectOutputTrigger(DAC_TIM, TIM_TRGOSource_Update);

	/* ʹ��TIM4 */
 // TIM_Cmd(DAC_TIM, ENABLE);

}

/**
  * @brief  ����DMA
  * @param  ��
  * @retval ��
  */
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* DAC1ʹ��DMA1 ͨ��7 ������5ʱ�� */
	RCC_AHB1PeriphClockCmd(DAC_DMA_CLK, ENABLE);
	
	/* ����DMA2 */
  DMA_InitStructure.DMA_Channel = DAC_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;					//�������ݵ�ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&DualSine16bit ;			//�ڴ����ݵ�ַ DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									//���ݴ��䷽���ڴ�������
  DMA_InitStructure.DMA_BufferSize = size;																	//�����СΪ32�ֽ�	
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//�������ݵ�ַ�̶�	
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//�ڴ����ݵ�ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//������������Ϊ��λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;					//�ڴ���������Ϊ��λ	
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;													//ѷģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;											//��DMAͨ�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DAC_DMA_STREAM, &DMA_InitStructure);
  
  /* ʹ�� DMA_Stream */
 // DMA_Cmd(DAC_DMA_STREAM, ENABLE);
 
}


/**
  * @brief  DAC��ʼ������
  * @param  ��
  * @retval ��
  */
void DAC_Mode_Init(void)
{
	//uint32_t Idx = 0;  

	DAC_Config();
	DAC_TIM_Config();
	DAC_DMA_Config();
	
	/* ������Ҳ������ݣ�˫ͨ���Ҷ���*/
   
}

/**
  * @brief  ��ʼһ��dac���
  * @param  ���żӼ���
  * @retval ��
   
   2019.7.29  ���Կ���  ��
  */
uint16_t throttle_laft=0,throttle=0;
float Kp;
void throttle_change(double V)
{
	uint16_t D_vulue=0,i;
	//TIM_Cmd(TIM3, DISABLE); //�رն�ʱ�ж�
	//throttle_toZero();
	throttle=(uint16_t)(V*(4096/3.3));
	DAC_SetChannel2Data(DAC_Align_12b_R, throttle);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
 //	if(throttle>=throttle_laft)//����
	//D_vulue=throttle-throttle_laft;
	//Kp=(D_vulue/200);
	//TIM_Cmd(TIM3, ENABLE); //��ʼ����

}
void setThrottleVal(uint16_t set_val)
{
	DAC_SetChannel2Data(DAC_Align_12b_R, set_val*4096/100);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
}


/**
  * @brief  ��ʼһ��dac���
  * @param  ���Ź���
  * @retval ��
    2019.7.29  ���Կ���
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
	
	
	RCC_AHB1PeriphClockCmd(DAC_CH1_GPIO_CLK|DAC_CH2_GPIO_CLK, ENABLE);	//  ʹ��ʱ��
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
  RCC_APB1PeriphClockCmd(DAC_CLK, ENABLE);                            //ʹ��dac��ʱ��
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//��ʹ�ò��η�����
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;						//ʹ��TIM2��Ϊ����Դ
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;	//��ʹ��DAC�������
	DAC_Init(DAC_CH1_CHANNEL, &DAC_InitStructure);                 //��ʼ��  ͨ��1

  /* ����DAC ͨ��2 */
  DAC_Init(DAC_CH2_CHANNEL, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
	/*DAC_SetChannel1Data(DAC_Align_12b_R,3100);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);*/
}


























