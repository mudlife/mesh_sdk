#ifndef __NETSTACK_NET_H__
#define __NETSTACK_NET_H__
#include <HC_DEFINE.H>
#include "netstack_mac.h"


/**
 * @name ����������
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

/** ������Ϣ�ṹ�� */
struct netstack_local{
	u8 mac_addr[6];				///<����MAC��ַ
	u8 pass[6];						///<�ܳ�
	u8 channel;						///<�ŵ�
	u8 seq;								///<���к�
	u8 nid;								///<����ID
	u8 net_sta;						///<����״̬
	u8 group_count;
	u8 uuid[2];
	u8 dst[3];						///<Ŀ���ַ
	u16 mod_time;
	u8 device_sta_adv_pram[14]; ///<�豸�㲥����
	u8 device_sta_adv_len;			///<�豸�㲥��������
	u8 pair_time;								///<��Գ�ʱʱ��
	u8 admin_id[3];							///<����ԱID
	u8 pweron;									///<Ӧ�ò�ʹ��
	#if defined(NET_RESET)
	u8 reboot_cnt;							///<�������������
	#endif 
	u8 flash_crc_val;						///<��ϢУ��
	
};

/** ��Ϣ����ṹ�� */
struct netstack_rx_flag{
	u8 src[3];							///<Դ��ַ
	u8 seq;									///<���к�
};

/** ���Ա�ṹ�� */
struct group_member{
	u8 user_id[3];				///<�û�ID
	u8 device_id;					///<�豸ID
};

/** ��ṹ�� */
struct group_t{
	union {
		u8 id[4];								///<id
		struct group_member gm; ///< ���Ա
	}g;												///<������
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