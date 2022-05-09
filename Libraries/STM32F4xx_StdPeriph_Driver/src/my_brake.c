#include "my_brake.h"   
#include "my_sysTick.h"
extern uint8_t free_position_flag;
extern uint8_t one_position_flag;
extern uint8_t two_position_flag;
extern uint8_t three_position_flag;
extern uint8_t RESET_flag;

#define FREE_pos 0
#define ONE_pos 1
#define TWO_pos 2 
#define THREE_pos 3 

//posotion=1即是一档
//调用posotion_choose(FREE_pos)
void posotion_choose(uint8_t posotion)
{
	EN_OFF;//步进电机使能
	if(posotion==FREE_pos)//空挡
	{
		free_position_flag=1;
		one_position_flag=0;
		two_position_flag=0;	
	  three_position_flag=0;		
	}
	if(posotion==ONE_pos)//一档
	{
		free_position_flag=0;
		one_position_flag=1;
		two_position_flag=0;	
	  three_position_flag=0;		
	}
	if(posotion==TWO_pos)//二挡
	{		
		free_position_flag=0;
		one_position_flag=0;
		two_position_flag=1;	
	  three_position_flag=0;		
	}
	if(posotion==THREE_pos)//三挡
	{
		free_position_flag=0;
		one_position_flag=0;
		two_position_flag=0;
		three_position_flag=1;
	}
}



void step_motor_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd (PWM_GPIO_CLK
														|DIR_GPIO_CLK
														|EN_GPIO_CLK
														|KEY_GPIO_CLK, ENABLE); 														   
		GPIO_InitStructure.GPIO_Pin    = PWM_PIN;	
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_2MHz; 
		GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = DIR_PIN;	
    GPIO_Init(DIR_GPIO_PORT, &GPIO_InitStructure);	
/////////////////////////////////////////////////////////////////////////////////	
		GPIO_InitStructure.GPIO_Pin = EN_PIN;	
    GPIO_Init(EN_GPIO_PORT, &GPIO_InitStructure);
/////////////////////////////////////////////////////////////////////////////////	
		GPIO_InitStructure.GPIO_Pin = KEY_PIN;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
 
		//设置状态
		PWM_OFF;	
    DIR_ON;		
	  EN_OFF;
}
void test()
{
//		Delay_ms(1000);
		posotion_choose(ONE_pos);
		Delay_ms(1000);
		posotion_choose(TWO_pos);
		Delay_ms(1000);
		posotion_choose(THREE_pos);
		Delay_ms(1000);
		posotion_choose(FREE_pos);
		Delay_ms(1000);	
}
/*********************************************END OF FILE**********************/
