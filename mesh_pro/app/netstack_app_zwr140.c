#include <string.h>
#include "network_conf.h"
#include "netstack_app.h"
//#include "pwm.h"
#include "os.h"





#define   LED_MODEL_WY      0x00     //冷暖模式
#define   LED_MODEL_RGB     0x01		//RGB模式 
#define   LED_MODEL_WY_RGB  0x02 //冷暖RGB共存模式



#define   CURRENT_LED_MODEL	 LED_MODEL_WY_RGB  //当前LED模式

#define		LED_MODEL_SHIFT		(4)

/**
 * @name 应用层操作码
 * @{
 */
#define CTL_LED_FLASH			0X40	///<闪灯
#define CTL_LED_ON				0X41  ///<开灯
#define CTL_LED_OFF				0X42  ///<关灯

#define CTL_BRIGHT_VAL		0X43	///<设置亮度
#define CTL_BRIGHT_UP			0X44  ///<亮度+
#define CTL_BRIGHT_DOWN		0X45  ///<亮度-

#define CTL_TONE_VAL			0X46	///<设置色温
#define CTL_TONE_UP				0X47	///<色温+
#define CTL_TONE_DOWN			0X48	///<色温-

#define CTL_KEY_BOARD			0X49  ///<快捷键

#define CTL_LED_RGB				0X4A  ///<RGB颜色

#define CTL_RGB_OFF 			0X4B	///<关闭RGB
#define CTL_NIGHT					0X4C	///<夜灯
#define CTL_TIMING				0X4D	///<定时
#define CTL_COUNT_DOWN		0X4E	///<倒计时
#define CTL_COLOUR_TEMP		0X51	///<色温开启状态
#define CTL_MUSIC					0X52	///<音乐开启状态
#define CTL_RGB_RUN				0X61	///<RGB变换
#define CTL_RGB_SWITCH		0X62	///<RGB切换
#define CTL_WY_SWITCH			0X63	///<冷暖切换
#define	CTL_MEMORY_SAVE		0X64	///<记忆保存
#define CTL_MEMORY_READ		0X65	///<记忆读取


#define CTL_CMD_MAX 			0X66  ///<最大指令


/** @} */


/**
 * @name RGB模式
 * @{
 */
 #define RGB_MODEL_OFF				0x00
 #define RGB_MODEL_CONSTANT		0X01
 #define RGB_MODEL_LIN				0X02
 #define RGB_MODEL_LIN_JUMP		0X03
 #define RGB_MODEL_SIN				0X04
 #define RGB_MODEL_BREATHING		0X05
 #define RGB_MODEL_RGB				0X06
 #define RGB_MODEL_LINE_1			0X07
/** @} */




struct rgb_model_constant{
	u16 rgb_time;
};

struct rgb_model_line{
	u16 k;
};

struct rgb_model_line_jump{
	u16 k;
	u16 a;
};

struct rgb_model_sin{
	u16 k;
	u16 a;
	u16 h;
};

struct rgb_model_breathing{
	u16 rgb_time_k;
	u8 rgb_time_a;
	u8 rgb_h;
	u8 jump_a;
};


struct rgb_model_rgb{
	u8 r;
	u8 g;
	u8 b;
	
};

typedef struct rgb_model_param{
	u8 rgb_model;
	u8 rgb_model_old;
	u8 percentage_k;
	union{
		struct rgb_model_constant constant_val;
		struct rgb_model_line	line_val;
		struct rgb_model_line_jump line_jump_val;
		struct rgb_model_sin sin_val;
		struct rgb_model_breathing breath_val;
		struct rgb_model_rgb rgb_val;
	}model_pram;
}RGB_Model_Param;


/** 
 * @brief LED灯设备广播参数结构体
 */
//LED灯设备广播参数结构体
typedef struct led_sta_t{
	u8 led_model;  //模式
	u8 led_sta;
	u8 led_bright; // 亮度
	u8 led_tone;	 //色温
	u8 led_start_cnt;
}Led_Sta_T;



#define C_VALUE_L     PWM0DL
#define C_VALUE_H     PWM0DH
#define W_VALUE_L     PWM0DTL
#define W_VALUE_H     PWM0DTH

#define PIN_AC				P0_2//根据引脚定义

unsigned char r_ac_dbs_in;
unsigned char r_ac_dbs_free;
unsigned char r_acpower_ready;

unsigned char r_runflag;
unsigned char r_waittimes;
unsigned short r_c_needvalue;
unsigned short r_w_needvalue;
unsigned short r_c_nowvalue;
unsigned short r_w_nowvalue;
unsigned short r_autosave;

unsigned char r_nowcount;
unsigned char r_runningflag;
unsigned char  r_bright;
unsigned char r_tone;
unsigned char r_tone;
unsigned char r_poweroncnt;
unsigned char r_systemtime;

extern void net_clear_network();
extern void delay_ms(u16 t);
void led_flash(void);
void F_FlashFuntion(u8 x);

unsigned char flash_count;
unsigned char r_acin;


Led_Sta_T * led_sta_t;


static void F_FlashFuntion(u8 x);
static void F_CheckClearNet(void);
static void PWM_Init(void);
static void F_Countdata(unsigned char x_Bright,unsigned char y_Tone);
static void F_AcPowerDect(void);
static void F_SetBrightToneValue(void);
static void F_ToneDown(void);
static void F_ToneUp(void);
static void F_BrightDown(void);
static void F_BrightUp(void);
static void F_CloseLed(void);
static void F_OpenLed(void);

void F_CheckClearNet(void)
{
	r_poweroncnt = local_info.pweron;
	if(BORF)
	{
		r_poweroncnt=0;
		BORF=0;
	}
	r_autosave = 8000;//5000;	
	if((r_poweroncnt==0x00) || (r_poweroncnt>9))
	{
		//r_bright = led_sta_t->led_bright;//获取灯的亮度
		//r_tone = led_sta_t->led_tone;		//获取色温值
		//led_sta_t->led_sta = 1;					//灯的状态设置为开灯  
		
		r_tone = 50;
		r_bright = 100; 
		r_poweroncnt = 1;
	}
	else if((r_poweroncnt==1)||(r_poweroncnt==5))
	{
				r_tone = 0;
				r_bright = 100; 	
				r_poweroncnt++;				
	}
	else if((r_poweroncnt==2)||(r_poweroncnt==6))
	{
				r_tone = 100;
				r_bright = 100; 		
				r_poweroncnt++;		
	}
	else if((r_poweroncnt==3)||(r_poweroncnt==7))
	{
				r_tone = 50;
				r_bright = 5;
				r_poweroncnt++;				
	}
	else if((r_poweroncnt==4)||(r_poweroncnt==8))
	{
				r_tone = 50;
				r_bright = 100; 		
				r_poweroncnt++;				
	}	
	else if(r_poweroncnt==9)
	{
				r_tone = 50;
				r_bright = 100;
				F_SetBrightToneValue();	
				r_c_nowvalue = r_c_needvalue;
				r_w_nowvalue = r_w_needvalue;
				C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);		
				C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
				W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);	
				W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);
	
				r_poweroncnt = 1;
				local_info.pweron = r_poweroncnt;	
        delay_ms(10);		
				net_updat_local_info();
		    delay_ms(10);
				F_FlashFuntion(5);
		
				net_clear_network();
				r_poweroncnt = 1;		
	}		
	
		local_info.pweron = r_poweroncnt;	
	  led_sta_t->led_start_cnt = r_poweroncnt;
	  delay_ms(10);
		net_updat_local_info();
	  delay_ms(10);
		F_SetBrightToneValue();	
		r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;
		C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);	
		C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
		W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

		led_sta_t->led_sta = 1;		//灯的状态设置为开灯  	
//	F_SetBrightToneValue();		
}



void F_SwitchOnOff()
{
	if(r_acpower_ready)
	{
		if(r_acin==0)
		{
			r_acin = 1;			
			//PWM0EN = 0x0F;	
			F_CheckClearNet();		
	    PWM0EN 	= 0x0F;			
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
				
				led_sta_t->led_bright =  r_bright;
				led_sta_t->led_tone   =  r_tone;	
	
//				r_poweroncnt = 00;	
//				local_info.pweron = r_poweroncnt;		
//				net_updat_local_info();				
			}
		}
	}
	
}

static void F_FlashFuntion(u8 x)
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



//while循环调用该函数
void app_process(void)
{
	F_SwitchOnOff();
	F_Autosave();  //关闭
}












//while循环调用该函数
//void app_process(void)
//{
//	F_AcPowerDect();  //关闭
//}

/**
 *  功能：闪灯
 */
 /*
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
*/
/**
 * 功能：APP层初始化
 * 参数：无
 * 返回：无
 */
u8 app_init(void)
{
	//1.初始化LED灯
	
	P0M1 = P0M1 & 0XF0;
	
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
//	led_sta_t->led_sta = 1;					//灯的状态设置为开灯  
	
	
	

//	F_CheckClearNet();


	r_runflag = 0;
	
	return 0;
}

/**
 * 功能：APP层处理来自网络层的数据包
 * 参数：wl_udp---数据包
 * 返回：成功返回0
 * 备注：由网络层调用，其他地方不能调用
 */
//static u32 e=0;

u8 app_input(NET_PDU *net_pdu)
{
	u8 keytemp = 0; 
	u8 res = 0;
	if(net_pdu->dat.net_cmd.param[1] != local_info.uuid[1]){
		return 0;
	}
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://闪灯   当设备入网，移除网络，配对，取消配对时会进行闪灯   ，移除网络对码的闪灯需要在2S内完成
			//net_pdu->dat.net_cmd.param[0]; 1---入网   2---移除网络   3---对码   4---取消对码
//		flash_count = 5;
		if(led_sta_t->led_sta == 0)
		{
			led_sta_t->led_sta = 1;
			F_OpenLed();
		}
		
		F_FlashFuntion(3);		
		
//		os_task_add(1,led_flash);
		break;
		case CTL_LED_ON://开灯
			led_sta_t->led_sta = 1;
			F_OpenLed();
				r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;	
		res = 1;
		break;
		case CTL_LED_OFF://关灯
			led_sta_t->led_sta = 0;
			F_CloseLed();
			r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;		
		res = 1;

		break;
		case CTL_BRIGHT_VAL://设置亮度
			r_bright = net_pdu->dat.net_cmd.param[3];
			led_sta_t->led_bright = r_bright;
			F_SetBrightToneValue();
		  
		break;
		case CTL_BRIGHT_UP://亮度+
			F_BrightUp();
//			P0 = P0 ^ 0X01;
		break;
		case CTL_BRIGHT_DOWN://亮度-
			F_BrightDown();
		
		break;
		case CTL_TONE_VAL://设置色温
			r_tone = net_pdu->dat.net_cmd.param[2];
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
		led_sta_t->led_model |= net_pdu->dat.net_cmd.param[2];			
		keytemp = net_pdu->dat.net_cmd.param[2];
			r_waittimes = 0;	
			r_waittimes = 0;	
			if(keytemp==0)//暖光
			{
				r_tone = 0;     
				r_bright = 100; //100% 亮度			
			}
			else if(keytemp==1)//中性光
			{
				r_tone = 50;
				r_bright = 100; //50% 亮度				
			}
			else if(keytemp==4)//辅助光
			{
				r_tone = 50;
				r_bright = 30; //70% 亮度			
			}
			else if(keytemp==5)//小夜灯
			{
				r_tone = 50;
				r_bright = 10; // 20% 亮度	
			}
			else if(keytemp==2)//冷光
			{
				r_tone = 100;
				r_bright = 100; //100% 亮度		
			}		
			else if(keytemp==3)//冷光
			{
				r_tone = 50;
				r_bright = 50; //100% 亮度		
			}					
		F_SetBrightToneValue();	
			r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;
		C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);			
		C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
		W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

		led_sta_t->led_sta = 1;
		break;
		case CTL_LED_RGB://RGB
			
		break;
		
	}
	
	led_sta_t->led_bright = r_bright;
	led_sta_t->led_tone = r_tone;		
	r_poweroncnt = 0x00;
	local_info.pweron = r_poweroncnt;				
	net_updat_local_info();		
	r_runningflag = 1;
//	res = 1;
	return res;
}	


/**
 * 功能：APP层发送数据
 * 参数：wl_udp---数据包
 */
/*
u8 app_output(NET_PDU *net_pdu)
{

	return 0;
}
*/


//开灯
static void F_OpenLed(void)
{
	r_waittimes = 0;	
	F_Countdata(r_bright,r_tone);

}
//关灯
static void F_CloseLed(void)
{
	r_waittimes = 0;
	r_c_needvalue = 0;
	r_w_needvalue = 0;

//	net_updat_local_info();				
}

/********************************************************************************
**	void F_BrightUp()
**	设置亮度 + 

**	输入以下变量值，改变亮度和色温
		r_bright
		r_tone

**********************************************************************************/
static void F_BrightUp(void)
{
	if(r_runningflag==0)
	{
		if(r_bright<100)
		{
			r_bright = r_bright + 5;
			if(r_bright>100)
			{
				r_bright = 100;
			}		
		}
		else
		{
				r_bright = 100;			
		}				
	}
	else
	{
		if(r_c_needvalue!=r_c_nowvalue)
		{
			if(r_c_needvalue>r_c_nowvalue)
			{
				if((r_c_needvalue - r_c_nowvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}
			}
			else 
			{
				if((r_c_nowvalue - r_c_needvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}				
			}
		}
		if(r_w_needvalue!=r_w_nowvalue)
		{
			if(r_w_needvalue>r_w_nowvalue)
			{
				if((r_w_needvalue - r_w_nowvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}
			}			
			else 
			{
			if((r_w_nowvalue - r_w_needvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}				
			}
		}		
	}	
			r_waittimes = 2;			
			F_Countdata(r_bright,r_tone);				
}
/********************************************************************************
**	void F_BrightDown()
**	设置亮度 - 

**	输入以下变量值，改变亮度和色温
		r_bright
		r_tone

**********************************************************************************/
static void F_BrightDown(void)
{
	if(r_runningflag==0)
	{
		if(r_bright>10)
		{
			r_bright = r_bright - 5;
			if(r_bright<10)
			{
				r_bright = 10;
			}		
		}
		else
		{
				r_bright = 10;			
		}				
	}
	else
	{
		if(r_c_needvalue!=r_c_nowvalue)
		{
			if(r_c_needvalue>r_c_nowvalue)
			{
				if((r_c_needvalue - r_c_nowvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}
			}
			else 
			{
				if((r_c_nowvalue - r_c_needvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}				
			}			
			
			
		}
		if(r_w_needvalue!=r_w_nowvalue)
		{
			if(r_w_needvalue>r_w_nowvalue)
			{
				if((r_w_needvalue - r_w_nowvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}
			}
			else 
			{
				if((r_w_nowvalue - r_w_needvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}				
			}			
			
		}		
	}	
			r_waittimes = 2;			
			F_Countdata(r_bright,r_tone);				
}




/*
void F_BrightDown()
{
	if(r_runningflag==0)
	{
		if(r_bright>5)
		{
			r_waittimes = 2;
			r_bright = r_bright-10;
			if(r_bright<10)
			{
						r_bright = 10;	
			}
			F_Countdata(r_bright,r_tone);					
		}
		else
		{
			if(r_bright>100)
			{
				r_bright = 100;
			}
		}
		
	}
}
/********************************************************************************
**	void F_ToneUp()
**	设置色温 + 

**	输入以下变量值，改变亮度和色温
		r_bright
		r_tone

**********************************************************************************/
static void F_ToneUp(void)
{
	if(r_runningflag==0)
	{
		if(r_tone<100)
		{
			r_waittimes = 2;
			r_tone = r_tone + 10;
			if(r_tone>100)
			{
				r_tone = 100;
			}		
		}
		else
		{
				r_tone = 100;
		}
			F_Countdata(r_bright,r_tone);					
	}
}
/********************************************************************************
**	void F_ToneDown()
**	设置色温- 

**	输入以下变量值，改变亮度和色温
		r_bright
		r_tone

**********************************************************************************/
static void F_ToneDown(void)
{
	if(r_runningflag==0)
	{
		if(r_tone>=10)
		{
			r_waittimes = 2;
			r_tone = r_tone-10;
			if(r_tone<10)
			{
				r_tone = 0;
			}		
		}
		else 
		{
				r_tone = 0;			
		}
			F_Countdata(r_bright,r_tone);				
	}
}
/********************************************************************************
**	void F_SetBrightToneValue()
**	设置色温 亮度 

**	输入以下变量值，改变亮度和色温
		r_bright
		r_tone

**********************************************************************************/
static void F_SetBrightToneValue(void)
{
	r_waittimes = 0;		
	F_Countdata(r_bright,r_tone);	
}

/********************************************************************************
**	void F_AcPowerDect()
**	交流电检测
**	放1ms中断
**	r_acpower_ready = 1 有交流输入
**	r_acpower_ready = 0 无交流输入
**	需要定义以下全局变量
**	unsigned char r_ac_dbs_in;
**	unsigned char r_ac_dbs_free;
**	unsigned char r_acpower_ready;
**********************************************************************************/

static void F_AcPowerDect(void)
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








void F_CWRunning(void)
{
		r_systemtime = 0xff;
		F_AcPowerDect();
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
				
				C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);				
				C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
				W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
				W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

//				if((r_c_nowvalue!=0)||(r_w_nowvalue!=0))
//				{
//					PWM0C 			= 0X03;           //  128分频
//					PWM0EN 			= 0x0F;			
//				}
//				else
//				{
//					PWM0C 			= 0;           //  128分频
//					PWM0EN 			= 0;			
//				}
				
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



/********************************************************************************
**	void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
**	output ram 
**	r_c_nowvalue = temp3;
**	r_w_nowvalue = temp4;	
		需要定义以下全局变量
**	input ram 
**	r_bright; 亮度值范围  0 -- 100 
**	r_tone;	  色温值范围  0 -- 100 
**********************************************************************************/
/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101))
	{
		temp1 = (((float)x_Bright)/100)*0x3ff;
		if(y_Tone==50) 
		{
			temp3 = (unsigned short)temp1;  
			temp4 = (unsigned short)temp1;			
		}
		else if(y_Tone==0)
		{
			temp4 = 0;  
			temp3 = (unsigned short)temp1*2;							
		}
		else if(y_Tone==100)
		{
			temp3 = 0;  
			temp4 = (unsigned short)temp1*2;					
		}	
		
		else if(y_Tone<50)
		{
			temp1 = (((float)x_Bright)/100)*0x3ff;
			temp3 = (unsigned short)temp1*2;
			if(temp3>0x3ff)
			{
				temp3 = 0x3ff;
			}
			temp1 = (((float)y_Tone)/100)*temp3;		
			temp2 = (float)((100-y_Tone)/100)*temp3;				
			temp3 = (unsigned short)temp2;
			temp4 = (unsigned short)temp1	;					
		}
		else
		{
			temp1 = (((float)x_Bright)/100)*0x3ff;
			temp3 = (unsigned short)temp1*2;
			if(temp3>0x3ff)
			{
				temp3 = 0x3ff;
			}
			temp1 = (((float)y_Tone)/100)*temp3;		
			temp2 =  (float)((100-y_Tone)/100)*temp3;				
			temp3 = (unsigned short)temp2;
			temp4 = (unsigned short)temp1	;		
		}
			
		if(temp3>=0x3ff)
		{
			temp3 = 0x3ff;
		}
		if(temp4>0x3ff)
		{
			temp4 = 0x3ff;
		}	
	//	local_info.led_bright = temp3;
	//	local_info.led_tone = temp4;

		r_c_needvalue = temp3;  //暖光
		r_w_needvalue = temp4;	//冷光
	}
}

*/




/********************************************************************************
**	void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
**	output ram 
**	r_c_nowvalue = temp3;
**	r_w_nowvalue = temp4;	
		需要定义以下全局变量
**	input ram 
**	r_bright; 亮度值范围  0 -- 100 
**	r_tone;	  色温值范围  0 -- 100 
**********************************************************************************/
/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101)){
		//1.计算色温
		//计算公式  PWM1 + PWM2 = 100%
		temp1 = y_Tone*511/100;
		temp2 = 511- temp1;
		
		//2.计算亮度
		//计算公式 PWM * 亮度%
		temp3 = temp1*x_Bright/100;
		temp4 = temp2*x_Bright/100;
		
		r_c_needvalue = temp4;  //暖光
		r_w_needvalue = temp3;	//冷光
	}
	
}


void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	
	if(y_Tone<=50)
	{
		temp1 = (((float)x_Bright)/100)*511;
		temp2 = (((float)y_Tone)/50)*(temp1);		
		temp4 = (unsigned short)temp2;
		temp3 = (unsigned short)(temp1 + (temp1 - temp2))	;			
		
	}
	else
	{
		temp1 = ((100-(float)y_Tone)/50)*((((float)x_Bright)/100)*511);		
		temp2 = (((float)x_Bright)/100)*511;		
		temp3 = (unsigned short)temp1;
		temp4 = (unsigned short)(temp2 + (temp2 - temp1))	;		
	}
		
	if(temp3>=511)
	{
		temp3 = 511;
	}
	if(temp4>511)
	{
		temp4 = 511;
	}	
//	local_info.led_bright = temp3;
//	local_info.led_tone = temp4;

	r_c_needvalue = temp3;  //暖光
	r_w_needvalue = temp4;	//冷光
		
}










*/

static void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	
	if(y_Tone<=50)
	{
		temp1 = (((float)x_Bright)/100)*1023;
		temp2 = (((float)y_Tone)/50)*(temp1);		
		temp4 = (unsigned short)temp2;
		temp3 = (unsigned short)(temp1 + (temp1 - temp2))	;			
		
	}
	else
	{
		temp1 = ((100-(float)y_Tone)/50)*((((float)x_Bright)/100)*1023);		
		temp2 = (((float)x_Bright)/100)*1023;		
		temp3 = (unsigned short)temp1;
		temp4 = (unsigned short)(temp2 + (temp2 - temp1))	;		
	}
		
	if(temp3>=1023)
	{
		temp3 = 1023;
	}
	if(temp4>1023)
	{
		temp4 = 1023;
	}	
//	local_info.led_bright = temp3;
//	local_info.led_tone = temp4;

	r_c_needvalue = temp3;  //暖光
	r_w_needvalue = temp4;	//冷光
		
}



/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101)){
		//1.计算色温
		//计算公式  PWM1 + PWM2 = 100%
		temp1 = y_Tone*1023.0/100;
		temp2 = 1023- temp1;
		
		//2.计算亮度
		//计算公式 PWM * 亮度%
		temp3 = (unsigned short)(temp1*x_Bright/100);
		temp4 = (unsigned short)(temp2*x_Bright/100);
		
		r_c_needvalue = temp4;  //暖光
		r_w_needvalue = temp3;	//冷光
	}
	
}

*/




static void PWM_Init(void)
{
//	P2M1 = (P2M1&0x0F)|0xC0; //P23  
//	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
//	PWM01_MAP = 0x23;					//PWM01通道映射P23口
//	PWM0_MAP = 0x01;					//PWM0通道映射P01口
//	P2M2 = (P2M2&0xF0)|0x08;
//	P2M3 = (P2M3&0xF0)|0x08;
//	PWM01_MAP = 0x26;					//PWM01通道映射P23口
//	PWM0_MAP = 0x24;					//PWM0通道映射P01口		
//	P2M3 = (P2M3&0x0F)|0x80;
//	P1M0 = (P1M0&0xF0)|0x08;
//	PWM01_MAP = 0x27;					//PWM01通道映射P23口
//	PWM0_MAP = 0x10;					//PWM0通道映射P01口				
	
	PWM0PH = 0X03;						
	PWM0PL = 0XFF;				
	
	PWM0DH = 0;   					
	PWM0DL = 0; 			
	PWM0DTH = 0;  					
	PWM0DTL = 0; 	

//	P2M1 = (P2M1&0x0F)|0xC0; //P23  
//	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
	
	P0M0 = 0x88;
	PWM01_MAP = 0x01;					//PWM01通道映射P23口
	PWM0_MAP = 0x00;					//PWM0通道映射P01口	
  P0_0 = 0;
	P0_1 = 0;
  PWM0C 			= 0X01;           //  128分频
	//PWM0EN 			= 0x0F;			

}

