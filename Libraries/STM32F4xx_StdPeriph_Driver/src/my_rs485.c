/**
  ******************************************************************************

  *                        485驱动
  ******************************************************************************

  ******************************************************************************
  */ 
#include "my_rs485.h"
#include <stdarg.h>


static void Delay(__IO u32 nCount); 


/// 配置USART接收中断
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = _485_INT_IRQ; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * 函数名：_485_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void _485_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* 配置时钟源 USART 时钟 */
	RCC_AHB1PeriphClockCmd(_485_USART_RX_GPIO_CLK|_485_USART_TX_GPIO_CLK|_485_RE_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(_485_USART_CLK, ENABLE); 

	
	/* Tx引脚源*/
  GPIO_PinAFConfig(_485_USART_RX_GPIO_PORT,_485_USART_RX_SOURCE, _485_USART_RX_AF);

  /* Rx引脚源*/
  GPIO_PinAFConfig(_485_USART_TX_GPIO_PORT,_485_USART_TX_SOURCE,_485_USART_TX_AF);

	
	/* USART GPIO 配置 */
   /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  /*TX*/
  GPIO_InitStructure.GPIO_Pin = _485_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(_485_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /*RX*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Pin = _485_USART_RX_PIN;
  GPIO_Init(_485_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  
  /* 485收发控制管脚 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = _485_RE_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(_485_RE_GPIO_PORT, &GPIO_InitStructure);
	  
	/* USART 模式配置 */
	USART_InitStructure.USART_BaudRate = _485_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //无奇偶检验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(_485_USART, &USART_InitStructure); 
	/*使能串口*/
  USART_Cmd(_485_USART, ENABLE);
	
	NVIC_Configuration();
	/* 使能串口接收中断 */
	USART_ITConfig(_485_USART, USART_IT_RXNE, ENABLE);
	/*控制485芯片进入接收芯片*/
	GPIO_ResetBits(_485_RE_GPIO_PORT,_485_RE_PIN); //默认进入接收模式
}



/***************** 发送一个字符  **********************/
//使用单字节数据发送前要使能发送引脚，发送后要使能接收引脚。
void _485_SendByte(  uint8_t ch )
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(_485_USART,ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(_485_USART, USART_FLAG_TXE) == RESET);	
	
}
/*****************  发送指定长度的字符串 **********************/
void _485_SendStr_length( uint8_t *str,uint32_t strlen )
{
	unsigned int k=0;

	_485_TX_EN()	;//	使能发送数据	
    do 
    {
        _485_SendByte( *(str + k) );
        k++;
    } while(k < strlen);
		
	/*加短暂延时，保证485发送数据完毕*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	使能接收数据
}


/*****************  发送字符串 **********************/
void _485_SendString(  uint8_t *str)
{
	unsigned int k=0;
	
	_485_TX_EN()	;//	使能发送数据
	
    do 
    {
        _485_SendByte(  *(str + k) );
        k++;
    } while(*(str + k)!='\0');
	
	/*加短暂延时，保证485发送数据完毕*/
	Delay(0xFFF);
		
	_485_RX_EN()	;//	使能接收数据
}









//中断缓存串口数据
#define UART_BUFF_SIZE      10
volatile    uint16_t uart_p = 0;
unsigned char     uart_buff[UART_BUFF_SIZE];
uint8_t flag_rx=0,biaoding,zhenwei;
uint8_t useable_buff[10]={0};
uint8_t tranmit_flag=0;
void bsp_485_IRQHandler(void)
{
	static uint8_t i=0,z;
    
        if(USART_GetITStatus(_485_USART, USART_IT_RXNE) != RESET)
        {
					  
            uart_buff[uart_p] = USART_ReceiveData(_485_USART);
            uart_p++;
					 // useable_buff[uart_p]=uart_buff[uart_p];
						if(uart_p==10)
						{
							uart_p=0;
							flag_rx=1;
						}
						USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);
        }
    
		if(flag_rx==1)
		{
			LED_RED;
			flag_rx=0;
			for(i=0;i<=9;i++)
			{
				if(uart_buff[i]==0xAB&&i!=9)
					biaoding=i;//标定帧头
				if(i==9&&uart_buff[i]==0xAB&&uart_buff[0]==0xCD)
					biaoding=9;
			}
			if(biaoding==0)
				zhenwei=9;//找到帧尾
			else
				zhenwei=biaoding-1;
		
		if(uart_buff[zhenwei]==0x3D)//判断帧尾
		{
			for(z=0;z<(9-biaoding);z++)//赋值
			{
				useable_buff[z]=uart_buff[biaoding+z];
			}
			for(z=(10-biaoding);z<=9;z++)
			{
				useable_buff[z]=uart_buff[z-(10-biaoding)];
			}
			tranmit_flag=1;
		}
		
	}
	/*	else
		{
			USART_ClearITPendingBit(_485_USART, USART_IT_RXNE);
//			clean_rebuff();       
		}*/
}

/*****************************************************************************************

                             得到车辆角度

********************************************************************************************/
double buy_angle=0.0;

double get_angle(void)
{
	uint32_t angle_data=0;

		angle_data=useable_buff[5]<<24|useable_buff[4]|useable_buff[3]<<8|useable_buff[6]<<16;
		buy_angle=(double)(angle_data*0.3515625);

	return buy_angle;
}

//获取接收到的数据和长度
char *get_rebuff(uint16_t *len) 
{
    *len = uart_p;
    return (char *)&uart_buff;
}

//清空缓冲区
void clean_rebuff(void) 
{

    uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;

}




static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
