/**
  ******************************************************************************
  ��λ���ƣ�����ؿ���
  ******************************************************************************
 
  *
  ******************************************************************************
  */
  
#include "DW_contrl.h"   


void DW_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
 
		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	

		/*��������ģʽΪ���ģʽ*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
  //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*������������Ϊ2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
    
    														   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
  /*
		GPIO_InitStructure.GPIO_Pin = LED3_PIN;	
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);	  
		*/															   
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //������
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //ǰ����
		
	/***********************������***********************************/
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOH, ENABLE); 
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3;	
		GPIO_Init(GPIOH, &GPIO_InitStructure);	
		
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOI, ENABLE); //F429���İ������
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
		GPIO_Init(GPIOI, &GPIO_InitStructure);
		
		RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		
		
		
		
		
		
}
/***************************************************
                     ǰ����
****************************************************/
void DW_Front()
{
  GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //������
	GPIO_SetBits(GPIOA,GPIO_Pin_6);   //ǰ����
}
/***************************************************
                     ���˵�
****************************************************/
void DW_Back()
{
	 GPIO_SetBits(GPIOA,GPIO_Pin_3);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //ǰ����
	
}
/***************************************************
                     �յ�
****************************************************/
void DW_Neutral()
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_3);    //������
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);   //ǰ����
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
