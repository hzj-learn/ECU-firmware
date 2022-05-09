

#include "my_adc.h"	
#include "my_sysTick.h"

/*************************************************************************
*  函数名称：  AD_data_filter
*  功能说明: 均值平滑滤波
*  作者: 曾         
*  日期: 2019.4.10
*  作用: 冒泡，去最大值最小值 中间值取平均
*  
*************************************************************************/

uint16_t VAR_1_value[8]={0},VAR_2_value[8],VAR_3_value[8],VAR_4_value[8];

u8 count_flag_now = 0, count_flag_last = 0; 
u8 count_step = 0,paobu_step=0;
u8 s_staus=0;

int AD_data_filter(uint16_t *Var,uint8_t num)
{
    u8 i,j;
    u16 var[8]={0},useful_var[6]={0},ave_var=0;
    u16 sum_var=0;
		
    Get_AD_value(num); 
    
    for(i=0;i<8;i++)var[i]=*(Var+i); 
    
    for(i=0;i<7;i++)
        for(j=0;j<7-i;j++)
        {
            if(var[j]>var[j+1])
            {
                var[j] += var[j+1];
                var[j+1] = var[j]-var[j+1];
                var[j]  -= var[j+1];
            }
        }
    
    for(i=2;i<6;i++)useful_var[i-1]=var[i]; //中间4次值  去掉两端数值4个比较不稳定的值   曾
    
    for(i=0;i<4;i++)sum_var += useful_var[i];  
    
    ave_var=sum_var/4;
    return ave_var;
}

/*************************************************************************
* 函数名：Get_AD_value

* 作用: 每个通道取n次AD值

*  时间2019.4.11        
*  
*************************************************************************/

 void Get_AD_value(u8 num)
{   
    u8 i;
    switch(num)
    {
        case(0):for(i=0;i<8;i++) 
               {
								 VAR_1_value[i] = Get_Adc(11);   
					       Delay_us(5);
   					     }break;
        case(1):for(i=0;i<8;i++)
               {
			        	 VAR_2_value[i] = Get_Adc(3);  
								// delay_us(1);
								 }break;
        case(2):for(i=0;i<8;i++) 
               { 
							   VAR_3_value[i] = Get_Adc(4);   
							 //  delay_us(1);
							   }break;
        case(3):for(i=0;i<8;i++)  
							{
							  VAR_4_value[i] = Get_Adc(5);  
              // delay_us(1);
							  }break;
        default:break;
    }
}

void Adc_Init(void)
{   
	
	 GPIO_InitTypeDef        GPIO_InitStructure;
    ADC_CommonInitTypeDef   ADC_CommonInitStructure;
    ADC_InitTypeDef         ADC_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
 
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;     
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(GPIOC, &GPIO_InitStructure); 

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE); 
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);

    ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled; 
    ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div4;
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode         = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right; 
    ADC_InitStructure.ADC_NbrOfConversion      = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles ); 
    ADC_Cmd(ADC1, ENABLE);

}		

/**************************************************************************
函数功能：AD采样 
时间：2019.4.10
入口参数：ADC1 的通道值：ch-引脚   曾
   ADC1、ADC2: 0-PA0    5-PA5    10-PC0
               1-PA1    6-PA6    11-PC1
               2-PA2    7-PA7    12-PC2
               3-PA3    8-PB0    13-PC3
               4-PA4    9-PB1    14-PC4                          
 用这两个模块一般就能满足要求了   
返回  值：AD转换结果
**************************************************************************/
 int Get_Adc(u8 ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,采样时间为239.5周期	
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_ClearITPendingBit(ADC1, ADC_IT_OVR); 
	  	//设置指定ADC的规则组通道，一个序列，采样时间

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}



















