/**
 * @file netstack_app_plug.c
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


#define CTL_LED_FLASH			0X40	///<����


#define CTL_PLUG_ON				0X41  ///<��
#define CTL_PLUG_OFF			0X42  ///<��



#define LED_G					P2_3
#define LED_W				  P0_0
#define PLUG_SWITCH 	P0_4


/** 
 * @brief PIR�豸�㲥�����ṹ��
 */
typedef struct{
	u8 plug_sta; ///<��������״̬
	u8 tmp[4];
	u8 flag;
}Plug_Sta_T;


Plug_Sta_T *plug_sta_t;

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
		plug_sta_t->flag |= 0x02;

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


/**
 * @brief Ӧ�ò����
 * 
 * @return ��
 * @note whileѭ�����øú���
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
 * @brief APP���ʼ��
 * 
 * @return ��
 */
u8 app_init(void)
{
	app_key_init();//������ʼ��
	
	//LED�Ƴ�ʼ��
	
	P0M0 = (P0M0&0XF0)|0X0C;  //led_w
	P2M1 = (P2M1&0X0F)|0XC0;	//led_g
	
	//�̵���IO����
	P0M2 = (P0M2&0XF0)|0X0C;
	
	LED_G = 1;
	
	LED_W = 1;
	
	//��ȡ�豸״̬�㲥�����Ļ�����
	plug_sta_t = (Plug_Sta_T *)net_get_adv_param_buf();
	net_set_adv_param_len(5);

	
	if(plug_sta_t->flag == 0){//��һ������
		plug_sta_t->flag |= 1;
		net_updat_local_info();	
	}
	
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
		case CTL_PLUG_ON://��
			
		break;
		case CTL_PLUG_OFF://��
			
		break;
	}
	return res;
}	

