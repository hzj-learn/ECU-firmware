#ifndef  __PID_H
#define  __PID_H

#include "stm32f4xx.h"

typedef struct 
{
	float ref;
	float fdb;
	
	float Kp;
	float Ki;
	float Kd;
	
	float error_now;
	float error_last;
	float error_lastlast;
	float error_inter;
	float pid_incea_out;
	float pid_out;
	float last_pid_out;
	
	float p_out;
	float i_out;
	float d_out;
	
	int16_t pid_error_limit;
	int16_t pid_out_limit;
}PID_s;


/* Externs */
extern PID_s PID_Steer;
extern PID_s PID_RearWheel;

void PID_Cal(PID_s* motor_type);
void PID_ParamInit(void);
void PID_IncreaCal(PID_s* pid);

#endif

