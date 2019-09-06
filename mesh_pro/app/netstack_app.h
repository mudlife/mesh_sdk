#ifndef __NETSTACK_APP_H__
#define __NETSTACK_APP_H__
#include <HC_DEFINE.H>

void app_process(void);
u8 app_init(void);
u8 app_input(NET_PDU *net_pdu);
u8 app_output(NET_PDU *net_pdu);


#endif