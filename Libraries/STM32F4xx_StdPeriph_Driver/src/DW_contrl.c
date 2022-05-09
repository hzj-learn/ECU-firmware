/**
  ******************************************************************************
  档位控制，等相关控制
  ******************************************************************************
 
  *
  ******************************************************************************
  */
  
#include "DW_contrl.h"   


void DW_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
 
		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	

		/*设置引脚模式为输出模式*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
  //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
    
    														   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
  /*
		GPIO_InitStructure.GPIO_Pin = LED3_PIN;	
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);	  
		*/															   
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //后退线
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //前进线
		
	/***********************蜂鸣器***********************************/
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOH, ENABLE); 
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3;	
		GPIO_Init(GPIOH, &GPIO_InitStructure);	
		
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOI, ENABLE); //F429核心板蜂鸣器
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
		GPIO_Init(GPIOI, &GPIO_InitStructure);
		
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		
		
		
		
		
		
}
/***************************************************
                     前进档
****************************************************/
void DW_Front()
{
  GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //后退线
	GPIO_SetBits(GPIOA,GPIO_Pin_6);   //前进线
}
/***************************************************
                     后退档
****************************************************/
void DW_Back()
{
	 GPIO_SetBits(GPIOA,GPIO_Pin_3);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //前进线
	
}
/***************************************************
                     空档
****************************************************/
void DW_Neutral()
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //后退线
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //前进线
}


void model_change(uint8_t model)
{
	switch (model)
{
	case 1:
	{
		GPIO_SetBits(GPIOH, GPIO_Pin_3);
		GPIO_SetBits(GPIOH, GPIO_Pin_5);
		GPIO_ResetBits(GPIOH, GPIO_Pin_4);
	}
		break;
	case 2:
	{
		GPIO_ResetBits(GPIOH, GPIO_Pin_3);
		GPIO_ResetBits(GPIOH, GPIO_Pin_5);
		GPIO_SetBits(GPIOH, GPIO_Pin_4);
	}
		break;
	default:
		GPIO_SetBits(GPIOH, GPIO_Pin_8);
		break;
}
}


















/*********************************************END OF FILE**********************/
