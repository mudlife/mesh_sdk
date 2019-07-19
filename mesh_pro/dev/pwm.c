#include "HC89S003F4.h"
#include "pwm.h"
#include "delay.h"
#include "network_conf.h"

//data u8 pwm1;
//data u8 pwm2;

//data u8 r_val=5;
//data u8 g_val=5;
//data u8 b_val=5;
//data u8 w_val=5;
//data u8 y_val=5;
//data u8 wy_val=5;


//#define R_VAL 5
//#define G_VAL 5
//#define B_VAL 5
//#define W_VAL 5
//#define Y_VAL 5
//#define WY_VAL 5


//data led_color;
//data u8 led_sta=1;

//#define WHITE  1
//#define YELLOW 2
//#define W_AND_Y 3
//#define RED  4
//#define GREEN 5
//#define BLUE 6


/**
 * 功能：PWM初始化
 * P00 ,P01 ,P23 , P24 ,P25 
 */
void pwm_init(void)
{

	P2M1 = (P2M1&0x0F)|0xC0; //P23  
//	P2M2 = (P2M2&0xFF)|0xCC; //P24 , P25
	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
	
	
	PWM01_MAP = 0x23;					//PWM01通道映射P10口
	PWM0_MAP = 0x01;					//PWM0通道映射P22口
//	PWM1_MAP = 0x00;
//	PWM11_MAP = 0x24;
//	PWM2_MAP = 0x25;
	PWM0C = 0x03;						//PWM0高有效，PWM01高有效，时钟128分频 0X03
	PWM0EN = 0x0F;						//使能PWM0，工作于独立模式
	
//	
//  PWM0C = 0x00;						//PWM0高有效，PWM01高有效，时钟8分频 0X01
//	PWM0EN = 0x0F;						//使能PWM0，工作于独立模式
//	
//	
//	PWM1C = 0x00;						//PWM1高有效，PWM11高有效，时钟8分频 0X01
//	PWM1EN = 0x0F;						//使能PWM1，工作于独立模式
//	
//	PWM2C = 0x00;						//PWM2高有效，PWM11高有效，时钟8分频 0X01
//	PWM2EN = 0x0F;						//使能PWM1，工作于独立模式
	

	//独立模式下，PWM0和PWM01共用一个周期寄存器
	//PWM0的占空比调节使用			PWM0组的占空比寄存器
	//PWM01的占空比调节使用			PWM0组的死区寄存器

	//周期计算 	= 0x0064 / (Fosc / PWM分频系数)		（Fosc见系统时钟配置的部分）
	//			= 0x0064 / (16000000 / 128)			
	// 			= 125   /125000
	//			= 1000us		   		约1KHz

	PWM0PH = 0x00;						//周期高4位设置为0x03
	PWM0PL = 0x64;						//周期低8位设置为0xFF
	
	
//	PWM1PH = 0x01;						//周期高4位设置为0x03
//	PWM1PL = 0x40;						//周期低8位设置为0xFF
//	
//	
//	PWM2PH = 0x01;						//周期高4位设置为0x03
//	PWM2PL = 0x40;						//周期低8位设置为0xFF
	
	

	//占空比计算= 0x0155 / (Fosc / PWM分频系数)		（Fosc见系统时钟配置的部分）
	//			= 0x0155 / (16000000 / 8)			
	// 			= 341 	 / 2000000
	//			= 170.5us		   占空比为 170.5/511.5 = 33.3%

	PWM0DH = 0x00;						//PWM0高4位占空比0x05
	PWM0DL = 0x0B;					//PWM0低8位占空比0x55
	PWM0DTH = 0x00;						//PWM01高4位占空比0x01
	PWM0DTL = 0x0B;						//PWM01低8位占空比0x55
	
//	PWM1DH = 0x00;						//PWM0高4位占空比0x05
//	PWM1DL = 0xC8;						//PWM0低8位占空比0x55
//	PWM1DTH = 0x00;						//PWM01高4位占空比0x01
//	PWM1DTL = 0xC8;						//PWM01低8位占空比0x55
//	
//	
//	PWM2DH = 0x00;						//PWM0高4位占空比0x05
//	PWM2DL = 0xC8;						//PWM0低8位占空比0x55
//	PWM2DTH = 0x00;						//PWM01高4位占空比0x01
//	PWM2DTL = 0xC8;						//PWM01低8位占空比0x55
	
//	pwm1 = 0x30;
//	pwm2 = 0x30;
//	pwm_off();
}


//void pwm_trig(void)
//{
//	if(led_sta == 1){
//		pwm_off();
//	}else{
//		pwm_on();
//	}
////	if(PWM0EN == 0x0F){
////	PWM0EN = 0x08;						//失能PWM0
////	}else{
////	PWM0EN = 0x0F;						//使能PWM0，工作于独立模式
////	}
//}


void pwm_on(void)
{
//	u8 i;
//	u8 p1=5,p2=5;
	local_info.led_sta = 1;
//	for(i=0;i<100;i++){
//			p2 = pwm2*i/100;
//			p1 = pwm1*i/100;
//			PWM0DH = 0;						//PWM0高4位占空比0x05
//			PWM0DL = p1;						//PWM0低8位占空比0x55
//			
//			PWM0DTH = 0;						//PWM01高4位占空比0x01
//			PWM0DTL = p2;						//PWM01低8位占空比0x55
//			delay_ms(10);
//	}
//	
//		PWM0DH = 0;						//PWM0高4位占空比0x05
//		PWM0DL = pwm1;						//PWM0低8位占空比0x55
//		
//		PWM0DTH = 0;						//PWM01高4位占空比0x01
//		PWM0DTL = pwm2;						//PWM01低8位占空比0x55
	
	
	
	PWM0EN = 0x0F;//0X0F

}

void pwm_off(void)
{
	
//	u8 i=1;
//	u8 p1=5,p2=5;
	local_info.led_sta = 0;
//		for(i=0;i<100;i++){
//			p2 = pwm2 - pwm2*i/100+5;
//			p1 = pwm1 - pwm1*i/100+5;
//			PWM0DH = 0;						//PWM0高4位占空比0x05
//			PWM0DL = p1;						//PWM0低8位占空比0x55
//			
//			PWM0DTH = 0;						//PWM01高4位占空比0x01
//			PWM0DTL = p2;						//PWM01低8位占空比0x55
//			delay_ms(10);
//		}


//	
//		PWM0DH = 0;						//PWM0高4位占空比0x05
//		PWM0DL = 5;						//PWM0低8位占空比0x55
//		
//		PWM0DTH = 0;						//PWM01高4位占空比0x01
//		PWM0DTL = 5;						//PWM01低8位占空比0x55
//	
	
	PWM0EN = 0x08;	
}

//void pwm_set(u8 led)
//{
//	if(led){
//		pwm_on();
//	}else{
//		pwm_off();
//	}
//}
//void pwm_set_wy(u8 w,u8 y)
//{
//	data u16 v;
//	pwm1 = w;
//	pwm2 = y;
//	pwm_off();
//	v = (u8)w*2.5;
//	if(v < 2) v = 2;
//	if(v >75) v = 75;
//	PWM0DH = v>>8&0xFF;						//PWM0高4位占空比0x05
//	PWM0DL = v&0xFF;						//PWM0低8位占空比0x55
//	v = (u8)y*2.5 ;
//	if(v < 2) v = 2;
//	if(v >75) v = 75;
//	PWM0DTH = v>>8&0xFF;						//PWM01高4位占空比0x01
//	PWM0DTL = v&0xFF;						//PWM01低8位占空比0x55
//	pwm_on();
//}




//void pwm_set_w(u8 w)
//{
//	data u16 v;
//	w_val = w;
//	PWM0EN = 0x08;
//	v = (u8)w*2.5;
//	if(v >250) v = 250;
//	PWM0DH = v>>8&0xFF;						//PWM0高4位占空比0x05
//	PWM0DL = v&0xFF;						//PWM0低8位占空比0x55
//	PWM0EN = 0x0F;
//}

//void pwm_set_y(u8 y)
//{
//	data u16 v;
//	y_val = y;
//	PWM0EN = 0x08;
//	v = (u8)y*2.5;
//	if(v >250) v = 250;
//	PWM0DTH = v>>8&0xFF;						//PWM01高4位占空比0x01
//	PWM0DTL = v&0xFF;						//PWM01低8位占空比0x55
//	PWM0EN = 0x0F;
//}

//void pwm_set_rgb(u8 r,u8 g,u8 b)
//{
////	data u16 v;
////	r_val = r;
////	g_val = g;
////	b_val = b;
////	//pwm_off();
////	wy_off();
////	PWM1EN = 0x0F;
////	PWM2EN = 0x0F;
////	delay_ms(2);
//	
//	if(r!=0)  led_color=RED;
//	else if(g!=0)  led_color=GREEN;
//	else if(b!=0)  led_color=BLUE;
////	
////	v = (u8)r*2.5;
////	if(v >250) v = 250;
////	PWM1DH = v>>8&0xFF;						//PWM0高4位占空比0x05
////	PWM1DL = v&0xFF;						//PWM0低8位占空比0x55
////	v = (u8)g*2.5;
////	if(v >250) v = 250;
////	PWM1DTH = v>>8&0xFF;						//PWM01高4位占空比0x01
////	PWM1DTL = v&0xFF;						//PWM01低8位占空比0x55
////	v = (u8)b*2.5;
////	if(v >250) v = 250;
////	
////	PWM2DH =  v>>8&0xFF;						//PWM0高4位占空比0x05
////	PWM2DL = v&0xFF;						//PWM0低8位占空比0x55
////	
////	PWM1EN = 0x0F;
////	PWM2EN = 0x0F;
//}


//void pwm_set_default(u8 model)
//{
//		switch(model){
//				case LED_MODEL_R:
//					pwm_set_rgb(R_VAL,0,0);
//				break;
//				case LED_MODEL_G:
//					pwm_set_rgb(0,G_VAL,0);
//				break;
//				case LED_MODEL_B:
//					pwm_set_rgb(0,0,B_VAL);
//				break;
//				case LED_MODEL_W:
//					pwm_set_wy(0,W_VAL);
//				break;
//				case LED_MODEL_Y:
//					pwm_set_wy(Y_VAL,0);
//				break;
//				case LED_MODEL_WY:
//					pwm_set_wy(Y_VAL,W_VAL);
//				break;
//				
//				
//			}
//}


