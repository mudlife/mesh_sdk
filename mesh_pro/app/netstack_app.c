/**
 * @file netstack_app.c
 * @brief 应用层文件
 * @details 实现具体应用，例如灯功能 LED初始化，开关，亮度调节，色温调节
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 */
#include <string.h>
#include "network_conf.h"
#include "netstack_app.h"
//#include "pwm.h"
#include "os.h"

/**
 * @name PWM 引脚定义
 * @{
 */
#define C_VALUE_L     PWM0DL
#define C_VALUE_H     PWM0DH
#define W_VALUE_L     PWM0DTL
#define W_VALUE_H     PWM0DTH
/** @} */

/// ADC 引脚定义
#define PIN_AC				P0_0//根据引脚定义

u8 r_ac_dbs_in;
u8 r_ac_dbs_free;
u8 r_acpower_ready;

/// 可运行标志位
u8 r_runflag;   
/// 等待次数
u8 r_waittimes;
/**
 * @name PWM 需要的值和当前值
 * @{
 */
u16 r_c_needvalue;
u16 r_w_needvalue;
u16 r_c_nowvalue;
u16 r_w_nowvalue;
/** @} */

/// ?
u16 r_autosave;
/// 当前次数
u8 r_nowcount;

u8 r_runningflag;
/// 亮度比 %
char  r_bright;
/// 色温比 %
char r_tone;
///上电次数
u8 r_poweroncnt;
/// 应用层系统时间
u8 r_systemtime;
/// ?
u8 r_acin;

extern void net_clear_network();
extern void delay_ms(u16 t);
void led_flash(void);
void F_FlashFuntion(u8 x);

u8 flash_count;

///快捷键 对应的灯光模式
code u8 led_key_model_val[][2] = {
																		{0,100},
																		{50,100},
																		{100,100},
																		{0,50},
																		{50,50},
																		{50,25}};
/// led状态结构定义
Led_Sta_T * led_sta_t;


void F_FlashFuntion(u8 x);
																		
/**
 * @brief 设置PWM
 * 
 * @return 无
 */																																				
static void app_set_def_pwm()
{
	if(r_w_needvalue != r_w_nowvalue || r_c_needvalue != r_c_nowvalue){
		C_VALUE_H = (u8)((r_c_nowvalue>>8)&0X03);		
		C_VALUE_L = (u8)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (u8)((r_w_nowvalue>>8)&0X03);	
		W_VALUE_L = (u8)(r_w_nowvalue&0X00FF);
	}
}

/**
 * @brief 清网检测
 * 
 * @return 无
 */	
void F_CheckClearNet()
{
	r_poweroncnt = local_info.pweron;
	r_autosave = 8000;	
	if((r_poweroncnt==0xa0) || (r_poweroncnt>9))
	{
		r_bright = led_sta_t->led_bright;//获取灯的亮度
		r_tone = led_sta_t->led_tone;		//获取色温值
		led_sta_t->led_sta = 1;					//灯的状态设置为开灯  
		
		if(r_bright>100)
		{
			r_bright = 50;
		}
		if(r_tone>100)
		{
			r_tone = 50;
		}				
		r_poweroncnt = 0;
	}else{
		r_poweroncnt++;	
		
		if((r_poweroncnt%4==0)&&r_poweroncnt!=8)
		{
					r_tone = 100;
					r_bright = 100; 	
								
		}
		else if((r_poweroncnt%4==1))
		{
					r_tone = 0;
					r_bright = 100; 		
		}
		else if((r_poweroncnt%4==2))
		{
					r_tone = 50;
					r_bright = 100;				
		}
		else if((r_poweroncnt%4==3))
		{
					r_tone = 50;
					r_bright = 20; 				
		}	
		else if(r_poweroncnt==8)
		{
					r_tone = 100;
					r_bright = 100;
					F_SetBrightToneValue();	
					r_c_nowvalue = r_c_needvalue;
					r_w_nowvalue = r_w_needvalue;
					app_set_def_pwm();
		
					F_FlashFuntion(5);
					net_clear_network();	
		}		
	}

		local_info.pweron = r_poweroncnt;		
		net_updat_local_info();
		F_SetBrightToneValue();	
		r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;
		app_set_def_pwm();

		led_sta_t->led_sta = 1;					//灯的状态设置为开灯  	
//	F_SetBrightToneValue();		
}


/**
 * @brief 开关灯切换
 * 
 * @return 无
 */	
void F_SwitchOnOff()
{
	if(r_acpower_ready)
	{
		if(r_acin==0)
		{
			r_acin = 1;			
			PWM0EN = 0x0F;	
			F_CheckClearNet();
			r_runflag = 0;			
		}
	}
	else
	{
		r_acin = 0;
		r_runflag = 1;
		PWM0EN = 0x00;	
	}
}



/**
 * @brief 自动保存
 * 
 * @return 无
 */	
void F_Autosave()
{
	if((r_systemtime&0x01)==1)
	{
		r_systemtime = r_systemtime & 0xfe;
		if(r_autosave)
		{
			r_autosave--;
			if(r_autosave==0)
			{
				led_sta_t->led_bright  =  r_bright;
				led_sta_t->led_tone = r_tone;	
				
//	r_bright = led_sta_t->led_bright;//获取灯的亮度
//	r_tone = led_sta_t->led_tone;		//获取色温值				
				
				r_poweroncnt = 0xa0;	
				local_info.pweron = r_poweroncnt;		
				net_updat_local_info();				
			}
		}
	}
	
}

/**
 * @brief 闪灯
 * 
 * @param x 闪灯次数
 * @return 无
 */	
void F_FlashFuntion(u8 x)
{
		r_runflag = 1;
		while(x)
		{
			PWM0EN = 0x00;	
			WDTC |= 0x10;    //清狗
			delay_ms(250);
			PWM0EN = 0x0F;
			WDTC |= 0x10;    //清狗			
			delay_ms(250);	
			x--;
		}
		r_runflag = 0;
}




/**
 * @brief 应用层进程
 * 
 * @return 无
 * @note while循环调用该函数
 */	
void app_process(void)
{
//	F_SwitchOnOff();
//	F_Autosave();  //关闭
}


//void app_Bright()
//{
//	F_BrightUp();
//	os_task_add(30,app_Bright);
//}

 
/**
 * @brief 闪灯
 * 
 * @return 无
 */
 
void led_flash(void)
{
	if(led_sta_t->led_sta == 1){
		led_sta_t->led_sta = 0;
		PWM0EN = 0x08;
	}else{
		led_sta_t->led_sta = 1;
		PWM0EN = 0x0F;	
	}
	if(flash_count>0){
		flash_count--;
		os_task_add(500,led_flash);
	}
}

/**
 * @brief APP层初始化
 * 
 * @return 无
 */
u8 app_init(void)
{
	//1.初始化LED灯
	
	P0M0 = P0M0 & 0XF0;
	
	r_c_nowvalue = 00;
	r_w_nowvalue = 00;
	r_c_needvalue = 00;
	r_w_needvalue = 00;	
	r_acin = 0;
	PWM_Init();
	
	//获取设备状态广播参数的缓存区
	led_sta_t = (Led_Sta_T *)net_get_adv_param_buf();
	//设置LED的模式   默认为冷暖模式
	led_sta_t->led_model = CURRENT_LED_MODEL<<LED_MODEL_SHIFT;
	//设置设备广播参数的长度  长度小于11     当前模式为冷暖模式 ，设备广播参数为4个字节 led_model,led_sta,led_bright,led_tone
	net_set_adv_param_len(5);
//	r_bright = led_sta_t->led_bright;//获取灯的亮度
//	r_tone = led_sta_t->led_tone;		//获取色温值
	led_sta_t->led_sta = 1;					//灯的状态设置为开灯  
	


//	F_CheckClearNet();

r_tone = led_key_model_val[1][0];
r_bright = led_key_model_val[1][1];
	F_OpenLed();
//os_task_add(30,app_Bright);
	r_runflag = 0;
//	F_Countdata(r_bright,r_tone);
	
	return 0;
}

/**
 * @brief APP层处理来自网络层的数据包
 *
 * @param net_pdu 数据包
 * @return 返回值用来判断网络层是否发送设备网络广播
 *  @retval 0 不发送设备广播
 *  @retval 1 放设备广播
 * @note 由网络层调用，其他地方不能调用
 */
u8 app_input(NET_PDU *net_pdu)
{
	u8 keytemp = 0; 
	u8 res = 0;
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://闪灯   当设备入网，移除网络，配对，取消配对时会进行闪灯   ，移除网络对码的闪灯需要在2S内完成
			//net_pdu->dat.net_cmd.param[0]; 1---入网   2---移除网络   3---对码   4---取消对码
		flash_count = 5;
		if(led_sta_t->led_sta == 0)
		{
			led_sta_t->led_sta = 1;
			F_OpenLed();
		}
//		
//		F_FlashFuntion(3);		
		
		os_task_add(1,led_flash);
		break;
		case CTL_LED_ON://开灯
			led_sta_t->led_sta = 1;
			led_sta_t->ttl = net_pdu->ttl;
			F_OpenLed();
		res = 1;
		break;
		case CTL_LED_OFF://关灯
			led_sta_t->led_sta = 0;
			led_sta_t->ttl = net_pdu->ttl;
			F_CloseLed();
		res = 1;

		break;
		case CTL_BRIGHT_VAL://设置亮度
			r_bright = net_pdu->dat.net_cmd.param[1];
			led_sta_t->led_bright = r_bright;
			F_SetBrightToneValue();
		  
	
		break;
		case CTL_BRIGHT_UP://亮度+
			F_BrightUp();
		break;
		case CTL_BRIGHT_DOWN://亮度-
			F_BrightDown();
		break;
		case CTL_TONE_VAL://设置色温
			r_tone = net_pdu->dat.net_cmd.param[0];
			led_sta_t->led_tone = r_tone;
			F_SetBrightToneValue();
		break;
		case CTL_TONE_UP://色温+
			F_ToneUp();
		break;
		case CTL_TONE_DOWN://色温-
			F_ToneDown();
		break;
		case CTL_KEY_BOARD://快捷键
		led_sta_t->led_model &= 0xF0;
		led_sta_t->led_model |= net_pdu->dat.net_cmd.param[0];			
		keytemp = net_pdu->dat.net_cmd.param[0];
			r_waittimes = 0;	
			r_waittimes = 0;	
		  if(keytemp<6){
				r_tone = led_key_model_val[keytemp][0];
				r_bright = led_key_model_val[keytemp][1];
			}	
		F_SetBrightToneValue();	
		app_set_def_pwm();

		led_sta_t->led_sta = 1;
		break;
		case CTL_LED_RGB://RGB
			
		break;
		
	}
	
	led_sta_t->led_bright = r_bright;
	led_sta_t->led_tone = r_tone;		
//	r_poweroncnt = 0xa0;
//	local_info.pweron = r_poweroncnt;				
//	net_updat_local_info();		
	r_runningflag = 1;
	return res;
}	


/**
 * @brief APP层发送数据
 *
 * @param net_pdu 数据包
 * @return APP输出结果
 */

u8 app_output(NET_PDU *net_pdu)
{

	return 0;
}



/**
 * @brief 开灯
 * 
 * @return 无
 */
void F_OpenLed()
{
	r_waittimes = 1;	
	F_Countdata(r_bright,r_tone);

}
/**
 * @brief 关灯
 * 
 * @return 无
 */
void F_CloseLed()
{
	r_waittimes = 1;
	r_c_needvalue = 0;
	r_w_needvalue = 0;
}


/**
 * @brief 亮度加
 * 
 * @return 无
 */
void F_BrightUp()
{
			if(r_c_needvalue>=r_c_nowvalue || r_w_needvalue>=r_w_nowvalue)
			{
				if((r_c_needvalue - r_c_nowvalue)<50 || (r_w_needvalue - r_w_nowvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}		

					r_waittimes = 2;			
					F_Countdata(r_bright,r_tone);					
				}
			}
			
}

/**
 * @brief 亮度减
 * 
 * @return 无
 */
void F_BrightDown()
{
		if(r_c_needvalue<=r_c_nowvalue || r_w_needvalue<=r_w_nowvalue)
			{
				if((r_c_nowvalue - r_c_needvalue)<50 || (r_w_nowvalue - r_w_needvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<5)
					{
						r_bright = 5;
					}		
					r_waittimes = 2;			
					F_Countdata(r_bright,r_tone);					
				}
			}
					
}




/**
 * @brief 色温加
 * 
 * @return 无
 */
void F_ToneUp()
{

		if(r_c_needvalue>=r_c_nowvalue || r_w_needvalue>=r_w_nowvalue)
		{
				
				if((r_c_needvalue - r_c_nowvalue)<50 || (r_w_needvalue - r_w_nowvalue)<50)
				{
					r_waittimes = 2;
					r_tone = r_tone + 5;
					if(r_tone>100)
					{
						r_tone = 100;
					}		
					r_waittimes = 2;	
					F_Countdata(r_bright,r_tone);		
				}
		}

}
/**
 * @brief 色温减
 * 
 * @return 无
 */
void F_ToneDown()
{
	if(r_c_needvalue<=r_c_nowvalue || r_w_needvalue<=r_w_nowvalue)
		{
				
				if((r_c_nowvalue - r_c_needvalue)<50 || (r_w_nowvalue - r_w_needvalue)<50)
				{
					r_tone = r_tone - 5;
					if(r_tone<10)
					{
						r_tone = 0;
					}		
					r_waittimes = 2;	
					F_Countdata(r_bright,r_tone);		
				}
		}
}

/**
 * @brief 设置亮度和色温值
 * 
 * @return 无
 */
void F_SetBrightToneValue()
{
	r_waittimes = 0;		
	F_Countdata(r_bright,r_tone);	
}


/**
 * @brief ADC 检测
 * 
 * @return 无
 */
void F_AcPowerDect()
{
	if(PIN_AC)
	{
		r_ac_dbs_in++;
		r_ac_dbs_free = 0;
		if(r_ac_dbs_in>2)
		{
			r_acpower_ready = 1;
		}		
	}
	else
	{
		r_ac_dbs_free++;
		r_ac_dbs_in = 0;
		if(r_ac_dbs_free>30)
		{
			r_acpower_ready = 0;
		}						
	}
}







/**
 * @brief 应用层中断处理函数
 * 
 * @return 无
 * @note 用来跟新PWM
 */
void F_CWRunning(void)
{
		r_systemtime = 0xff;
//		F_AcPowerDect();
		if(r_runflag==0) //;r_runningflag
		{
			if(r_nowcount>=r_waittimes)
			{
				r_nowcount=0;
		
				if(r_c_needvalue>r_c_nowvalue)
				{
					r_c_nowvalue++;
				}
				else if(r_c_needvalue<r_c_nowvalue)
				{
					r_c_nowvalue--;					
				}
				if(r_w_needvalue>r_w_nowvalue)
				{
					r_w_nowvalue++;
				}
				else if(r_w_needvalue<r_w_nowvalue)
				{
					r_w_nowvalue--;					
				}				
			  
				app_set_def_pwm();


				if((r_c_needvalue==r_c_nowvalue)&&(r_w_needvalue==r_w_nowvalue))
				{
						r_runningflag = 0;					
				}				
			}
			else
			{
				r_nowcount++;
			}
		}
		
}



/**
 * @brief 根据亮度和色温比计算PWM
 * 
 * @param x_Bright 亮度百分比
 * @param y_Tone 色温百分比
 * @return 无
 * @note 要避免浮点型运算，比价耗费资源
 */
void F_Countdata(const u8 x_Bright,const u8 y_Tone)
{
	u32 temp1,temp2;
	u16 temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101)){
		//1.计算色温
		//计算公式  PWM1 + PWM2 = 100%
		temp1 = y_Tone*10;
		temp2 = 1000 - temp1;
		
		//2.计算亮度
		//计算公式 PWM * 亮度%

		temp3 = ( x_Bright*temp1/100);
		temp4 = (x_Bright*temp2/100);
		
		r_c_needvalue = temp4;  //暖光
		r_w_needvalue = temp3;	//冷光
	}
	
}





/**
 * @brief PWM初始化
 *
 * @return 无
 */
void PWM_Init(void)
{
	P2M1 = (P2M1&0x0F)|0xC0; //P23  
	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
	PWM01_MAP = 0x23;					//PWM01通道映射P23口
	PWM0_MAP = 0x01;					//PWM0通道映射P01口
	
//	P2M2 = (P2M2&0xF0)|0x08;
//	P2M3 = (P2M3&0xF0)|0x08;
//	PWM01_MAP = 0x26;					//PWM01通道映射P23口
//	PWM0_MAP = 0x24;					//PWM0通道映射P01口		
			
	
	PWM0C 			= 0X00;           //  128分频
	PWM0EN 			= 0x0F;			
	
	PWM0PH = 0X03;						
	PWM0PL = 0XE8;				
	
	PWM0DH = 0x00;   					
	PWM0DL = 0x00; 			
	PWM0DTH = 0x00;  					
	PWM0DTL = 0x00; 		
 

}

