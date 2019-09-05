#ifndef __NETSTACK_NET_H__
#define __NETSTACK_NET_H__
#include <HC_DEFINE.H>
#include "netstack_mac.h"


/**
 * @name 网络特征码
 * @{
 */
#define NET_FEATURE_NET_STA		(0)
#define NET_FEATURE_SETTING		(1)
#define NET_FEATURE_LOW_POWER  (2)
#define NET_FEATURE_FRIENDSHIP (3)
#define NET_FEATURE_LEARN			 (4)
#define NET_FEATURE_NET				(5)
#define NET_FEATURE_FREE_PAIR		(6)
/** @} */

/** 本地信息结构体 */
struct netstack_local{
	u8 mac_addr[6];				///<本地MAC地址
	u8 pass[6];						///<密匙
	u8 channel;						///<信道
	u8 seq;								///<序列号
	u8 nid;								///<网络ID
	u8 net_sta;						///<网络状态
	u8 group_count;
	u8 uuid[2];
	u8 dst[3];						///<目标地址
	u16 mod_time;
	u8 device_sta_adv_pram[14]; ///<设备广播参数
	u8 device_sta_adv_len;			///<设备广播参数长度
	u8 pair_time;								///<配对超时时间
	u8 admin_id[3];							///<管理员ID
	u8 pweron;									///<应用层使用
	#if defined(NET_RESET)
	u8 reboot_cnt;							///<网络层重启次数
	#endif 
	u8 flash_crc_val;						///<信息校验
	
};

/** 消息缓存结构体 */
struct netstack_rx_flag{
	u8 src[3];							///<源地址
	u8 seq;									///<序列号
};

/** 组成员结构体 */
struct group_member{
	u8 user_id[3];				///<用户ID
	u8 device_id;					///<设备ID
};

/** 组结构体 */
struct group_t{
	union {
		u8 id[4];								///<id
		struct group_member gm; ///< 组成员
	}g;												///<共用体
};

u8 net_init(u16 uuid);
//void net_set_mac_addr(const u8 *addr);
u8 net_input(u8 *src_addr,NET_PDU *net_pdu,u8 len);
u8 net_output(u8 *addr,NET_PDU *net_pdu,u8 pkt_type,u8 count);
void net_updat_local_info(void);
u8 net_get_local_info(void);
u8 net_set_adv_param_len(u8 len);
void *net_get_adv_param_buf(void);
#endif