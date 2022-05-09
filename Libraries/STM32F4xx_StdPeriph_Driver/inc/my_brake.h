#ifndef __MY_BRAKE_H
#define	__MY_BRAKE_H

#include "stm32f4xx.h"

//�������
#define PWM_PIN                  GPIO_Pin_3                
#define PWM_GPIO_PORT            GPIOC                      
#define PWM_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define DIR_PIN                  GPIO_Pin_10                
#define DIR_GPIO_PORT            GPIOF                      
#define DIR_GPIO_CLK             RCC_AHB1Periph_GPIOF

#define EN_PIN                  GPIO_Pin_1               
#define EN_GPIO_PORT            GPIOC                     
#define EN_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define KEY_PIN                  GPIO_Pin_11                
#define KEY_GPIO_PORT            GPIOG                      
#define KEY_GPIO_CLK             RCC_AHB1Periph_GPIOG



#define PWM_TOGGLE		            digitalToggle(PWM_GPIO_PORT,PWM_PIN)
#define PWM_ON			            digitalHi(PWM_GPIO_PORT,PWM_PIN)
#define PWM_OFF			            digitalLo(PWM_GPIO_PORT,PWM_PIN)

#define DIR_TOGGLE	            	digitalToggle(DIR_GPIO_PORT,DIR_PIN)
#define DIR_ON			            digitalHi(DIR_GPIO_PORT,DIR_PIN)
#define DIR_OFF			            digitalLo(DIR_GPIO_PORT,DIR_PIN)

#define EN_TOGGLE		            digitalToggle(EN_GPIO_PORT,EN_PIN)
#define EN_ON			            digitalHi(EN_GPIO_PORT,EN_PIN)
#define EN_OFF			            digitalLo(EN_GPIO_PORT,EN_PIN)





/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//����Ϊ�ߵ�ƽ
#define digitalLo(p,i)			 {p->BSRRH=i;}		//����͵�ƽ
#define digitalToggle(p,i)		 {p->ODR ^=i;}		//�����ת״̬

void step_motor_GPIO_Config(void);
void posotion_choose(uint8_t posotion);
void test();

#endif /* __LED_H */
