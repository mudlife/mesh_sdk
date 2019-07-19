#ifndef __UART_H__
#define __UART_H__
#include <HC_DEFINE.H>
void uart_init(void);
void send_byte(u8 d);
void send_str(u8 *buf,u8 len);
void send_num(u8 num);

#endif