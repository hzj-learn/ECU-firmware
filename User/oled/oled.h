
//  ��������   : 0.69��OLED �ӿ���ʾ����(STM32F103ZEϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PD6��SCL��
//              SDA   ��PD7��SDA��            
//              ----------------------------------------------------------------
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __OLED_H
#define __OLED_H			  	 
//#include "stdlib.h"	 
#include <stdio.h>
#include "stm32f4xx.h"
	#include "my_sysTick.h"
//#include "stm32f4xx_hal.h"
//#include "cmsis_os.h"


#define  oled_delay      Delay_ms


#define  OLED_SDA_GPIO   GPIOC
#define  OLED_SCL_GPIO   GPIOA

#define  OLED_SDA_PIN    GPIO_Pin_9   
#define  OLED_SCL_PIN    GPIO_Pin_8

//--------------------OLED��ַ--------------------//
#define OLED_ADDR    0x78       // 0x78 ���� 0x7A 

#define OLED_MODE    0
#define SIZE         8
#define XLevelL		   0x00
#define XLevelH		   0x10
#define Max_Column	 128
#define Max_Row		   64
#define	Brightness   0xFF 
#define X_WIDTH 	   128
#define Y_WIDTH 	   64   					

//-----------------OLED IIC�˿ڶ���----------------  	
//SCL
#define oled_sclk_clr() GPIO_ResetBits(OLED_SCL_GPIO, OLED_SCL_PIN)
#define oled_sclk_set() GPIO_SetBits(OLED_SCL_GPIO, OLED_SCL_PIN)
//SDA
#define oled_sdin_clr() GPIO_ResetBits(OLED_SDA_GPIO, OLED_SDA_PIN)
#define oled_sdin_set() GPIO_SetBits(OLED_SDA_GPIO, OLED_SDA_PIN)

////SCL
//#define oled_sclk_clr() HAL_GPIO_WritePin(OLED_SCL_GPIO, OLED_SCL_PIN, GPIO_PIN_RESET)
//#define oled_sclk_set() HAL_GPIO_WritePin(OLED_SCL_GPIO, OLED_SCL_PIN, GPIO_PIN_SET)
////SDA
//#define oled_sdin_clr() HAL_GPIO_WritePin(OLED_SDA_GPIO, OLED_SDA_PIN, GPIO_PIN_RESET)
//#define oled_sdin_set() HAL_GPIO_WritePin(OLED_SDA_GPIO, OLED_SDA_PIN, GPIO_PIN_SET)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//------------------OLED ����λ�ÿ��ƺ궨��--------------------//
//��������
//�����С18  Y����������� 4 ��
#define  CHINESE_Y_1        0
#define  CHINESE_Y_2        2
#define  CHINESE_Y_3        4
#define  CHINESE_Y_4        6
//�����С18  X�����������  7 ��
#define  CHINESE_X_1        0
#define  CHINESE_X_2        18
#define  CHINESE_X_3        36
#define  CHINESE_X_4        54
#define  CHINESE_X_5        72
#define  CHINESE_X_6        90
#define  CHINESE_X_7        108

//������ʾ
//�����С 12    X ÿ��������X����ռ6����λ    Y��������8

#define  SIZE12_X_SIZE         6

#define  SIZE12_Y_1            0
#define  SIZE12_Y_2            2
#define  SIZE12_Y_3            3
#define  SIZE12_Y_4            4
#define  SIZE12_Y_5            5
#define  SIZE12_Y_6            6
#define  SIZE12_Y_7            7
#define  SIZE12_Y_8            8

//�����С 16    X ÿ��������X����ռ6����λ    Y��������8
#define  SIZE16_X_SIZE         8

#define  SIZE16_Y_1            0
#define  SIZE16_Y_2            2
#define  SIZE16_Y_3            4
#define  SIZE16_Y_4            6



//��������
#define   NUM_OFFSET        10
#define   SHOW_NUM(x)       10+2*x
//OLED�����ú���\

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void iic_start(void);
void iic_stop(void);
void iic_writeCommand(unsigned char IIC_Command);
void iic_writeData(unsigned char IIC_Data);
void iic_writeByte(unsigned char IIC_Byte);
void iic_waitAck(void);
void iic_delay(void);

//OLED�����ú���
void oled_wr_byte(uint8_t dat,uint8_t cmd);	    
void oled_displayON(void);
void oled_displayOFF(void);
void oled_refreshGram(void);  		    
void oledInit(void);
void oled_clear(void);
void oled_drawPoint(uint8_t x,uint8_t y,uint8_t t);
uint8_t oled_readPoint(uint8_t x,uint8_t y);
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void oled_showChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t f_w,uint8_t f_h,uint8_t mode);
void oled_showNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t f_w,uint8_t f_h);
void oled_showRealNum(uint8_t x,uint8_t y,int32_t num,uint8_t len,uint8_t f_w,uint8_t f_h);
void oled_showString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t f_w,uint8_t f_h);	
void oled_showPicture(uint8_t x,uint8_t y,const uint8_t *p,uint8_t p_w,uint8_t p_h);

void oledGpioConfig(void);
void oledDisplayInit(void);

#endif  
	 
#ifdef __cplusplus
}
#endif


