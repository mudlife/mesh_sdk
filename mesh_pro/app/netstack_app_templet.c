/**
 * @file netstack_app_templet.c
 * @brief 应用层文件
 * @details 实现具体应用，该文件为模板
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
#include "os.h"
#include "delay.h"



/**
 * @brief 应用层进程
 * 
 * @return 无
 * @note while循环调用该函数
 */	
void app_process(void)
{
	
}


/**
 * @brief APP层初始化
 * 
 * @return 无
 */
u8 app_init(void)
{

	
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
		case CTL_LED_FLASH://闪灯 
	 
		break;
		case CTL_PLUG_ON://开
			
		break;
		case CTL_PLUG_OFF://关
			
		break;
	}
	return res;
}	

