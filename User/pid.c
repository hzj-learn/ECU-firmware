#include "pid.h"

PID_s PID_Steer;
PID_s PID_RearWheel;

/*-------------------------------------------------------------------------------
                              PID处理函数
  ------------------------------------------------------------------------------*/
void PID_Cal(PID_s* motor_type)
{
  //float error_position;
	motor_type->error_last   = motor_type->error_now;
	motor_type->error_now    = motor_type->ref - motor_type->fdb;
	motor_type->error_inter += motor_type->error_now;
	
	// limit intergration of pid
	if(motor_type->error_inter > motor_type->pid_error_limit)
		 motor_type->error_inter = motor_type->pid_error_limit;
	if(motor_type->error_inter < -motor_type->pid_error_limit)
		 motor_type->error_inter = -motor_type->pid_error_limit;
	
  motor_type->pid_out = motor_type->Kp * motor_type->error_now 
	                    + motor_type->Ki * motor_type->error_inter 
	                    +	motor_type->Kd * (motor_type->error_now-motor_type->error_last);
	//限幅
	if(motor_type->pid_out > motor_type->pid_out_limit)       
   { motor_type->pid_out = motor_type->pid_out_limit ;  }
	if(motor_type->pid_out < - motor_type->pid_out_limit) 
   { motor_type->pid_out = - motor_type->pid_out_limit ; }

}
void PID_IncreaCal(PID_s* pid)
{
	pid->last_pid_out = pid->pid_out;
	pid->error_lastlast = pid->error_last;
	pid->error_last   = pid->error_now;
	pid->error_now    = pid->ref - pid->fdb;
	
	//pid->error_inter += pid->error_now;
	

	
	pid->p_out = pid->Kp*( pid->error_now - pid->error_last );
	pid->i_out = pid->Ki*pid->error_now;
	pid->d_out = pid->Kd*( pid->error_now - 2*pid->error_last + pid->error_lastlast );
	
	if(pid->i_out > pid->pid_error_limit)
		 pid->i_out = pid->pid_error_limit;
	if(pid->i_out < -pid->pid_error_limit)
		 pid->i_out = -pid->pid_error_limit;
	
	pid->pid_incea_out = pid->p_out + pid->i_out + pid->d_out;
	pid->pid_out = pid->last_pid_out + pid->pid_incea_out;
	
	if(pid->pid_out > pid->pid_out_limit)       
		 pid->pid_out = pid->pid_out_limit ;  
	if(pid->pid_out < - pid->pid_out_limit) 
     pid->pid_out = - pid->pid_out_limit ; 
	
	
	
	
}

//    else if(pid->pid_mode == DELTA_PID)//???P
//    {
//        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
//        pid->iout = pid->i *  pid->err[NOW];
//        pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);
//        
//        abs_limit(&(pid->iout), pid->IntegralLimit);
//        pid->delta_u = pid->pout + pid->iout + pid->dout;

//        pid->delta_out = pid->last_delta_out + pid->delta_u;

//        abs_limit(&(pid->delta_out), pid->MaxOutput);
//        pid->last_delta_out = pid->delta_out;	//update last time
//    }
//    
//    pid->err[LLAST] = pid->err[LAST];
//    pid->err[LAST] = pid->err[NOW];

//    pid->get[LLAST] = pid->get[LAST];
//    pid->get[LAST] = pid->get[NOW];
//    pid->set[LLAST] = pid->set[LAST];
//    pid->set[LAST] = pid->set[NOW];

/*
 ***************************************************************
 * @功 能：PID参数初始化
 * @描 述：
 * @返 回：None
 ***************************************************************
 */
void PID_ParamInit(void)
{
	PID_Steer.Kp 						  = 250;
	PID_Steer.Ki 						  = 2.5;
	PID_Steer.Kd 						  = 300;
	PID_Steer.pid_error_limit = 4500;
	PID_Steer.pid_out_limit   = 7500;
	
	PID_RearWheel.Kp 						  = 0;
	PID_RearWheel.Ki 						  = 0;
	PID_RearWheel.Kd 						  = 0;
	PID_RearWheel.pid_error_limit = 500;
	PID_RearWheel.pid_out_limit   = 4000;
}


/*
 ***************************************************************
 * @功 能：PID串口调试
 * @描 述：
 * @返 回：None
 ***************************************************************
 */
static unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    uint8_t i,j;
    CRC_Temp = 0xffff;

    for (i=0; i<CRC_CNT; i++)
    {
        CRC_Temp ^= Buf[i];
        for (j=0; j<8; j++)
        {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}

/*
 ***************************************************************
 * @功 能：PID串口调试 输出函数
 * @描 述：
 * @返 回：None
 ***************************************************************
 */
void OutPut_Data(int data_1, int data_2, int data_3, int data_4)
{
    int temp[4] = {0};
    unsigned int  temp1[4] = {0};
    unsigned char databuf[10] = {0};
    unsigned char i;
    unsigned short CRC16 = 0;
		
		temp1[0] = data_1;
		temp1[1] = data_2;
		temp1[2] = data_3;
		temp1[3] = data_4;

    for(i=0;i<4;i++)
    {
        databuf[i*2]   = (unsigned char)(temp1[i]%256);
        databuf[i*2+1] = (unsigned char)(temp1[i]/256);
    }

    CRC16 = CRC_CHECK(databuf,8);
    databuf[8] = CRC16%256;
    databuf[9] = CRC16/256;

    for(i=0;i<10;i++)
    {
//			 USART_SendData(USART1,databuf[i]); 
//		   while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
    }    


}



