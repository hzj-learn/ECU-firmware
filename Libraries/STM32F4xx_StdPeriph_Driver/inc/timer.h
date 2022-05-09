
#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx.h"


/************�õ��ĺ궨��******************************/
//APB1----45MHz
//APB2----90MHz

#define GENERAL_TIM           		TIM2                     //ͨ�ö�ʱ��ģ��ţ��ɸ���֮��  
#define GENERAL_TIM_CLK       		RCC_APB1Periph_TIM2      //ʱ��ģ�� tim2��Ӧ��ʱ��ģ��

#define GENERAL_TIM_IRQn					TIM2_IRQn                //�жϺ�
#define GENERAL_TIM_IRQHandler    TIM2_IRQHandler          //�жϷ�������  ���жϷ��������ڵĵ�ַ


void TIM2_Configuration(void); //��ʱ�жϳ�ʼ��

void DAC_TIM_Init(void);
















#endif
