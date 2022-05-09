#ifndef __AUTOCAR_H
#define	__AUTOCAR_H
#include "stm32f4xx.h"

void all_init(void);

void can_receive_process(void);


void can_Transmit_process(uint8_t u5,uint16_t angle);

int angle_process(uint16_t N,uint16_t Ideal_angle,uint16_t Actualangle) ;

void simple_speed(uint8_t Actualspeed,uint8_t Ideal_speed);



















#endif

/**************************END OF FILE************************************/





