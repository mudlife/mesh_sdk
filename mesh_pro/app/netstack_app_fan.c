/**
 * @file netstack_app_fan.c
 * @brief Ӧ�ò��ļ�
 * @details ʵ�־���Ӧ�ã�����
 * @author mudlife
 * @par Copyright (c):
 * 			�Ϻ�����΢���ӿƼ����޹�˾
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


#define CTL_LED_FLASH							0X40	///<����


#define CTL_FAN_ON								0X41  ///<��       ��ƽ �� 10ms  ��10ms
#define CTL_FAN_OFF								0X42  ///<��       ��ƽ �� 10ms  ��10ms
#define CTL_FAN_TIMER							0X43  ///<��ʱ		 ��ƽ �� 10ms  ��40ms
#define CTL_FAN_STRENGTH_UP				0X44  ///<ǿ��+		 ��ƽ �� 10ms  ��70ms
#define CTL_FAN_STRENGTH_DOWN			0X45  ///<ǿ��-		 ��ƽ �� 10ms  ��80ms
#define CTL_FAN_ROTATE						0X46	///<��ת  	 ��ƽ �� 10ms  ��20ms
#define CTL_FAN_MODEL							0x49  ///<ģʽ		 ��ƽ �� 10ms  ��30ms
#define CTL_FAN_SLEEP							0X4B  ///<˯��		 ��ƽ �� 10ms  ��50ms
#define CTL_FAN_TYPE							0X4C	///<������		 ��ƽ �� 10ms  ��60ms

#define LED_G					P2_3
#define LED_W				  P0_0
#define WAVE_IO			 	P0_3

code u8 wave_shape[]={
					0,
					10,
					10,
					40,
					70,
					80,
					20,
					0,
					0,
					30,
					0,
					50,
					60
};


/** 
 * @brief PIR�豸�㲥�����ṹ��
 */
typedef struct{
	u8 fan_sta; ///<��������״̬
	u8 tmp[4];
	u8 flag;
}Fan_Sta_T;


Fan_Sta_T *fan_sta_t;

u8 flash_count;
u8 led_g_w;


extern void net_sta_adv(void);

static app_model_switch(void);



/***************************************************************************************
  * @˵��  	INT8-15�жϷ�����
  *	@����	��
  * @����ֵ ��
  * @ע		��
***************************************************************************************/
void INT8_15_Rpt() interrupt INT8_15_VECTOR 
{
	if(PINTF1&0x04)						//�ж�INT15�жϱ�־λ
	{

		PINTF1 &= ~0x04;				//���INT15�жϱ�־λ	
		fan_sta_t->flag |= 0x02;

	}
}



/**
 * @brief Ӧ�ò㰴����ʼ��
 *
 * @return void
 */
static void app_key_init(void)
{
	//p00 p01 ��smt��������
	P2M1 = (P2M1&0xF0)|0x06;

	PITS2 = 0X10;
	//�ⲿ�ж�2ʹ��
	PINTE1 |= 1<<2;
	
	IP3 = 0X40;
	
	
	EX8_15 = 1;
	EA = 1;
	
}



/**
 * @brief ����
 * 
 * @return ��
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


static void app_wave_output(u8 index)
{
	WAVE_IO = 1;
	delay_ms(10);
	WAVE_IO = 0;
	delay_ms(wave_shape[index]);
	WAVE_IO = 1;
}


/**
 * @brief Ӧ�ò����
 * 
 * @return ��
 * @note whileѭ�����øú���
 */	
void app_process(void)
{
//	if(fan_sta_t->flag&0x02){
//		fan_sta_t->flag &= 0xFD;
//		delay_ms(10);
//		if(P2_2 == 0){
//			fan_sta_t->fan_sta = fan_sta_t->fan_sta?0:1;
//			PLUG_SWITCH = fan_sta_t->fan_sta;
//			LED_G = fan_sta_t->fan_sta?0:1;
//			net_updat_local_info();
//			net_sta_adv();
//		}

//	}
}


/**
 * @brief APP���ʼ��
 * 
 * @return ��
 */
u8 app_init(void)
{
//	app_key_init();//������ʼ��
	
	//LED�Ƴ�ʼ��
	
	P0M0 = (P0M0&0XF0)|0X0C;  //led_w
	P2M1 = (P2M1&0X0F)|0XC0;	//led_g
	
	//�̵���IO����
	P0M1 = (P0M2&0X0F)|0XC0;
	
	LED_G = 1;
	
	LED_W = 1;
	WAVE_IO = 1;
	//��ȡ�豸״̬�㲥�����Ļ�����
	fan_sta_t = (Fan_Sta_T *)net_get_adv_param_buf();
	net_set_adv_param_len(5);

	
//	if(fan_sta_t->flag == 0){//��һ������
//		fan_sta_t->flag |= 1;
//		net_updat_local_info();	
//	}
	
	return 0;
}


/**
 * @brief APP�㴦���������������ݰ�
 *
 * @param net_pdu ���ݰ�
 * @return ����ֵ�����ж�������Ƿ����豸����㲥
 *  @retval 0 �������豸�㲥
 *  @retval 1 ���豸�㲥
 * @note ���������ã������ط����ܵ���
 */
u8 app_input(NET_PDU *net_pdu)
{

	u8 res = 0;
	if(net_pdu->dat.net_cmd.param[0] != local_info.uuid[0] || net_pdu->dat.net_cmd.param[1] != local_info.uuid[1]){
		return 0;
	}
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://����   ���豸�������Ƴ����磬��ԣ�ȡ�����ʱ���������   ���Ƴ���������������Ҫ��2S�����
	  flash_count = 5;
		os_task_add(1,led_flash);
		break;
		case CTL_FAN_ON:								//��
		case CTL_FAN_OFF:								//��
		case CTL_FAN_TIMER:							//��ʱ
		case CTL_FAN_STRENGTH_UP:				//ǿ��+
		case CTL_FAN_STRENGTH_DOWN:			//ǿ��-
		case CTL_FAN_ROTATE:						//��ת
		case CTL_FAN_MODEL:							//ģʽ
		case CTL_FAN_SLEEP:							//˯��
		case CTL_FAN_TYPE:							//������
			app_wave_output(net_pdu->dat.net_cmd.opcode&0x0F);
		break;
	}
	return res;
}	

