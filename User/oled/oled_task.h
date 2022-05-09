#ifndef __OLED_TASK_H
#define __OLED_TASK_H
 
//-----------------------------------------
//头文件
//#include "header.h"


typedef enum 
{
	MAIN_UI,
	MENU_UI,
}show_ui_e;

//-----------------------------------------
//函数声明
void OLED_Dis_DriveData(void);
void OLED_Dis_DriveData_Update(void);

#endif 

