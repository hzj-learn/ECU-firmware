/**
  ******************************************************************************
 
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"



/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;   	

  /* 使能GPIO时钟*/
  RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	  /* 引脚源*/
  GPIO_PinAFConfig(CAN_TX_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_RX_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

	  /* 配置 CAN TX 引脚 */
  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	  /* 配置 CAN RX 引脚 */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	 	/*中断设置*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN RX中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	/************************CAN通信参数设置**********************************/
	/* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN寄存器初始化*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  使能自动离线管理 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 不锁定 溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 取决于报文标示符 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+5+3) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 时间段1 占用了5个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =20;    //简类通讯协议波特率250kbit/s		  //BTR-BRP 波特率分频器  定义了时间单元的时间长度 45/(1+5+3)/5=1 Mbps
	CAN_Init(CANx, &CAN_InitStructure);
}


/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的筛选器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
	
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN筛选器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//筛选器组0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在掩码模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
  //前一个32位是CAN_FxR1寄存器  后一个32位CAN_FxR2寄存器
	//这4个16位的变量具体含义取决与当前过滤器，工作与何种模式
	//在掩码模式下CAN_FxR1用来存放验证码，CAN_FxR2用来存放屏蔽码
	CAN_FilterInitStructure.CAN_FilterIdHigh= 0x0000;//((((u32)0x18F50505<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 
	CAN_FilterInitStructure.CAN_FilterIdLow= 0x0000;//(((u32)0x18F50505<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0x0000;			//筛选器高16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0x0000;			//筛选器低16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能筛选器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


/**
  * @brief  初始化 Rx Message数据结构体
  * @param  RxMessage: 指向要初始化的数据结构体
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*把接收结构体清零*/
  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_EXT;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    RxMessage->Data[ubCounter] = 0x00;
  }
}


/*
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置,设置一个数据内容为0-7的数据包
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 /////////////////////////////////////////
******************************************
 *********ECU发送数据给EPS***************
******************************************
/////////////////////////////////////////
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage,uint8_t u5,uint8_t u6,uint8_t u7)
{	  
	uint8_t ubCounter = 0;
  uint8_t u8=0;
  //TxMessage.StdId=0x00;						 
  TxMessage->ExtId=0x10F8109A;					 //使用的扩展ID
  TxMessage->IDE=CAN_ID_EXT;					 //扩展模式
  TxMessage->RTR=CAN_RTR_DATA;				 //发送的是数据
  TxMessage->DLC=8;							 //数据长度为8字节
	
	/*设置要发送的数据0-7*/
/*	for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    TxMessage->Data[ubCounter] = ubCounter;
  }*/
	 //简类协议
	 //报文格式：低字节在前高字节在后，高位在前低位在后。
	TxMessage->Data[0] = 0xFF;   //  第一个字节:   0xFF
	TxMessage->Data[1] = 0x77;   //  第二个字节：扭矩控制状态（默认发送0x77，即扭矩叠加模式）
	TxMessage->Data[2] = 0x00;   //  第三个字节：保留0x00
	TxMessage->Data[3] = 0x00;   //  第四个字节：保留0x00
	TxMessage->Data[4] = u5;   //  第五个字节：车速，实时车速；后控制器can反馈
	TxMessage->Data[5] = u6;   //  第六个字节：控制扭矩高位
	TxMessage->Data[6] = u7;   //  第七个字节：控制扭矩低位
	                           //  方向盘扭矩计算公式：torque=receive[5]*256+receive[6]
	u8=(((((0xFF^0x77)^0x00)^0x00)^u5)^u6)^u7;  //前7个字节的异或值
	
	TxMessage->Data[7] = u8;   // 异或校验   协议的加密处理
  
}
/**************************END OF FILE************************************/











