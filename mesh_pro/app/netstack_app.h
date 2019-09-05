#ifndef __NETSTACK_APP_H__
#define __NETSTACK_APP_H__
#include <HC_DEFINE.H>

void app_process(void);
u8 app_init(void);
u8 app_input(NET_PDU *net_pdu);
u8 app_output(NET_PDU *net_pdu);

//void F_OpenLed();
//void F_CloseLed();
//void F_BrightUp();
//void F_BrightDown();
//void F_ToneUp();
//void F_ToneDown();
//void F_SetBrightToneValue();
//void F_AcPowerDect();
//void F_CWRunning(void);
//void F_Countdata(unsigned char x_Bright,unsigned char y_Tone);
//void PWM_Init(void);
#endif