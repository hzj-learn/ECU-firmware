
//  功能描述   : 0.69寸OLED 接口演示例程(STM32F103ZE系列IIC)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PD6（SCL）
//              SDA   接PD7（SDA）            
//              ----------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#include "oled.h"
//#include "stdlib.h"
#include <stdio.h>
#include "oledfont.h"  	 
#include "font.h"
#include "my_sysTick.h"
	  
#include "stm32f4xx.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

volatile static uint8_t oledGram[128][8];



void iic_delay(void)
{
	__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();
}
/**********************************************
                   IIC Start
**********************************************/
void iic_start(void)
{

	oled_sclk_set() ;
	oled_sdin_set();
	iic_delay();
	oled_sdin_clr();
	iic_delay();
	oled_sclk_clr();
	iic_delay();
}

/**********************************************
//IIC Stop
**********************************************/
void iic_stop(void)
{
	oled_sdin_clr();
	oled_sclk_clr();
	  iic_delay();
	oled_sclk_set();
	  iic_delay();
	oled_sdin_set();
	  iic_delay();
	
}

void iic_waitAck(void)
{
	oled_sclk_set() ;
	iic_delay();
	oled_sclk_clr();
	iic_delay();
}
/**********************************************
               IIC Write byte
**********************************************/

void iic_write_byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	oled_sclk_clr();
	for(i=0;i<8;i++)		
	{
			m=da;
		//	oled_sclk_clr();
		m=m&0x80;
		if(m==0x80)  {oled_sdin_set();}
		else          oled_sdin_clr();
			da=da<<1;
		   iic_delay();
		oled_sclk_set();
		iic_delay();
		oled_sclk_clr();
		iic_delay();
		}


}
/**********************************************
               IIC Write Command
**********************************************/
void iic_write_command(unsigned char IIC_Command)
{
   iic_start();
   iic_write_byte(OLED_ADDR);            //Slave address,SA0=0
	 iic_waitAck();	
   iic_write_byte(0x00);			//write command
	 iic_waitAck();	
   iic_write_byte(IIC_Command); 
	 iic_waitAck();	
   iic_stop();
}
/**********************************************
              IIC Write Data
**********************************************/
void iic_write_data(unsigned char IIC_Data)
{
   iic_start();
   iic_write_byte(OLED_ADDR);			//D/C#=0; R/W#=0
	iic_waitAck();	
   iic_write_byte(0x40);			//write data
	iic_waitAck();	
   iic_write_byte(IIC_Data);
	iic_waitAck();	
   iic_stop();
}

/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}




//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/*-----------------------------------------------
                主界面显示函数
-------------------------------------------------*/
//void Main_Interface_Show(void)
//{
//   OLED_Clear();
////	 OLED_ShowString(CHINESE_X_1, 3, "....", 16);
//   OLED_ShowString(CHINESE_X_1, 3, "Please..", 16);	
////	OLED_ShowCHinese(CHINESE_X_1+9,CHINESE_Y_1,0);//
////	OLED_ShowCHinese(CHINESE_X_1+27,CHINESE_Y_1,1);//
////	OLED_ShowCHinese(CHINESE_X_1+45,CHINESE_Y_1,2);//
////	OLED_ShowCHinese(CHINESE_X_1+63,CHINESE_Y_1,3);//
////	OLED_ShowCHinese(CHINESE_X_1+81,CHINESE_Y_1,4);//
////	OLED_ShowCHinese(CHINESE_X_1+99,CHINESE_Y_1,5);//
////				
////	OLED_ShowCHinese(CHINESE_X_1+7,CHINESE_Y_3+1,6);//
////	OLED_ShowCHinese(CHINESE_X_2+7,CHINESE_Y_3+1,7);//
////	OLED_ShowString(CHINESE_X_2+15+7,CHINESE_Y_3+1,":",16);
////					
////	OLED_ShowCHinese(CHINESE_X_7,CHINESE_Y_3,8);//
////	OLED_ShowCHinese(CHINESE_X_7,CHINESE_Y_3+2,9);// 
//}




/*----------------------------------------------------------------------------------------------------------*/

//向SSD1306写入一个字节。（SPI模式）
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;	
void oled_wr_byte(uint8_t dat,uint8_t cmd)
{		  
		if(cmd)
		{
		 iic_write_data(dat);
		}
		else
		{
		 iic_write_command(dat);	
		}  	  
}

//更新显存到LCD		 
void oled_refreshGram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		oled_wr_byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		oled_wr_byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		oled_wr_byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			oled_wr_byte(oledGram[n][i],OLED_DATA); 
	}   
} 
//开启OLED显示    
void oled_displayON(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_wr_byte(0X14,OLED_CMD);  //DCDC ON
	oled_wr_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void oled_displayOFF(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_wr_byte(0X10,OLED_CMD);  //DCDC OFF
	oled_wr_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void oled_clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			oledGram[n][i]=0X00;  
	oled_refreshGram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void oled_drawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)oledGram[x][pos]|=temp;
	else oledGram[x][pos]&=~temp;	    
}
//读点 
//x:0~127
//y:0~63
uint8_t oled_readPoint(uint8_t x,uint8_t y)
{
	uint8_t pos,bx,temp=0x00;
	//x = 127-x;
	y = 63-y;
	pos=y/8;
	bx=y%8;
	temp=1<<bx;
  if(temp&oledGram[x][pos]) return 1;
	return 0;
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
		for(y=y1;y<=y2;y++)
			oled_drawPoint(x,y,dot);											    
//	oled_refreshGram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//f_w:字宽
//f_h:字高
//mode:0,反白显示;1,正常显示				 
void oled_showChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t f_w,uint8_t f_h,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;	
	uint8_t csize=(f_h/8+((f_h%8)?1:0))*f_w;//得到自由分辨字符所占的字节数
	chr=chr-' ';//得到偏移后的值		 
	for(t=0;t<csize;t++)
	{   
		if(f_w==6&&f_h==8)temp=asc2_0608[chr][t];		//调用0608ascii字体
		else if(f_w==6&&f_h==12)temp=asc2_0612[chr][t];	//调用0612ascii字体
		else if(f_w==12&&f_h==24)temp=asc2_1224[chr][t];//调用1224ascii字体
		else return;	//没有的字库
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_drawPoint(x,y,mode);
			else oled_drawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==f_h)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}     
}
//m^n函数
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//f_w:字宽
//f_h:字高
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void oled_showNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t f_w,uint8_t f_h)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_showChar(x+(f_w)*t,y,' ',f_w,f_h,1);
				continue;
			}
			else 
				enshow=1; 
		}
	 	oled_showChar(x+(f_w)*t,y,temp+'0',f_w,f_h,1); 
	}
} 

void oled_showRealNum(uint8_t x,uint8_t y,int32_t num,uint8_t len,uint8_t f_w,uint8_t f_h)
{         	
	uint8_t t,temp,flag;
	uint8_t enshow=0;	
  
	
  if(num < 0)
	{
	  flag = 1;
		num = -num;
	}
	
	
	if(flag == 1)
	{
	  oled_showChar(x-5,y,'-',f_w,f_h,1);
	}
	else
	{
		oled_showChar(x-5,y,' ',f_w,f_h,1);
	}
	
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_showChar(x+(f_w)*t,y,' ',f_w,f_h,1);
				continue;
			}
			else 
				enshow=1; 
		}
	 	oled_showChar(x+(f_w)*t,y,temp+'0',f_w,f_h,1); 
	}
}
//显示字符串
//x,y:起点坐标  
//f_w:字宽
//f_h:字高
//*p:字符串起始地址 
void oled_showString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t f_w,uint8_t f_h)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(f_w))){x=0;y+=f_h;}
        if(y>(64-f_h)){y=x=0;oled_clear();}
        oled_showChar(x,y,*p,f_w,f_h,1);	 
        x+=f_w;
        p++;
    }  
	
}

//显示图片
//x,y:起点坐标  
//p_w:图片宽（单位像素）
//p_h:图片高（单位像素）
//*p:图片起始地址 
void oled_showPicture(uint8_t x,uint8_t y,const uint8_t *p,uint8_t p_w,uint8_t p_h)
{	
	uint8_t temp,i,col,row;
	uint8_t y0=y;
	uint8_t width=p_w;
	if(x+p_w>128)width=128-p_w;//实际显示宽度
	uint8_t high=p_h;
	if(y+p_h>64)high=64-p_h;//实际显示高度
	uint8_t exp_col_bytes=(p_h/8+((p_h%8)?1:0));//显示一列的字节数
	uint8_t act_col_bytes=(high/8+((high%8)?1:0));//实际显示一列的字节数
	
	for(row=0;row<width;row++)//列++
	{
		for(col=0;col<act_col_bytes;col++)//显示一列
		{   
			temp = p[col+row*exp_col_bytes];
			for(i=0;i<8;i++)
			{
				if(temp&0x80)oled_drawPoint(x,y,1);
				else oled_drawPoint(x,y,0);
				temp<<=1;
				y++;
				if((y-y0)==high)
				{
					y=y0;
					x++;
					break;
				}		
			} 
		}
	}		
}



//初始化OLED				    
void oledInit(void)
{ 	
	
	GPIO_InitTypeDef         GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE ); 
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); 

  /* GPIO Ports Clock Enable */

	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;        
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;      //复用功能
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;     //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      //上拉
	GPIO_Init( GPIOA, &GPIO_InitStructure ); 
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;        
	GPIO_Init( GPIOC, &GPIO_InitStructure ); 
	
  oled_delay(500);
	
	oled_wr_byte(0xAE,OLED_CMD); //关闭显示
	oled_wr_byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	oled_wr_byte(0x80,OLED_CMD); //[3:0],分频因子;[7:4],震荡频率
	oled_wr_byte(0xA8,OLED_CMD); //设置驱动路数
	oled_wr_byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //设置显示偏移
	oled_wr_byte(0X00,OLED_CMD); //默认为0

	oled_wr_byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	oled_wr_byte(0x8D,OLED_CMD); //电荷泵设置
	oled_wr_byte(0x14,OLED_CMD); //bit2，开启/关闭
	oled_wr_byte(0x20,OLED_CMD); //设置内存地址模式
	oled_wr_byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_wr_byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_wr_byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	oled_wr_byte(0x12,OLED_CMD); //[5:4]配置
		 
	oled_wr_byte(0x81,OLED_CMD); //对比度设置
	oled_wr_byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	oled_wr_byte(0xD9,OLED_CMD); //设置预充电周期
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_wr_byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	oled_wr_byte(0xAF,OLED_CMD); //开启显示	 
	oled_clear();
	
	
	oled_delay(500);
	
//	/*显示图片*/
//	oled_showPicture(0,0,bmp_rssi[3],24,22);
//	oled_showPicture(24,0,bmp_logo,80,22);
//	oled_showPicture(106,4,bmp_battery[50*12/99],10,16);
//	oled_showChar(119,13,'%',6,8,1);
//	OLED_ShowString(CHINESE_X_1, 3, "Please..", 16);	
//	oled_showNum(116,4,50,2,6,8);
//	
//	GUI_Rectangle(0, 25, 125, 55);
	oled_showPicture(25,10,bmp_logo_GOGO1,90,30);
	oled_refreshGram();
	
  oled_delay(500);
//	test_Button();
}  


void oledGpioConfig(void)
{
	
	GPIO_InitTypeDef         GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE ); 
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); 

  /* GPIO Ports Clock Enable */

	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;        
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;      //复用功能
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;     //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      //上拉
	GPIO_Init( GPIOA, &GPIO_InitStructure ); 
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;        
	GPIO_Init( GPIOC, &GPIO_InitStructure ); 
}

void oledDisplayInit(void)
{
	  oled_delay(500);
	
	oled_wr_byte(0xAE,OLED_CMD); //关闭显示
	oled_wr_byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	oled_wr_byte(0x80,OLED_CMD); //[3:0],分频因子;[7:4],震荡频率
	oled_wr_byte(0xA8,OLED_CMD); //设置驱动路数
	oled_wr_byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //设置显示偏移
	oled_wr_byte(0X00,OLED_CMD); //默认为0

	oled_wr_byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	oled_wr_byte(0x8D,OLED_CMD); //电荷泵设置
	oled_wr_byte(0x14,OLED_CMD); //bit2，开启/关闭
	oled_wr_byte(0x20,OLED_CMD); //设置内存地址模式
	oled_wr_byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_wr_byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_wr_byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	oled_wr_byte(0x12,OLED_CMD); //[5:4]配置
		 
	oled_wr_byte(0x81,OLED_CMD); //对比度设置
	oled_wr_byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	oled_wr_byte(0xD9,OLED_CMD); //设置预充电周期
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_wr_byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	oled_wr_byte(0xAF,OLED_CMD); //开启显示	 
	oled_clear();
	
	oled_delay(500);
	oled_showPicture(25,10,bmp_logo_GOGO1,90,30);
	oled_refreshGram();
  oled_delay(500);
}

#ifdef __cplusplus
}
#endif










