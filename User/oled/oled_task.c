#include "oled_task.h"
#include "oled.h"
#include "lcmdrv.h"

/* 
 **************************************************************
 * ���ܣ� ��ʾ�����ٶȡ���������CAN_ID ������
 * ���أ� ��
 *
 *
 *          - - - - - - - - 
 *         |    GO GO      |
 *         |CAN_ID:        |
 *         |speed:         |
 *         |feedback:      |
 *          - - - - - - - -
 **************************************************************
 */
 
void OLED_Dis_DriveData(void)
{
	
//  OLED_Clear(); 
//	OLED_ShowString(CHINESE_X_3, 0, "GO GO", 16);
//	OLED_ShowString(CHINESE_X_1, 2, "REC_ID:    0x", 12);
//	OLED_ShowString(CHINESE_X_1, 3, "speed_set:", 12);
//	OLED_ShowString(CHINESE_X_1, 4, "pid_out:", 12);
//	OLED_ShowString(CHINESE_X_1, 5, "feedback:", 12);
	
}
//���ݸ��º���
void OLED_Dis_DriveData_Update(void)
{
//	OLED_ShowNum(CHINESE_X_6+13, 2, motor_can_rx_message.ExtId, 3, 12);
//	 OLED_ShowSignNum(CHINESE_X_6, 3, DRIVE_GlobleVal.Motor_Ctl_Speed, 4, 12);  
//  OLED_ShowSignNum(CHINESE_X_6, 4, DRIVE_GlobleVal.Motor_PIDOutput, 4, 12);                                             //�ٶ���ʾ
//  OLED_ShowSignNum(CHINESE_X_6+4, 5, DRIVE_GlobleVal.Motor_Feeback, 4, 12);             //������������ʾ  
//	
}


/* 
 **************************************************************
 * ���ܣ� ��ʾ�����ٶȡ���������CAN_ID ������
 * ���أ� ��
 **************************************************************
 */


/* 
 **************************************************************
 * ���ܣ� ��ʾ�����ٶȡ���������CAN_ID ������
 * ���أ� ��
 **************************************************************
 */
show_ui_e   show_ui = MAIN_UI;


void setShow_ui(show_ui_e ui)
{
   show_ui = ui;
	 GUI_ClearSCR();
}

void OLEDDisplay_Task(void)
{
  switch (show_ui)
  {
  	case MAIN_UI:
  		break;
  	case MENU_UI:
  		break;
  	default:
  		break;
  }
}
