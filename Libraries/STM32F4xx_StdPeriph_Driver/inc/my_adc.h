#ifndef __MY_ADC_H
#define __MY_ADC_H
#include "stm32f4xx.h"

void Get_AD_value(u8 num);

int AD_data_filter(u16 *Var,u8 num);

void  Adc_Init(void);

int Get_Adc(u8 ch);  

























#endif

