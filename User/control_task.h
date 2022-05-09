#ifndef   __CONTROL_TASK
#define   __CONTROL_TASK

/* Includes *********************************/
#include "stm32f4xx.h"

/* Defines ******************************** */
#define FLAG_SET              0X00
#define FLAG_RESET            0XFF

/* Define struct */

typedef struct {

	uint8_t timer_5ms_flag;
	uint8_t timer_10ms_flag;
	uint8_t timer_20ms_flag;
	uint8_t timer_50ms_flag;
	uint8_t timer_100ms_flag;
	uint8_t timer_500ms_flag;
	uint8_t timer_1000ms_flag;
	uint8_t timer_2000ms_flag;

}TimerFlag_t;

typedef struct {

	uint16_t speed;
	uint16_t raw_speed;
	uint8_t gear_position;
	

}MotorDriverFeedback_t;

typedef struct {

	int32_t angle;

}SteerFeedback_t;

typedef struct {

	MotorDriverFeedback_t MotorDriverFeedback;
  SteerFeedback_t SteerFeedback;
	uint16_t set_speed;
	int16_t set_steer_angle;
}GlobalVal_t;

/* Extern ******************************** */
extern TimerFlag_t TimerFlag;
extern GlobalVal_t GlobalVal;

/* Extern function *********************** */
void ControlTask(void);
void sendCarInfoInBluetooth(void);
void DriverlessMode(void);
#endif
