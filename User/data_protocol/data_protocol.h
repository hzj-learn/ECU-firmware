/*
 ***************************************************************
 * File Name: data_protocol.h
 * Description:
 ***************************************************************
 */

/* Defines ************************************************** */
#ifndef __DATA_PROTOCOL_H
#define __DATA_PROTOCOL_H

/* Includes ************************************************* */
#include "stm32f4xx.h"
	
/*
 ***************************************************************
 *                    数据协议相关宏定义
 ***************************************************************
 */
/* 获取高低八位数据 */
#define BYTE0(dwTemp)       			 ( *( (char *)(&dwTemp)	   ) )
#define BYTE1(dwTemp)      				 ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       			 ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       			 ( *( (char *)(&dwTemp) + 3) )

#define FRAME_LENGTH_MAX                   50u

 /* PC ---> ECU */
#define PC_FRAME_HEAD_L                    0xAA
#define PC_FRAME_HEAD_H                    0xAF
/* 帧类型 */
#define PC_FRAME_TYPE_DATA                 0x01
#define PC_FRAME_TYPE_REQUEST              0x02
#define PC_FRAME_TYPE_ANSWER               0x03
/* 标识符 */

/* 数据帧标识符 */
#define PC_DATA_IDENTIFIER_TEST      0x01
/* 请求帧标识符 */
#define PC_REQ_IDENTFIER_TEST              0xFF
#define PC_FRAME_REQ_DATA_ONCE             0x10
#define PC_FRAME_REQ_DATA_TX_EVERY_10MS    0x11
#define PC_FRAME_REQ_DATA_TX_EVERY_20MS    0x12
#define PC_FRAME_REQ_DATA_TX_EVERY_50MS    0x13

/* 应答帧标识符 */


 /* ECU ---> PC */
#define ECU_FRAME_HEAD_L                   0xAA
#define ECU_FRAME_HEAD_H                   0xAA

#define ECU_FRAME_TYPE_DATA                0x01
#define ECU_FRAME_TYPE_REQUEST             0x02
#define ECU_FRAME_TYPE_ANSWER              0x03

/* 数据帧标识符 */
#define ECU_ANSWER_PC_REQ_TEST 						 0x01
#define ECU_SEND_TEST_DATA    						 0x02
#define ECU_SEND_CAR_REAL_DATA             0x03
#define ECU_SEND_CAR_RAW_DATA              0x04
#define ECU_ATIVE_UPLOAD_DATA              0x05
/* 请求帧标识符 */
#define ECU_IDENTFIER_REQ_TEST             0xFF
/* 应答帧标识符 */
#define ECU_IDENTIFIER_ANSWER_TEST         0xEE

/* 状态 */
/*  extern use */
#define DP_ERROR                           0xFF
#define DP_NORMAL                          0x00
#define DP_FALSE                           0xFF
#define DP_TRUE                            0x00
/*
 ***************************************************************
 *                      结构体定义
 ***************************************************************
 */
typedef struct
{
	uint8_t  frame_type;
	uint8_t  data_identifier;
	uint8_t  request_identifier;
	uint8_t  answer_identifier;
	uint8_t  identifier;
	uint8_t  data_len;
	uint8_t  check_sum;
	uint16_t heart_beat_data;
	int16_t  data_test;
	uint8_t  data[FRAME_LENGTH_MAX];
}FrameInfo_t;

typedef struct
{
  uint8_t  is_check_error;
	uint8_t  connect_statue;
}Statue_t;

typedef struct
{
	FrameInfo_t RxFrameInfo;
	FrameInfo_t TxFrameInfo;
	Statue_t Statue;
}DataProtocolInfo_t;

typedef struct
{
	/* mm/s */
	int16_t speed;
	int16_t gear_position;
	int16_t dir;
	
	int16_t pos_x;
	int16_t pos_y;
	uint8_t mode;
	
	int16_t imu_acc_x;
	int16_t imu_acc_y;
	int16_t imu_acc_z;
	int16_t imu_gyro_x;
	int16_t imu_gyro_y;
	int16_t imu_gyro_z;
	int16_t imu_pitch;
	int16_t imu_yaw;
	int16_t imu_roll;
}CarGloVal_t;

typedef struct
{
	/* mm/s */
	uint8_t  running_mode;
	uint8_t  running_statue;
	uint16_t throttle_val;
	uint8_t  throttle_position;
	uint16_t torque_val;
	uint8_t  gear_position;
	uint8_t  steer_dir;
	uint8_t  brake_state;
	int16_t  heart_beat_data;
	uint16_t raw_steer_angle;
	int16_t  real_steer_angle;
}CarCtlData_t;

/* Extern declaration */
extern DataProtocolInfo_t DataProtocolInfo;
extern CarCtlData_t CarCtlDataFromWireless;
extern CarCtlData_t CarCtlDataFromWireless;
extern CarCtlData_t CarCtlDataFromPC;

/* Function declaration */
void DataProtocolPrepareParsed (uint8_t data);
void addHearBeatData(void);
uint16_t getHearBeatData(void);
void DataProtocolCommunicationTask (int16_t cycle_time_ms);
void DataProcotolInit(void);
void BluetoothRemoteCheckTask(void);
void Serial_Send_Data_2(uint8_t *dataToSend , uint8_t length);

void Driverless_Data_Receive_Parsed(uint8_t *data_buf, uint8_t num);
void Driverless_DataProtocolPrepareParsed (uint8_t data);
#endif
