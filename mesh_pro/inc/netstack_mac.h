#ifndef __NETSTACK_MAC_H__
#define __NETSTACK_MAC_H__
#include <HC_DEFINE.H>


enum SYNC_WORD{
	SYNC_WORD_BLE=0x00,
	SYNC_WORD_G24=0x01
};


#define HEAD_TYPE_MASK	0x07

#define CHANNEL_MASK		0x03
//head type
typedef enum{
	TYPE_RESERVED,     //����
	TYPE_UNICAST,     //����
	TYPE_MULTICAST,		//�鲥
	TYPE_BROADCAST,		//�㲥
}PKT_TYPE;


/*
 * Different length in a packet
 */
#define PREAMBLE_LEN            (1)
#define ACCESS_ADDRESS_LEN      (4)
#define PDU_HEADER_LEN          (2)
#define MAX_PDU_PAYLOAD_LEN     (37)
#define CRC_LEN                 (3)
#define MAX_DATA_WHITENING_LEN  (PDU_HEADER_LEN + MAX_PDU_PAYLOAD_LEN + CRC_LEN)
#define DEV_ADDR_LEN            (6)


/** 网络层指令 */
typedef struct{
	u8 opcode;///<操作码
	u8 param[15];///<参数
}NET_CMD;

/** 网络层数据 */
typedef struct{
	u8 seg:1;///<分段标志
	u8 sn:1;///<当前序列
	u8 nesn:1;///<期望序列
	u8 rfu:5;///<保留
	u8 app_pdu[10];///<应用层数据
}NET_DAT;

/** 网络层PDU */
typedef struct{
	u8 nid:5;///<网络ID   1011 1110
	u8 ctl:1;///<控制标志位
	u8 at:1;///<地址类型标志位
	u8 sm:1;///<安全标志位
	u8 src[3];///<源地址
	u8 seq;///<序列号
	u8 ttl;///<生命周期
	u8 dst[3];///<目标地址
	u8 group_id;///<组ID
	/** 共用�*/
	union{
		NET_CMD net_cmd; ///<网络层指令
		NET_DAT net_dat; ///<网络层数据
	}dat;
	u8 len;						 ///<数据大小
}NET_PDU;


/** 蓝牙广播头部结构体 */
typedef struct _ADV_PDU_HEADER {
    u8 rx_add: 1;	///<接收地址类型
    u8 tx_add: 1;///<发送地址类型
    u8 rfu0: 2;///<保留
    u8 type: 4;///<广播类型
    u8 length: 6;///<广播数据长度
    u8 rfu1: 2;///<保留
} ADV_PDU_HEADER;


/* PDU header fields */
#define PDU_HDR_TXADDR_RANDOM       BIT6
#define PDU_HDR_RXADDR_RANDOM       BIT7
#define PDU_HDR_TYPE_MASK           (0x0F)
#define PDU_HDR_TYPE_ADV_IND        (0)
#define PDU_HDR_TYPE_ADV_DIRECT_IND  (1)
#define PDU_HDR_TYPE_ADV_NONCONN_IND (2)
#define PDU_HDR_TYPE_SCAN_REQ       (3)
#define PDU_HDR_TYPE_SCAN_RSP       (4)
#define PDU_HDR_TYPE_CONN_REQ       (5)
#define PDU_HDR_TYPE_ADV_SCAN_IND   (6)
#define PDU_HDR_LENGTH_MASK         (0x3F)


 #define IBEACON_PDU_LEN     (6 + 27) // 6-byte ADVA, 30-byte IBEACON
 #define EXP_LEN             (1 + IBEACON_PDU_LEN) // 1-byte Length
 


/** ADV_IND PDU */
typedef struct _ADV_IND_PDU {
    union {
        ADV_PDU_HEADER bits;
        u8 bytes[2];
    } header;									///<头部
    u8 mac_addr[DEV_ADDR_LEN];///<蓝牙MAC地址
   	u8 adv_flag[3];						///<ibeacon flag
		u8 len;										///<ibeacon len
		u8 type;									///<ibeacon type
		u8 company_id[2];					///<公司ID
		u8 dat[27];								///<数据
} ADV_IND_PDU;


/** �����㲥���ṹ�� */
typedef struct _BLE_PKT {
    ADV_IND_PDU adv_ind;  ///<�������ݰ�
    u8 crc[CRC_LEN];      ///<CRC У��
    u8 len;								///<���ݳ���
		u8 rssi;							///<RSSIֵ
} BLE_PKT;


u8 mac_init(void);
void mac_pack_packet(u8 *addr,u8 len);
void mac_load_packet(void);
void mac_send_packet(u8 channel);
void mac_listen_packet(u8 channel);
void mac_process(void);
void mac_set_power(u8 power);
u8 mac_scan_rssi(u8 ch);
void mac_direct_mode_enter(void);
void mac_direct_mode_break(void);

#endif