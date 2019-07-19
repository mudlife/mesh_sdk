#ifndef __NETSTACK_APP_H__
#define __NETSTACK_APP_H__
#include <HC_DEFINE.H>


#define   LED_MODEL_WY      0x00     //冷暖模式
#define   LED_MODEL_RGB     0x01		//RGB模式
#define   LED_MODEL_WY_RGB  0x02 //冷暖RGB共存模式

#define   CURRENT_LED_MODEL	 LED_MODEL_WY  //当前LED模式

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

#define CTL_CMD_MAX 			0X4B  ///<最大指令
/** @} */

/** 
 * @brief LED灯设备广播参数结构体
 */
typedef struct led_sta_t{
	u8 led_model;  ///<LED模式
	u8 led_sta;     ///<LED状态
	u8 led_bright; ///<亮度
	u8 led_tone;	 ///<色温
	u8 ttl;
}Led_Sta_T;




void app_process(void);
u8 app_init(void);
u8 app_input(NET_PDU *net_pdu);
u8 app_output(NET_PDU *net_pdu);
void F_OpenLed();
void F_CloseLed();
void F_BrightUp();
void F_BrightDown();
void F_ToneUp();
void F_ToneDown();
void F_SetBrightToneValue();
void F_AcPowerDect();
void F_CWRunning(void);
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone);
void PWM_Init(void);
#endif