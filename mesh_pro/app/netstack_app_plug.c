/**
 * @file netstack_app_plug.c
 * @brief 应用层文件
 * @details 实现具体应用，插座
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-08-29
 */
 
 
#include <string.h>
#include <math.h>
#include "network_conf.h"
#include "netstack_app.h"
//#include "pwm.h"
#include "os.h"
#include "delay.h"


#define CTL_LED_FLASH			0X40	///<闪灯


#define CTL_PLUG_ON				0X41  ///<开
#define CTL_PLUG_OFF			0X42  ///<关



#define LED_G					P2_3
#define LED_W				  P0_0
#define PLUG_SWITCH 	P0_4


/** 
 * @brief PIR设备广播参数结构体
 */
typedef struct{
	u8 plug_sta; ///<插座开关状态
	u8 tmp[4];
	u8 flag;
}Plug_Sta_T;


Plug_Sta_T *plug_sta_t;

u8 flash_count;
u8 led_g_w;


extern void net_sta_adv(void);

static app_model_switch(void);



/***************************************************************************************
  * @说明  	INT8-15中断服务函数
  *	@参数	无
  * @返回值 无
  * @注		无
***************************************************************************************/
void INT8_15_Rpt() interrupt INT8_15_VECTOR 
{
	if(PINTF1&0x04)						//判断INT15中断标志位
	{

		PINTF1 &= ~0x04;				//清除INT15中断标志位	
		plug_sta_t->flag |= 0x02;

	}
}



/**
 * @brief 应用层按键初始化
 *
 * @return void
 */
static void app_key_init(void)
{
	//p00 p01 带smt上拉输入
	P2M1 = (P2M1&0xF0)|0x06;

	PITS2 = 0X10;
	//外部中断2使能
	PINTE1 |= 1<<2;
	

	
	IP3 = 0X40;
	
	
	EX8_15 = 1;
	EA = 1;
	
}



/**
 * @brief 闪灯
 * 
 * @return 无
 */
 
void led_flash(void)
{
	if(led_g_w == 1){
		led_g_w = 0;
		LED_W = 0;

	}else{
		led_g_w = 1;
		LED_W = 1;
	}
	if(flash_count>0){
		flash_count--;
		os_task_add(500,led_flash);
	}else{
		LED_W = 1;
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
	if(plug_sta_t->flag&0x02){
		plug_sta_t->flag &= 0xFD;
		delay_ms(10);
		if(P2_2 == 0){
			plug_sta_t->plug_sta = plug_sta_t->plug_sta?0:1;
			PLUG_SWITCH = plug_sta_t->plug_sta;
			LED_G = plug_sta_t->plug_sta?0:1;
			net_updat_local_info();
			net_sta_adv();
		}

	}
}


/**
 * @brief APP层初始化
 * 
 * @return 无
 */
u8 app_init(void)
{
	app_key_init();//按键初始化
	
	//LED灯初始化
	
	P0M0 = (P0M0&0XF0)|0X0C;  //led_w
	P2M1 = (P2M1&0X0F)|0XC0;	//led_g
	
	//继电器IO设置
	P0M2 = (P0M2&0XF0)|0X0C;
	
	LED_G = 1;
	
	LED_W = 1;
	
	//获取设备状态广播参数的缓存区
	plug_sta_t = (Plug_Sta_T *)net_get_adv_param_buf();
	net_set_adv_param_len(5);

	
	if(plug_sta_t->flag == 0){//第一次运行
		plug_sta_t->flag |= 1;
		net_updat_local_info();	
	}
	
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

	u8 res = 0;
	if(net_pdu->dat.net_cmd.param[0] != local_info.uuid[0] || net_pdu->dat.net_cmd.param[1] != local_info.uuid[1]){
		return 0;
	}
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://闪灯   当设备入网，移除网络，配对，取消配对时会进行闪灯   ，移除网络对码的闪灯需要在2S内完成
	  flash_count = 5;
		os_task_add(1,led_flash);
		break;
		case CTL_PLUG_ON://开
			
		break;
		case CTL_PLUG_OFF://关
			
		break;
	}
	return res;
}	

