/**
 * @file netstack_app_light_infrared.c
 * @brief 应用层文件
 * @details 实现具体应用，灯应用，带红外遥控,LED初始化，开关，亮度调节，色温调节
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-09-06
 */
#include <string.h>
#include <math.h>
#include "network_conf.h"
#include "netstack_app.h"
#include "flash.h"
#include "os.h"
#include "uart.h"


#if (UUID == UUID_LIGHT)

#define LED_WY		1
#define LED_RGB		0

#endif


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
typedef struct led_sta_t{
	u8 led_model;  ///<LED模式
	u8 led_sta;     ///<LED状态
	u8 led_bright; ///<亮度
	u8 led_tone;	 ///<色温
#if LED_RGB
	RGB_Model_Param rgb_model_dat;
#endif
}Led_Sta_T;


#define REVERSE	 0  //IO反向

/**
 * @name 灯颜色 PWM 寄存器定义
 * @{
 */
#define C_VALUE_L     PWM0DL		///< 暖光
#define C_VALUE_H     PWM0DH		///< 暖光
#define W_VALUE_L     PWM0DTL		///< 冷光
#define W_VALUE_H     PWM0DTH		///< 冷光
#define R_VALUE_L			PWM1DL		///< 红光
#define R_VALUE_H			PWM1DH		///< 红光
#define G_VALUE_L			PWM1DTL		///< 绿光
#define G_VALUE_H			PWM1DTH		///< 绿光
#define B_VALUE_L			PWM2DL		///< 蓝光
#define B_VALUE_H			PWM2DH		///< 蓝光
/** @} */


/**
 * @name 灯颜色 PWM 引脚定义
 * @{
 */
 #define C_PIN		0x01   ///<P01  暖光
 #define W_PIN		0x23   ///<P23  冷光
 #define R_PIN		0x26	 ///<P27  红光
 #define G_PIN		0x25	 ///<P25  绿色
 #define B_PIN		0x24	 ///<P24  蓝光
// #define C_PIN		0x00   ///<P01  暖光
// #define W_PIN		0x11   ///<P23  冷光
// #define R_PIN		0x01	 ///<P27  红光
// #define G_PIN		0x02	 ///<P25  绿色
// #define B_PIN		0x03	 ///<P24  蓝光
 /** @} */
 
 
 /**
 * @name PWM 引脚映射
 * @{
 */
 #define C_PIN_MAP		PWM0_MAP   	///<P01  暖光
 #define W_PIN_MAP		PWM01_MAP   ///<P23  冷光
 #define R_PIN_MAP		PWM1_MAP	 	///<P27  红光
 #define G_PIN_MAP		PWM11_MAP		///<P25  绿色
 #define B_PIN_MAP		PWM2_MAP	 	///<P24  蓝光
 /** @} */

/// ADC 引脚定义
#define PIN_AC				P0_0//根据引脚定义

#if defined(APP_ADC)

u8 r_ac_dbs_in;
u8 r_ac_dbs_free;
u8 r_acpower_ready;
#endif
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
#if LED_RGB
data u16 r_r_needvalue;
data u16 r_g_needvalue;
data u16 r_b_needvalue;

data u16 r_r_nowvalue;
data u16 r_g_nowvalue;
data u16 r_b_nowvalue;
#endif
/** @} */

/// ?
//u16 r_autosave;
/// 当前次数
u8 r_nowcount;

u8 r_runningflag;
/// 亮度比 %
char  r_bright;
/// 色温比 %
char r_tone;
#if LED_RGB
u8  r_color_r;
u8  r_color_g;
u8  r_color_b;
#endif

extern data u16 g_time;
extern u8 uart_rx_dat;  ///<串口数据

float g_tmp;
float breath_p;
u16 jump_time;

extern u8 data var_rx_buf[VAR_RX_BUF_SIZE];


///上电次数
//u8 r_poweroncnt;
///// 应用层系统时间
//u8 r_systemtime;
/// ?
//data u8 r_acin;

extern void net_clear_network();
extern void delay_ms(u16 t);
static void app_set_def_pwm();										
static u16 rgb_breath_time_clc(void);
static void led_flash(void);

static void app_set_rgb(const u8 r,const u8 g,const u8 b);
static void app_save_model(u8 index);
static void app_read_model(u8 index);


static void F_CWRunning(void);
static void PWM_Init(void);
static void F_SetBrightToneValue(void);
static void F_ToneDown(void);
static void F_ToneUp(void);
static void F_BrightDown(void);
static void F_BrightUp(void);
u8 flash_count;



///快捷键 对应的灯光模式
code u8 led_key_model_val[][2] = {
																		{0,100},
																		{50,100},
																		{100,100},
																		{0,50},
																		{50,50},
																		{50,25}};

code u8 infrared_cmd_map[][3] = {
					{0x35,CTL_BRIGHT_VAL,50},//亮度50%
					{0x3D,CTL_BRIGHT_DOWN,0},//亮度-
					{0xAF,CTL_BRIGHT_VAL,20},//亮度20%
					{0xD7,CTL_TONE_DOWN,0},//色温-
					{0xF7,CTL_TONE_UP,0},//色温+
					
};


																		
/// led状态结构定义
Led_Sta_T * led_sta_t;
//RGB_Model_Param rgb_model_dat;


//code u8 rgb_model_index[][6]={
//	{0,0,0,0,0,0},//off
//	{0x04,0x60,0x00,0x00,0X00,0X00},//constant
//	{0x01,0x90,0,0,0,0},//line
//	{0x01,0x90,0x01,0x00,0x00,0x00},//line_jump
//	{0x00,0x60,0x01,0xC8,0x04,0xE8},//sin
//	{0x80,0x05,0x81,0x90,0x00,0x00},//breath
//	{0x00,0xff,0x00,0x00,0x00,0x00},//rgb
//};																		
//			
#if LED_RGB				
/**
 * @brief RGB亮度加减
 *
 * @param f 亮度加减标志
 *   -1  亮度减
 *    1  亮度加
 * @return void
 */
 
static void rgb_bright_ctl(char f)
{
	if(((led_sta_t->led_model>>LED_MODEL_SHIFT)&0x03) == 0x00)
		return;
	if((led_sta_t->rgb_model_dat.percentage_k&0x80) == 0x00){//如果是固定颜色
		led_sta_t->rgb_model_dat.percentage_k += f*10;
		if(led_sta_t->rgb_model_dat.percentage_k>200) 
			led_sta_t->rgb_model_dat.percentage_k = 0;
		else if(led_sta_t->rgb_model_dat.percentage_k>100) 
			led_sta_t->rgb_model_dat.percentage_k = 100;
	}
	
}	
																		
/**
 * @brief RGB 动态模式参数计算
 *
 * @return void
 */
static void rgb_breath_clc(void)
{
	if(led_sta_t->rgb_model_dat.rgb_model == RGB_MODEL_BREATHING){
		if((led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x8000) == 0x0000){//固定颜色
			local_info.mod_time = 20000;
		}else{
			g_tmp = 1.0*led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a/(led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x3FFF);
			local_info.mod_time = (u16)(20000*g_tmp);
		}
		//计算Kb
//		if((led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x8000) == 0){//如果是固定颜色，
//			breath_p = 0.01 * (led_sta_t->rgb_model_dat.percentage_k&0x7F);
//		}else 
		if(local_info.mod_time > 0){
			breath_p = 200.0*(led_sta_t->rgb_model_dat.percentage_k&0x7F)/local_info.mod_time;
		}
	}
//	else{
//		breath_p = 0.01 * (led_sta_t->rgb_model_dat.percentage_k&0x7F);
//	}
	//跳频
	if(led_sta_t->rgb_model_dat.model_pram.breath_val.jump_a > 0){
		jump_time = (local_info.mod_time/led_sta_t->rgb_model_dat.model_pram.breath_val.jump_a)>>1;
	}else{
		jump_time = 0;
	}
}	
							


static void app_default_rgb_model_breath(void)
{

	led_sta_t->rgb_model_dat.rgb_model = RGB_MODEL_BREATHING;
	led_sta_t->rgb_model_dat.rgb_model_old = led_sta_t->rgb_model_dat.rgb_model;
	led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k = 0xC0C8;
	led_sta_t->rgb_model_dat.percentage_k = 100;
	led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a = 170;
	led_sta_t->rgb_model_dat.model_pram.breath_val.jump_a = 0;
	led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_h = 0;
	rgb_breath_clc();

}	
#endif

/**
 * @brief 保存数据任务
 *
 * @return void
 */
static void app_save_task(void)
{
	net_updat_local_info();
}


/**
 * @brief 根据亮度和色温比计算PWM
 * 
 * @param x_Bright 亮度百分比
 * @param y_Tone 色温百分比
 * @return 无
 * @note 要避免浮点型运算，比价耗费资源
 */
static void F_Countdata(const u8 x_Bright,const u8 y_Tone)
{
	u32 temp1,temp2;
	u16 temp3,temp4;
	if((led_sta_t->led_model&0x10) != 0x00){
		r_c_needvalue = 0;  //暖光
		r_w_needvalue = 0;	//冷光
		return;
	}
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
 * @brief 开灯
 * 
 * @return 无
 */
static void F_OpenLed(void)
{
//	if(led_sta_t->led_sta == 1)
//		return;
	led_sta_t->led_sta = 1;
	r_waittimes = 1;	

	PWM0EN = 0x0F;

	F_Countdata(r_bright,r_tone);
#if LED_RGB
	led_sta_t->rgb_model_dat.rgb_model = led_sta_t->rgb_model_dat.rgb_model_old;
#endif
}
/**
 * @brief 关灯
 * 
 * @return 无
 */
static void F_CloseLed(void)
{
//	if(led_sta_t->led_sta == 0)
//		return;
	led_sta_t->led_sta = 0;
	r_waittimes = 1;

	PWM0EN = 0x08;

	
#if LED_RGB
	r_r_needvalue = 0;
	r_g_needvalue = 0;
	r_b_needvalue = 0;
	led_sta_t->rgb_model_dat.rgb_model_old = led_sta_t->rgb_model_dat.rgb_model;
	led_sta_t->rgb_model_dat.rgb_model = RGB_MODEL_OFF;
#endif
}

/**
 * @brief 灯状态切换
 * 
 * @return 无
 */
static void F_Switch(void)
{
	if(led_sta_t->led_sta == 0)
	{
		F_OpenLed();
	}else{
		F_CloseLed();
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
//	r_acin = 0;
	PWM_Init();
	#if defined(UART_EN)
	uart_init();
	#endif
	//获取设备状态广播参数的缓存区
	led_sta_t = (Led_Sta_T *)net_get_adv_param_buf();
	//设置LED的模式   默认为冷暖模式
//	led_sta_t->led_model = CURRENT_LED_MODEL<<LED_MODEL_SHIFT;
	//设置设备广播参数的长度  长度小于11     当前模式为冷暖模式 ，设备广播参数为4个字节 led_model,led_sta,led_bright,led_tone
	net_set_adv_param_len(6);//sizeof(Led_Sta_T)
//	led_sta_t->led_sta = 1;					//灯的状态设置为开灯  
	
	if(((led_sta_t->led_model>>LED_MODEL_SHIFT)&0x0F) == 0x00){//冷暖模式
			r_tone = led_key_model_val[1][0];
			r_bright = led_key_model_val[1][1];
		  led_sta_t->led_bright =  r_bright;
		  led_sta_t->led_tone = r_tone;
			net_updat_local_info();
	}
	
//	if(led_sta_t->led_bright == 0){
//		r_tone = led_key_model_val[1][0];
//		r_bright = led_key_model_val[1][1];
//	}else{
//		r_bright = led_sta_t->led_bright;//获取灯的亮度
//		r_tone = led_sta_t->led_tone;		//获取色温值
//	}
	

////判断是否是第一次
//if(led_sta_t->rgb_model_dat.rgb_model == RGB_MODEL_OFF){//加载默认的模式
//	#if LED_RGB
//	app_default_rgb_model_breath();
//	#endif
//	r_tone = led_key_model_val[1][0];
//	r_bright = led_key_model_val[1][1];
//}else{
//	r_bright = led_sta_t->led_bright;//获取灯的亮度
//	r_tone = led_sta_t->led_tone;		//获取色温值
//}

	F_OpenLed();
	r_runningflag = 1;
	r_waittimes = 0;
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
	//判读UUID
	if(net_pdu->dat.net_cmd.param[0] != local_info.uuid[0] || net_pdu->dat.net_cmd.param[1] != local_info.uuid[1]){
		return 0;
	}
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://闪灯   当设备入网，移除网络，配对，取消配对时会进行闪灯   ，移除网络对码的闪灯需要在2S内完成
			//net_pdu->dat.net_cmd.param[0]; 1---入网   2---移除网络   3---对码   4---取消对码
		flash_count = net_pdu->dat.net_cmd.param[2];
//		if(led_sta_t->led_sta == 0)
//		{
//			led_sta_t->led_sta = 1;
//			F_OpenLed();
//		}

		os_task_add(1,led_flash);
		break;
		case CTL_LED_ON://开灯
			F_OpenLed();
		res = 1;
		break;
		case CTL_LED_OFF://关灯
			F_CloseLed();
		res = 1;

		break;
		case CTL_BRIGHT_VAL://设置亮度
			r_bright = net_pdu->dat.net_cmd.param[3];
			led_sta_t->led_bright = r_bright;
			F_SetBrightToneValue();
		break;
		case CTL_BRIGHT_UP://亮度+
				F_BrightUp();
		#if LED_RGB
			rgb_bright_ctl(1);
		#endif
		break;
		case CTL_BRIGHT_DOWN://亮度-
			F_BrightDown();
		#if LED_RGB
			rgb_bright_ctl(-1);
		#endif
		break;
		case CTL_TONE_VAL://设置色温
//			if((led_sta_t->led_sta&0x02) == 0)
//				return res;
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
		if((led_sta_t->led_model&0x12) != 0x12){ // 不是 RGB手动模式
					keytemp = net_pdu->dat.net_cmd.param[2];	
				if(keytemp<6){
					r_tone = led_key_model_val[keytemp][2];
					r_bright = led_key_model_val[keytemp][3];
					led_sta_t->led_bright =  r_bright;
					led_sta_t->led_tone = r_tone;
				}	
			F_SetBrightToneValue();	
			app_set_def_pwm();
				if((led_sta_t->led_model&0x30) == 0x10){
					led_sta_t->led_model &= 0x0F;
					led_sta_t->led_model |= 0x20;
				}
			led_sta_t->led_sta |= 1;
		}
	
		break;
		case CTL_LED_RGB://RGB
#if LED_RGB
			led_sta_t->rgb_model_dat.rgb_model = RGB_MODEL_RGB;
			led_sta_t->rgb_model_dat.model_pram.rgb_val.r = net_pdu->dat.net_cmd.param[2];
			led_sta_t->rgb_model_dat.model_pram.rgb_val.g = net_pdu->dat.net_cmd.param[3];
			led_sta_t->rgb_model_dat.model_pram.rgb_val.b = net_pdu->dat.net_cmd.param[4];
			led_sta_t->rgb_model_dat.rgb_model_old = led_sta_t->rgb_model_dat.rgb_model;
#endif
		break;
		case CTL_RGB_OFF://关闭RGB
			
		break;
		case CTL_NIGHT://夜灯
			
		break;
		case CTL_TIMING://定时
			
		break;
		case CTL_COUNT_DOWN://倒计时
			
		break;
		case CTL_COLOUR_TEMP://色温开启状态
			if(net_pdu->dat.net_cmd.param[2] == 0x01){
				led_sta_t->led_sta |= (1<<1);
			}else{
				led_sta_t->led_sta &= ~(1<<1);
			}
		break;
		case CTL_MUSIC://音乐开启状态
			
		break;
		#if LED_RGB
		case CTL_RGB_RUN://RGB变换  动态演示
			if(led_sta_t->led_model != 0x11){
				//关WY
				led_sta_t->led_model = 0x11;   //RGB自动模式
				F_SetBrightToneValue();
				app_default_rgb_model_breath();
			}else{ //保存颜色 
				led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k = rgb_breath_time_clc();
				led_sta_t->led_model = 0x10;   //RGB自动模式
				F_SetBrightToneValue();
			}
		break;
		case CTL_RGB_SWITCH://RGB手动切换
			led_sta_t->led_model = 0x12;   //RGB手动模式
		  F_SetBrightToneValue();
			#if LED_RGB
			led_sta_t->rgb_model_dat.rgb_model = net_pdu->dat.net_cmd.param[2];
			memcpy(&led_sta_t->rgb_model_dat.percentage_k,&net_pdu->dat.net_cmd.param[3],6);
		  led_sta_t->rgb_model_dat.rgb_model_old = led_sta_t->rgb_model_dat.rgb_model;
			rgb_breath_clc();
			#endif
		break;
		case CTL_WY_SWITCH:
			led_sta_t->led_model = 0x00;
			led_sta_t->rgb_model_dat.rgb_model = RGB_MODEL_OFF;
		  led_sta_t->rgb_model_dat.rgb_model_old = led_sta_t->rgb_model_dat.rgb_model;
			r_bright = led_sta_t->led_bright;
		  F_SetBrightToneValue();
		break;
		case CTL_MEMORY_SAVE:
		{
			u8 b,t;
			b = led_sta_t->led_bright;
			t = led_sta_t->led_tone;
			led_sta_t->led_bright =  r_bright;
		  led_sta_t->led_tone = r_tone;
			app_save_model(net_pdu->dat.net_cmd.param[2]);
			led_sta_t->led_bright = b;
			led_sta_t->led_tone = t;
		}
		break;
		case CTL_MEMORY_READ:
			app_read_model(net_pdu->dat.net_cmd.param[2]);
		  r_bright = led_sta_t->led_bright;//获取灯的亮度
			r_tone = led_sta_t->led_tone;		//获取色温值
		  F_SetBrightToneValue();
			rgb_breath_clc();
		break;
		#endif
	}

	if(os_task_update_time(app_save_task,3000) != 0){
		os_task_add(3000,app_save_task);
	}

//	led_sta_t->led_bright = r_bright;
//	led_sta_t->led_tone = r_tone;			
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
 * @brief RGB 计算
 * 
 * @param time RGB时间轴
 * @param percentage RGB亮度百分比 0 ~ 100
 * @return void
 */																		
static void rgb_clc(u16 time,u8 percentage)
{
#if LED_RGB
	if (time < 256) {
		r_color_r = 255;
		r_color_g = 0.5*time;
		r_color_b = 0;
	}
	else if(time < 512) {
		r_color_r = 255;
		r_color_g = 0.5*time;
		r_color_b = 0;
	}
	else if(time < 768){
		r_color_r = 767 - time;
		r_color_g = 255;
		r_color_b = 0;
	}
	else if (time < 1024) {
		r_color_r = 0;
		r_color_g = 255;
		r_color_b = time - 768;
	}
	else if (time < 1280) {
		r_color_r = 0;
		r_color_g = 1279 - time;
		r_color_b = 255;
	}
	else if (time < 1792) {
		r_color_r = 0.5*time - 640;
		r_color_g = 0;
		r_color_b = 255;
	}
	
	r_color_r = (r_color_r*percentage)/100;
	r_color_g = (r_color_g*percentage)/100;
	r_color_b = (r_color_b*percentage)/100;
	
	app_set_rgb(r_color_r,r_color_g,r_color_b);
	r_waittimes = 0;
#endif
}
																		
/**
 * @brief 设置冷暖PWM
 * 
 * @return 无
 */																																				
static void app_set_def_pwm(void)
{

		C_VALUE_H = (u8)((r_c_nowvalue>>8)&0X03);		
		C_VALUE_L = (u8)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (u8)((r_w_nowvalue>>8)&0X03);	
		W_VALUE_L = (u8)(r_w_nowvalue&0X00FF);

}
#if LED_RGB
/**
 * @brief 设置RGB PWM
 * 
 * @return 无
 */	
static void app_set_rgb_pwm(void)
{
		R_VALUE_H = (u8)((r_r_nowvalue>>8)&0X03);		
		R_VALUE_L = (u8)(r_r_nowvalue&0X00FF);
		G_VALUE_H = (u8)((r_g_nowvalue>>8)&0X03);	
		G_VALUE_L = (u8)(r_g_nowvalue&0X00FF);
		B_VALUE_H = (u8)((r_b_nowvalue>>8)&0X03);	
		B_VALUE_L = (u8)(r_b_nowvalue&0X00FF);

}

/**
 * @brief 计算RGB时间
 * 
 * @return 返回计算的时间
 */
static u16 rgb_breath_time_clc(void)
{
	u16 rgb_time = 3000;
	if((led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0xC000) == 0xC000){//改变颜色
				
			rgb_time = ((u16)(1.0*g_time*(led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x3FFF)/1000))%(led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a*20);
		
			if(rgb_time > (led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a*10)){
				rgb_time = 20*led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a - rgb_time;
			}
		}else if((led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0xC000) == 0x8000){
			rgb_time = ((u16)(1.0*g_time*(led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x3FFF)/1000))%(led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_a*10);
		}else{//不改变颜色
			rgb_time = (led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_time_k&0x3FFF);
		}
		
		if(jump_time > 0){//跳变
//				rgb_time = (rgb_time/(led_sta_t->rgb_model_dat.model_pram.breath_val.jump_a*10))*led_sta_t->rgb_model_dat.model_pram.breath_val.jump_a*10;
			rgb_time = (rgb_time - rgb_time%jump_time);
		}
		rgb_time = (rgb_time + led_sta_t->rgb_model_dat.model_pram.breath_val.rgb_h*10);
		return rgb_time;
}
#endif	


/**
 * @brief 红外数据处理
 * 
 * @return 无
 */	
static void infrared_deal(void)
{
	u8 i;
	NET_PDU *pdu = (NET_PDU *)&var_rx_buf[15];
	if(uart_rx_dat != 0x00){
		pdu->dat.net_cmd.param[0] = local_info.uuid[0];
		pdu->dat.net_cmd.param[1] = local_info.uuid[1];
		for(i=0;i<5;i++){//查找指令
			if(uart_rx_dat == infrared_cmd_map[i][1]){
				pdu->dat.net_cmd.opcode = infrared_cmd_map[i][1];
				pdu->dat.net_cmd.param[2] = infrared_cmd_map[i][2];
			}
		}
		if( i== 5 && uart_rx_dat == 0x97){//开关指令重映射
			pdu->dat.net_cmd.opcode = CTL_LED_ON + led_sta_t->led_sta&0x01;
		}else if(i== 5 && uart_rx_dat == 0x5F){//定时关灯
			os_task_add(18000,F_CloseLed);  // 18000ms = 1800S = 30min
			uart_rx_dat = 0x00;
			return;
		}else{
			uart_rx_dat = 0x00;
			return;
		}
		
		app_input(pdu);
		uart_rx_dat = 0x00;
	}
}


/**
 * @brief 应用层进程
 * 
 * @return 无
 * @note while循环调用该函数
 */	
void app_process(void)
{
	u16 rgb_time = 3000;
	u8 p = 100;
	infrared_deal();
	F_CWRunning();
#if LED_RGB
//		F_AcPowerDect();
	//根据RGB模式转换时间
	switch(led_sta_t->rgb_model_dat.rgb_model){
		case RGB_MODEL_OFF:
			p = 0;
		break;
		case RGB_MODEL_BREATHING:
			//判断是否改变颜色
			rgb_time = rgb_breath_time_clc();
			//判读是否进行呼吸功能
			if(led_sta_t->rgb_model_dat.percentage_k&0x80){//有呼吸功能
				g_tmp = breath_p*g_time;
				p = (u16)(g_tmp)%200;
				if(p > 100){
					p = 200 - p;
				}
			}else{//没有呼吸功能
				p = led_sta_t->rgb_model_dat.percentage_k%101;
			}
		break;
		case RGB_MODEL_RGB:
			r_color_r = led_sta_t->rgb_model_dat.model_pram.rgb_val.r;
			r_color_g = led_sta_t->rgb_model_dat.model_pram.rgb_val.g;
			r_color_b = led_sta_t->rgb_model_dat.model_pram.rgb_val.b;
		break;
	}

		rgb_clc(rgb_time,p);
		r_r_nowvalue = r_r_needvalue;
		r_g_nowvalue = r_g_needvalue;
		r_b_nowvalue = r_b_needvalue;
		app_set_rgb_pwm();
#endif
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
 
static void led_flash(void)
{
	if(led_sta_t->led_sta == 1){
		led_sta_t->led_sta &= 0xFE;
		PWM0EN = 0x08;
#if LED_RGB
		PWM1EN = 0x08;
		PWM2EN = 0x08;
#endif
	}else{
		led_sta_t->led_sta |= 1;
		PWM0EN = 0x0F;	
#if LED_RGB
		PWM1EN = 0x0F;	
		PWM2EN = 0x0F;
#endif		
	}
	if(flash_count>0){
		flash_count--;
		os_task_add(500,led_flash);
	}
}





/**
 * @brief 亮度加
 * 
 * @return 无
 */
static void F_BrightUp(void)
{
	if((led_sta_t->led_model&0x10) != 0x00)
		return;
			if(r_c_needvalue>=r_c_nowvalue || r_w_needvalue>=r_w_nowvalue)
			{
				if((r_c_needvalue!=0 &&(r_c_needvalue - r_c_nowvalue)<50) || (r_w_needvalue!=0 && (r_w_needvalue - r_w_nowvalue)<50))
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
	led_sta_t->led_bright = r_bright;
}

/**
 * @brief 亮度减
 * 
 * @return 无
 */
static void F_BrightDown(void)
{
	if(((led_sta_t->led_model>>LED_MODEL_SHIFT)&0x01) != 0x00)
		return;
		if(r_c_needvalue<=r_c_nowvalue || r_w_needvalue<=r_w_nowvalue)
			{
				if( (r_c_needvalue!=0 && (r_c_nowvalue - r_c_needvalue)<50) ||(r_w_needvalue != 0 && (r_w_nowvalue - r_w_needvalue)<50))
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
	led_sta_t->led_bright = r_bright;			
}




/**
 * @brief 色温加
 * 
 * @return 无
 */
static void F_ToneUp(void)
{
	if(((led_sta_t->led_model>>LED_MODEL_SHIFT)&0x01) != 0x00)
		return;
	if(r_c_needvalue>=r_c_nowvalue || r_w_needvalue>=r_w_nowvalue)
	{
			
			if((r_c_needvalue!=0 && (r_c_needvalue - r_c_nowvalue)<50) || (r_w_needvalue != 0 && (r_w_needvalue - r_w_nowvalue)<50))
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
	led_sta_t->led_tone = r_tone;		

}
/**
 * @brief 色温减
 * 
 * @return 无
 */
static void F_ToneDown(void)
{
	if(((led_sta_t->led_model>>LED_MODEL_SHIFT)&0x01) != 0x00)
		return;
	if(r_c_needvalue<=r_c_nowvalue || r_w_needvalue<=r_w_nowvalue)
	{
			
			if((r_c_needvalue!= 0 && (r_c_nowvalue - r_c_needvalue)<50) || (r_w_needvalue!= 0 && (r_w_nowvalue - r_w_needvalue)<50))
			{
				r_tone = r_tone - 5;
				if(r_tone<5)
				{
					r_tone = 0;
				}		
				r_waittimes = 2;	
				F_Countdata(r_bright,r_tone);		
			}
	}
	led_sta_t->led_tone = r_tone;		
}
/**
 * @brief 设置亮度和色温值
 * 
 * @return 无
 */
static void F_SetBrightToneValue(void)
{
	r_waittimes = 2;
	F_Countdata(r_bright,r_tone);	
	
}



/**
 * @brief 应用层中断处理函数
 * 
 * @return 无
 * @note 用来跟新PWM
 */
static void F_CWRunning(void)
{
//	app_process();
		if(r_runningflag==1) //;r_runningflag
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

				if((r_c_needvalue==r_c_nowvalue) && (r_w_needvalue==r_w_nowvalue)){
						r_runningflag = 0;					
				}				
			}
			else
			{
				r_nowcount++;
			}
		}
		
}




#if LED_RGB
/**
 * @brief 计算RGB的PWM值
 * 
 * @param r 红色 0-255
 * @param g 绿色 0-255
 * @param b 蓝色 0-255
 * @return void
 */
static void app_set_rgb(const u8 r,const u8 g,const u8 b)
{

	r_r_needvalue = r*4;
	if(r_r_needvalue > 1000)r_r_needvalue = 1000;

	r_g_needvalue = g*4;
	if(r_g_needvalue > 1000)r_g_needvalue = 1000;

	r_b_needvalue = b*4;
	if(r_b_needvalue > 1000)r_b_needvalue = 1000;

}
#endif	


/**
 * @brief PWM初始化
 *
 * @return 无
 */
static void PWM_Init(void)
{
//	#if (UUID == UUID_LIGHT)
//	#if LED_WY
//	P1M0 = (P1M0&0x0F)|0xC0; //P11 
//	P0M0 = (P0M0&0xF0)|0x0C; //P00 
//	W_PIN_MAP = W_PIN;					
//	C_PIN_MAP = C_PIN;					
//	
////	P2M2 = (P2M2&0xF0)|0x08;
////	P2M3 = (P2M3&0xF0)|0x08;
////	PWM01_MAP = 0x26;					//PWM01通道映射P23口
////	PWM0_MAP = 0x24;					//PWM0通道映射P01口		
//			
//	
//	PWM0C 			= 0X00;           //  128分频
//	PWM0EN 			= 0x0F;			
//	
//	PWM0PH = 0X03;						
//	PWM0PL = 0XE8;				
//	
//	
//	C_VALUE_H = 0x02;   					
//	C_VALUE_L = 0x00; 			
//	W_VALUE_H = 0x02;  					
//	W_VALUE_L = 0x00; 	
//	
//	#endif
//	
//	#if LED_RGB
//	P0M0 = (P0M0&0x0F)|0xC0; //P00 
//	P0M1 = 0xCC; //P00 
//	R_PIN_MAP = R_PIN;
//	G_PIN_MAP = G_PIN;
//	B_PIN_MAP = B_PIN;
//	
//	PWM1C 			= 0X00;           //  128分频
//	PWM1EN 			= 0x0F;			
//	
//	PWM1PH = 0X03;						
//	PWM1PL = 0XE8;				
//	
//	PWM2C 			= 0X00;           //  128分频
//	PWM2EN 			= 0x0F;			
//	
//	PWM2PH = 0X03;						
//	PWM2PL = 0XE8;	

//	R_VALUE_H = 0X00;
//	R_VALUE_L = 0X00;
//	G_VALUE_H = 0X00;
//	G_VALUE_L = 0X00;
//	B_VALUE_H = 0X00;
//	B_VALUE_L = 0X00;
//	
//	#endif
//	
// #endif

	#if (UUID == UUID_LIGHT)
	#if LED_WY
	P2M1 = (P2M1&0x0F)|0xC0; //P23  
	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
	W_PIN_MAP = W_PIN;					//PWM01通道映射P23口
	C_PIN_MAP = C_PIN;					//PWM0通道映射P01口
	
//	P2M2 = (P2M2&0xF0)|0x08;
//	P2M3 = (P2M3&0xF0)|0x08;
//	PWM01_MAP = 0x26;					//PWM01通道映射P23口
//	PWM0_MAP = 0x24;					//PWM0通道映射P01口		
			
	#if REVERSE
	PWM0C 			= 0X0C;           //  128分频
	#else
	PWM0C 			= 0X00;           //  128分频
	#endif
	PWM0EN 			= 0x0F;			
	
	PWM0PH = 0X03;						
	PWM0PL = 0XE8;				
	
	
	C_VALUE_H = 0x00;   					
	C_VALUE_L = 0x00; 			
	W_VALUE_H = 0x00;  					
	W_VALUE_L = 0x00; 	
	
	#endif
	
	#if LED_RGB
	
	P2M2 = 0xCC;									//P24,P25
	P2M3 = (P2M3&0xF0)|0x0C;  		//P26
	R_PIN_MAP = R_PIN;
	G_PIN_MAP = G_PIN;
	B_PIN_MAP = B_PIN;
	#if REVERSE
	PWM1C 			= 0X0D;           //  128分频
	#else
	PWM1C 			= 0X01;           //  128分频
	#endif
	PWM1EN 			= 0x0F;			
	
	PWM1PH = 0X03;						
	PWM1PL = 0XE8;				
	#if REVERSE
	PWM2C 			= 0X0D;           //  128分频
	#else
	PWM2C 			= 0X01;           //  128分频
	#endif
	PWM2EN 			= 0x0F;			
	
	PWM2PH = 0X03;						
	PWM2PL = 0XE8;	

	R_VALUE_H = 0X00;
	R_VALUE_L = 0X00;
	G_VALUE_H = 0X00;
	G_VALUE_L = 0X00;
	B_VALUE_H = 0X00;
	B_VALUE_L = 0X00;
	
	#endif
	
 #endif

}

extern u8 data var_tx_buf[VAR_TX_BUF_SIZE];


static void app_save_model(u8 index)
{
	flash_read_buf(APP_ADDR,var_tx_buf,24);
	memcpy(&var_tx_buf[12*index],led_sta_t,12);
	flash_erase_section(APP_ADDR);
	flash_write_buf(APP_ADDR,var_tx_buf,24);
	
}

static void app_read_model(u8 index)
{
	flash_read_buf(APP_ADDR+12*index,led_sta_t,12);
}
