#ifndef __NETWORK_CONF_H__
#define __NETWORK_CONF_H__
#include "HC89S003F4.h"
#include "netstack_phy.h"
//#include "netstack_mac.h"
#include "netstack_net.h"

//#include "uart.h"
//网路配置文件
extern struct netstack_local local_info;

#define LOCAL_INFO_ADDR 0x3C00    //本地信息存储地址
#define NETSTACK_GROUP_ADDR			(LOCAL_INFO_ADDR+256)  //组成员地址
#define LOCAL_NAME_ADDR 				(NETSTACK_GROUP_ADDR+128)

#define NETSTACK_GROUP_MAX			10   //最大组数

#define CHIP_ADDR 							0x0100	


//xdata 基地址
#define VAR_BASE_X			0x00

//本地信息结构体变量地址和大小
#define VAR_LOCAL_INFO_X			VAR_BASE_X
#define VAR_LOCAL_INTO_SIZE		0x28



#define VAR_TX_BEACON_OFFSET	 			0x08
//#define VAR_TX_BEACON_SIZE				 0x22

//data 基地址
#define VAR_BASE_D						0x10

//接收缓存地址和大小
#define VAR_RX_BUF_D					VAR_BASE_D
#define VAR_RX_BUF_SIZE				0x2F

//发送缓存地址和大小
#define VAR_TX_BUF_D					(VAR_BASE_D+VAR_RX_BUF_SIZE)
#define VAR_TX_BUF_SIZE				0x2F



#define	FEATURE_LOW_POWER		0
#define	FEATURE_NET					0
#define FEATURE_LEARN				0


/**
 * @name 设备类型 UUID ，该UUID由 上海晶曦微电子科技有限公司定义， 如果客户需要新的UUID，需要向上海晶曦微电子科技有限公司申请
 * @{
 */
#define UUID_BASE_STATION		0X0000					//基站
#define UUID_LIGHT					0X0001					//灯
#define UUID_PLUG						0X0002					//插座
#define UUID_ALARM					0X0003					//警报器
#define UUID_FAN						0X0004					//风扇
/** @} */



/////////////////////////////////////////////////////////////////////////////////////////////////
//  可修改配置  开始
/////////////////////////////////////////////////////////////////////////////////////////////////


#define UUID	UUID_LIGHT   //设置当前的UUID为灯的UUID    

#define COMPANY_ID 	0x4E00  //公司ID， 用户可以自定义

#define DEFAULT_DEV_NAME	"LED" 
#define DEFAULT_DEV_NAME_LEN	3

/////////////////////////////////////////////////////////////////////////////////////////////////
//  可修改配置  结束
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif