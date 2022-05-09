
//  ��������   : 0.69��OLED �ӿ���ʾ����(STM32F103ZEϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PD6��SCL��
//              SDA   ��PD7��SDA��            
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
//OLED���Դ�
//��Ÿ�ʽ����.
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




//m^n����
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/*-----------------------------------------------
                ��������ʾ����
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

//��SSD1306д��һ���ֽڡ���SPIģʽ��
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;	
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

//�����Դ浽LCD		 
void oled_refreshGram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		oled_wr_byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		oled_wr_byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		oled_wr_byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
			oled_wr_byte(oledGram[n][i],OLED_DATA); 
	}   
} 
//����OLED��ʾ    
void oled_displayON(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC����
	oled_wr_byte(0X14,OLED_CMD);  //DCDC ON
	oled_wr_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void oled_displayOFF(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC����
	oled_wr_byte(0X10,OLED_CMD);  //DCDC OFF
	oled_wr_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void oled_clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			oledGram[n][i]=0X00;  
	oled_refreshGram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void oled_drawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)oledGram[x][pos]|=temp;
	else oledGram[x][pos]&=~temp;	    
}
//���� 
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
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
		for(y=y1;y<=y2;y++)
			oled_drawPoint(x,y,dot);											    
//	oled_refreshGram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//f_w:�ֿ�
//f_h:�ָ�
//mode:0,������ʾ;1,������ʾ				 
void oled_showChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t f_w,uint8_t f_h,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;	
	uint8_t csize=(f_h/8+((f_h%8)?1:0))*f_w;//�õ����ɷֱ��ַ���ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
	for(t=0;t<csize;t++)
	{   
		if(f_w==6&&f_h==8)temp=asc2_0608[chr][t];		//����0608ascii����
		else if(f_w==6&&f_h==12)temp=asc2_0612[chr][t];	//����0612ascii����
		else if(f_w==12&&f_h==24)temp=asc2_1224[chr][t];//����1224ascii����
		else return;	//û�е��ֿ�
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
//m^n����
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//f_w:�ֿ�
//f_h:�ָ�
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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
//��ʾ�ַ���
//x,y:�������  
//f_w:�ֿ�
//f_h:�ָ�
//*p:�ַ�����ʼ��ַ 
void oled_showString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t f_w,uint8_t f_h)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(f_w))){x=0;y+=f_h;}
        if(y>(64-f_h)){y=x=0;oled_clear();}
        oled_showChar(x,y,*p,f_w,f_h,1);	 
        x+=f_w;
        p++;
    }  
	
}

//��ʾͼƬ
//x,y:�������  
//p_w:ͼƬ����λ���أ�
//p_h:ͼƬ�ߣ���λ���أ�
//*p:ͼƬ��ʼ��ַ 
void oled_showPicture(uint8_t x,uint8_t y,const uint8_t *p,uint8_t p_w,uint8_t p_h)
{	
	uint8_t temp,i,col,row;
	uint8_t y0=y;
	uint8_t width=p_w;
	if(x+p_w>128)width=128-p_w;//ʵ����ʾ���
	uint8_t high=p_h;
	if(y+p_h>64)high=64-p_h;//ʵ����ʾ�߶�
	uint8_t exp_col_bytes=(p_h/8+((p_h%8)?1:0));//��ʾһ�е��ֽ���
	uint8_t act_col_bytes=(high/8+((high%8)?1:0));//ʵ����ʾһ�е��ֽ���
	
	for(row=0;row<width;row++)//��++
	{
		for(col=0;col<act_col_bytes;col++)//��ʾһ��
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



//��ʼ��OLED				    
void oledInit(void)
{ 	
	
	GPIO_InitTypeDef         GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE ); 
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); 

  /* GPIO Ports Clock Enable */

	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;        
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;      //���ù���
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;     //���츴�����
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      //����
	GPIO_Init( GPIOA, &GPIO_InitStructure ); 
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;        
	GPIO_Init( GPIOC, &GPIO_InitStructure ); 
	
  oled_delay(500);
	
	oled_wr_byte(0xAE,OLED_CMD); //�ر���ʾ
	oled_wr_byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	oled_wr_byte(0x80,OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
	oled_wr_byte(0xA8,OLED_CMD); //��������·��
	oled_wr_byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //������ʾƫ��
	oled_wr_byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	oled_wr_byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	oled_wr_byte(0x8D,OLED_CMD); //��ɱ�����
	oled_wr_byte(0x14,OLED_CMD); //bit2������/�ر�
	oled_wr_byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	oled_wr_byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	oled_wr_byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	oled_wr_byte(0xDA,OLED_CMD); //����COMӲ����������
	oled_wr_byte(0x12,OLED_CMD); //[5:4]����
		 
	oled_wr_byte(0x81,OLED_CMD); //�Աȶ�����
	oled_wr_byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	oled_wr_byte(0xD9,OLED_CMD); //����Ԥ�������
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	oled_wr_byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	oled_wr_byte(0xAF,OLED_CMD); //������ʾ	 
	oled_clear();
	
	
	oled_delay(500);
	
//	/*��ʾͼƬ*/
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
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;      //���ù���
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;     //���츴�����
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      //����
	GPIO_Init( GPIOA, &GPIO_InitStructure ); 
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;        
	GPIO_Init( GPIOC, &GPIO_InitStructure ); 
}

void oledDisplayInit(void)
{
	  oled_delay(500);
	
	oled_wr_byte(0xAE,OLED_CMD); //�ر���ʾ
	oled_wr_byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	oled_wr_byte(0x80,OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
	oled_wr_byte(0xA8,OLED_CMD); //��������·��
	oled_wr_byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //������ʾƫ��
	oled_wr_byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	oled_wr_byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	oled_wr_byte(0x8D,OLED_CMD); //��ɱ�����
	oled_wr_byte(0x14,OLED_CMD); //bit2������/�ر�
	oled_wr_byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	oled_wr_byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	oled_wr_byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	oled_wr_byte(0xDA,OLED_CMD); //����COMӲ����������
	oled_wr_byte(0x12,OLED_CMD); //[5:4]����
		 
	oled_wr_byte(0x81,OLED_CMD); //�Աȶ�����
	oled_wr_byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	oled_wr_byte(0xD9,OLED_CMD); //����Ԥ�������
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	oled_wr_byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	oled_wr_byte(0xAF,OLED_CMD); //������ʾ	 
	oled_clear();
	
	oled_delay(500);
	oled_showPicture(25,10,bmp_logo_GOGO1,90,30);
	oled_refreshGram();
  oled_delay(500);
}

#ifdef __cplusplus
}
#endif










