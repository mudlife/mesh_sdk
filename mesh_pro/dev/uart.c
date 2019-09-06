#include "uart.h"
#include "HC89S003F4.h"
u8 uart_rx_dat;
#if defined(UART_EN)
void uart_init(void)
{
#if defined(LOG)
	P0M0 = P0M0&0x0F | 0x80; //P01 设置为推挽输出
	P1M0 = P1M0&0xF0 | 0x02; //P00 设置为上拉输入
	//TXD_MAP = 0x01;   //TXD映射P01
	RXD_MAP = 0x10;   //RXD映射P00
	T4CON = 0x06;
	TH4 = 0xFF;
	TL4 = 0x98;
	SCON2 = 0x02;
	SCON = 0x10;
	IE |= 0x10;
	EA = 1;
	uart_rx_dat = 0;
#endif
}


void send_byte(u8 d)
{
#if defined(LOG)
	IE &=~0x10;
	SBUF = d;
	while(!(SCON & 0x02));
	SCON &=~ 0x02;	
	IE |= 0x10;
	SCON |= 0x10;				//UART1接收使能
#endif
}

void send_str(u8 *buf,u8 len)
{
#if defined(LOG)
	u8 i;
	IE &=~0x10;
	for(i=0;i<len;i++){
		SBUF = buf[i];
		while(!(SCON & 0x02));
		SCON &=~ 0x02;	
	}
	
	IE |= 0x10;
	SCON |= 0x10;				//UART1接收使能
#endif
}

void send_num(u8 num)
{
#if defined(LOG)
	u8 num_arry[3];
	num_arry[0] = num/100 + 0x30;
	num_arry[1] = num%100/10 +0x30;
	num_arry[2] = num%10 + 0x30;
	send_str(num_arry,3);
#endif
}

void UART1_Rpt(void) interrupt UART1_VECTOR
{
	if(SCON & 0x01)						//判断接收中断标志位
	{
		uart_rx_dat = SBUF;
		SCON &=~ 0x01;					//清除接收中断标志位
	}									
}



#endif















