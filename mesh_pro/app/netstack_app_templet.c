/**
 * @file netstack_app_templet.c
 * @brief Ӧ�ò��ļ�
 * @details ʵ�־���Ӧ�ã����ļ�Ϊģ��
 * @author mudlife
 * @par Copyright (c):
 * 			�Ϻ�����΢���ӿƼ����޹�˾
 * @version 1.0.0
 * @date 2019-09-06
 */
 
 
#include <string.h>
#include <math.h>
#include "network_conf.h"
#include "netstack_app.h"
#include "os.h"
#include "delay.h"



/**
 * @brief Ӧ�ò����
 * 
 * @return ��
 * @note whileѭ�����øú���
 */	
void app_process(void)
{
	
}


/**
 * @brief APP���ʼ��
 * 
 * @return ��
 */
u8 app_init(void)
{

	
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
		case CTL_LED_FLASH://���� 
	 
		break;
		case CTL_PLUG_ON://��
			
		break;
		case CTL_PLUG_OFF://��
			
		break;
	}
	return res;
}	

