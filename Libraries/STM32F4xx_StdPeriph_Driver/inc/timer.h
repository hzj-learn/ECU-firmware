
#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx.h"


/************用到的宏定义******************************/
//APB1----45MHz
//APB2----90MHz

#define GENERAL_TIM           		TIM2                     //通用定时器模块号，可更换之。  
#define GENERAL_TIM_CLK       		RCC_APB1Periph_TIM2      //时钟模块 tim2对应的时钟模块

#define GENERAL_TIM_IRQn					TIM2_IRQn                //中断号
#define GENERAL_TIM_IRQHandler    TIM2_IRQHandler          //中断服务函数名  ，中断服务函数所在的地址


void TIM2_Configuration(void); //定时中断初始化

void DAC_TIM_Init(void);
















#endif
