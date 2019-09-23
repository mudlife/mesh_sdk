#include <string.h>
#include "network_conf.h"
#include "netstack_app.h"
//#include "pwm.h"
#include "os.h"



#define   LED_MODEL_WY      0x00     //��ůģʽ
#define   LED_MODEL_RGB     0x01		//RGBģʽ
#define   LED_MODEL_WY_RGB  0x02 //��ůRGB����ģʽ

#define   CURRENT_LED_MODEL	 LED_MODEL_WY  //��ǰLEDģʽ

#define		LED_MODEL_SHIFT		(4)

#define CTL_LED_FLASH			0X40	//����
#define CTL_LED_ON				0X41  //����
#define CTL_LED_OFF				0X42  //�ص�

#define CTL_BRIGHT_VAL		0X43	//��������
#define CTL_BRIGHT_UP			0X44  //����+
#define CTL_BRIGHT_DOWN		0X45  //����-

#define CTL_TONE_VAL			0X46	//����ɫ��
#define CTL_TONE_UP				0X47	//ɫ��+
#define CTL_TONE_DOWN			0X48	//ɫ��-

#define CTL_KEY_BOARD			0X49  //��ݼ�

#define CTL_LED_RGB				0X4A  //RGB��ɫ

#define CTL_CMD_MAX 			0X4B  //���ָ��


//LED���豸�㲥�����ṹ��
typedef struct led_sta_t{
	u8 led_model;  //ģʽ
	u8 led_sta;
	u8 led_bright; // ����
	u8 led_tone;	 //ɫ��
	u8 led_start_cnt;
}Led_Sta_T;


#define C_VALUE_L     PWM0DL
#define C_VALUE_H     PWM0DH
#define W_VALUE_L     PWM0DTL
#define W_VALUE_H     PWM0DTH

#define PIN_AC				P0_2//�������Ŷ���

unsigned char r_ac_dbs_in;
unsigned char r_ac_dbs_free;
unsigned char r_acpower_ready;

unsigned char r_runflag;
unsigned char r_waittimes;
unsigned short r_c_needvalue;
unsigned short r_w_needvalue;
unsigned short r_c_nowvalue;
unsigned short r_w_nowvalue;
unsigned short r_autosave;

unsigned char r_nowcount;
unsigned char r_runningflag;
unsigned char  r_bright;
unsigned char r_tone;
unsigned char r_tone;
unsigned char r_poweroncnt;
unsigned char r_systemtime;

extern void net_clear_network();
extern void delay_ms(u16 t);
void led_flash(void);
void F_FlashFuntion(u8 x);

unsigned char flash_count;
unsigned char r_acin;


Led_Sta_T * led_sta_t;



static void F_OpenLed();
static void F_CloseLed();
static void F_BrightUp();
static void F_BrightDown();
static void F_ToneUp();
static void F_ToneDown();
static void F_SetBrightToneValue();
static void F_AcPowerDect();
static void F_CWRunning(void);
static void F_Countdata(unsigned char x_Bright,unsigned char y_Tone);
static void PWM_Init(void);


void F_FlashFuntion(u8 x);

static void F_CheckClearNet()
{
	r_poweroncnt = local_info.pweron;
	r_autosave = 8000;	
	if((r_poweroncnt==0xa0) || (r_poweroncnt>8))
	{
		r_bright = led_sta_t->led_bright;//��ȡ�Ƶ�����
		r_tone = led_sta_t->led_tone;		//��ȡɫ��ֵ
		led_sta_t->led_sta = 1;					//�Ƶ�״̬����Ϊ����  
		
		if(r_bright>100)
		{
			r_bright = 50;
		}
		if(r_tone>100)
		{
			r_tone = 50;
		}				
		r_poweroncnt = 0;
	}
	else if((r_poweroncnt==0)||(r_poweroncnt==4))
	{
				r_tone = 100;
				r_bright = 100; 	
				r_poweroncnt++;				
	}
	else if((r_poweroncnt==1)||(r_poweroncnt==5))
	{
				r_tone = 0;
				r_bright = 100; 		
				r_poweroncnt++;		
	}
	else if((r_poweroncnt==2)||(r_poweroncnt==6))
	{
				r_tone = 50;
				r_bright = 100;
				r_poweroncnt++;				
	}
	else if((r_poweroncnt==3)||(r_poweroncnt==7))
	{
				r_tone = 50;
				r_bright = 5; 		
				r_poweroncnt++;				
	}	
	else if(r_poweroncnt==8)
	{
				r_tone = 100;
				r_bright = 100;
				F_SetBrightToneValue();	
				r_c_nowvalue = r_c_needvalue;
				r_w_nowvalue = r_w_needvalue;
				C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);		
				C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
				W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);	
				W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);
	
				r_poweroncnt = 0;
				local_info.pweron = r_poweroncnt;		
				net_updat_local_info();
		
				F_FlashFuntion(5);
		
				net_clear_network();
				r_poweroncnt = 0;		
	}		
	
		local_info.pweron = r_poweroncnt;	
	  led_sta_t->led_start_cnt = r_poweroncnt;
		net_updat_local_info();
		F_SetBrightToneValue();	
		r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;
		C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);	
		C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
		W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

		led_sta_t->led_sta = 1;					//�Ƶ�״̬����Ϊ����  	
//	F_SetBrightToneValue();		
}



static void F_SwitchOnOff()
{
	if(r_acpower_ready)
	{
		if(r_acin==0)
		{
			r_acin = 1;			
			PWM0EN = 0x0F;	
			F_CheckClearNet();
			r_runflag = 0;			
		}
	}
	else
	{
		r_acin = 0;
		r_runflag = 1;
		PWM0EN = 0x00;	
	}
}




static void F_Autosave()
{
	if((r_systemtime&0x01)==1)
	{
		r_systemtime = r_systemtime & 0xfe;
		if(r_autosave)
		{
			r_autosave--;
			if(r_autosave==0)
			{
				led_sta_t->led_bright  =  r_bright;
				led_sta_t->led_tone = r_tone;	
				
//	r_bright = led_sta_t->led_bright;//��ȡ�Ƶ�����
//	r_tone = led_sta_t->led_tone;		//��ȡɫ��ֵ				
				
				r_poweroncnt = 0xa0;	
				local_info.pweron = r_poweroncnt;		
				net_updat_local_info();				
			}
		}
	}
	
}

static void F_FlashFuntion(u8 x)
{
		r_runflag = 1;
		while(x)
		{
			PWM0EN = 0x00;	
			WDTC |= 0x10;    //�幷
			delay_ms(250);
			PWM0EN = 0x0F;
			WDTC |= 0x10;    //�幷			
			delay_ms(250);	
			x--;
		}
		r_runflag = 0;
}



//whileѭ�����øú���
void app_process(void)
{
	F_SwitchOnOff();
	F_Autosave();  //�ر�
}












//whileѭ�����øú���
//void app_process(void)
//{
//	F_AcPowerDect();  //�ر�
//}

/**
 *  ���ܣ�����
 */
 /*
void led_flash(void)
{
	if(led_sta_t->led_sta == 1){
		led_sta_t->led_sta = 0;
		PWM0EN = 0x08;
	}else{
		led_sta_t->led_sta = 1;
		PWM0EN = 0x0F;	
	}
	if(flash_count>0){
		flash_count--;
		os_task_add(500,led_flash);
	}
}
*/
/**
 * ���ܣ�APP���ʼ��
 * ��������
 * ���أ���
 */
u8 app_init(void)
{
	//1.��ʼ��LED��
	P0M1 = P0M1 & 0XF0;
	r_c_nowvalue = 00;
	r_w_nowvalue = 00;
	r_c_needvalue = 00;
	r_w_needvalue = 00;	
	r_acin = 0;
	PWM_Init();
	
	//��ȡ�豸״̬�㲥�����Ļ�����
	led_sta_t = (Led_Sta_T *)net_get_adv_param_buf();
	//����LED��ģʽ   Ĭ��Ϊ��ůģʽ
	led_sta_t->led_model = CURRENT_LED_MODEL<<LED_MODEL_SHIFT;
	//�����豸�㲥�����ĳ���  ����С��11     ��ǰģʽΪ��ůģʽ ���豸�㲥����Ϊ4���ֽ� led_model,led_sta,led_bright,led_tone
	net_set_adv_param_len(5);
//	r_bright = led_sta_t->led_bright;//��ȡ�Ƶ�����
//	r_tone = led_sta_t->led_tone;		//��ȡɫ��ֵ
//	led_sta_t->led_sta = 1;					//�Ƶ�״̬����Ϊ����  
	
	
	

//	F_CheckClearNet();


	r_runflag = 0;
	
	return 0;
}

/**
 * ���ܣ�APP�㴦���������������ݰ�
 * ������wl_udp---���ݰ�
 * ���أ��ɹ�����0
 * ��ע�����������ã������ط����ܵ���
 */
//static u32 e=0;

u8 app_input(NET_PDU *net_pdu)
{
	u8 keytemp = 0; 
	u8 res = 0;
	if(net_pdu->dat.net_cmd.param[1] != local_info.uuid[1]){
		return 0;
	}
	switch(net_pdu->dat.net_cmd.opcode){
		case CTL_LED_FLASH://����   ���豸�������Ƴ����磬��ԣ�ȡ�����ʱ���������   ���Ƴ���������������Ҫ��2S�����
			//net_pdu->dat.net_cmd.param[0]; 1---����   2---�Ƴ�����   3---����   4---ȡ������
//		flash_count = 5;
		if(led_sta_t->led_sta == 0)
		{
			led_sta_t->led_sta = 1;
			F_OpenLed();
		}
		
		F_FlashFuntion(3);		
		
//		os_task_add(1,led_flash);
		break;
		case CTL_LED_ON://����
			led_sta_t->led_sta = 1;
			F_OpenLed();
				r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;	
		res = 1;
		break;
		case CTL_LED_OFF://�ص�
			led_sta_t->led_sta = 0;
			F_CloseLed();
			r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;		
		res = 1;

		break;
		case CTL_BRIGHT_VAL://��������
			r_bright = net_pdu->dat.net_cmd.param[3];
			led_sta_t->led_bright = r_bright;
			F_SetBrightToneValue();
		  
	
		break;
		case CTL_BRIGHT_UP://����+
			F_BrightUp();
//			P0 = P0 ^ 0X01;
		break;
		case CTL_BRIGHT_DOWN://����-
			F_BrightDown();
		
		break;
		case CTL_TONE_VAL://����ɫ��
			r_tone = net_pdu->dat.net_cmd.param[2];
			led_sta_t->led_tone = r_tone;
			F_SetBrightToneValue();
		break;
		case CTL_TONE_UP://ɫ��+
			F_ToneUp();
		break;
		case CTL_TONE_DOWN://ɫ��-
			F_ToneDown();
		break;
		case CTL_KEY_BOARD://��ݼ�
		led_sta_t->led_model &= 0xF0;
		led_sta_t->led_model |= net_pdu->dat.net_cmd.param[0];			
		keytemp = net_pdu->dat.net_cmd.param[0];
			r_waittimes = 0;	
			r_waittimes = 0;	
			if(keytemp==0)//ů��
			{
				r_tone = 0;     
				r_bright = 100; //100% ����			
			}
			else if(keytemp==1)//���Թ�
			{
				r_tone = 50;
				r_bright = 100; //50% ����				
			}
			else if(keytemp==4)//������
			{
				r_tone = 50;
				r_bright = 30; //70% ����			
			}
			else if(keytemp==5)//Сҹ��
			{
				r_tone = 50;
				r_bright = 10; // 20% ����	
			}
			else if(keytemp==2)//���
			{
				r_tone = 100;
				r_bright = 100; //100% ����		
			}		
			else if(keytemp==3)//���
			{
				r_tone = 50;
				r_bright = 50; //100% ����		
			}					
		F_SetBrightToneValue();	
			r_c_nowvalue = r_c_needvalue;
		r_w_nowvalue = r_w_needvalue;
		C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);			
		C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
		W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
		W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

		led_sta_t->led_sta = 1;
		break;
		case CTL_LED_RGB://RGB
			
		break;
		
	}
	
	led_sta_t->led_bright = r_bright;
	led_sta_t->led_tone = r_tone;		
	r_poweroncnt = 0xa0;
	local_info.pweron = r_poweroncnt;				
	net_updat_local_info();		
	r_runningflag = 1;
//	res = 1;
	return res;
}	


/**
 * ���ܣ�APP�㷢������
 * ������wl_udp---���ݰ�
 */
/*
u8 app_output(NET_PDU *net_pdu)
{

	return 0;
}
*/


//����
static void F_OpenLed()
{
	r_waittimes = 0;	
	F_Countdata(r_bright,r_tone);

}
//�ص�
static void F_CloseLed()
{
	r_waittimes = 0;
	r_c_needvalue = 0;
	r_w_needvalue = 0;

//	net_updat_local_info();				
}

/********************************************************************************
**	void F_BrightUp()
**	�������� + 

**	�������±���ֵ���ı����Ⱥ�ɫ��
		r_bright
		r_tone

**********************************************************************************/
static void F_BrightUp()
{
	if(r_runningflag==0)
	{
		if(r_bright<100)
		{
			r_bright = r_bright + 5;
			if(r_bright>100)
			{
				r_bright = 100;
			}		
		}
		else
		{
				r_bright = 100;			
		}				
	}
	else
	{
		if(r_c_needvalue!=r_c_nowvalue)
		{
			if(r_c_needvalue>r_c_nowvalue)
			{
				if((r_c_needvalue - r_c_nowvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}
			}
			else 
			{
				if((r_c_nowvalue - r_c_needvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}				
			}
		}
		if(r_w_needvalue!=r_w_nowvalue)
		{
			if(r_w_needvalue>r_w_nowvalue)
			{
				if((r_w_needvalue - r_w_nowvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}
			}			
			else 
			{
			if((r_w_nowvalue - r_w_needvalue)<50)
				{
					r_bright = r_bright + 5;
					if(r_bright>100)
					{
						r_bright = 100;
					}							
				}				
			}
		}		
	}	
			r_waittimes = 2;			
			F_Countdata(r_bright,r_tone);				
}
/********************************************************************************
**	void F_BrightDown()
**	�������� - 

**	�������±���ֵ���ı����Ⱥ�ɫ��
		r_bright
		r_tone

**********************************************************************************/
static void F_BrightDown()
{
	if(r_runningflag==0)
	{
		if(r_bright>10)
		{
			r_bright = r_bright - 5;
			if(r_bright<10)
			{
				r_bright = 10;
			}		
		}
		else
		{
				r_bright = 10;			
		}				
	}
	else
	{
		if(r_c_needvalue!=r_c_nowvalue)
		{
			if(r_c_needvalue>r_c_nowvalue)
			{
				if((r_c_needvalue - r_c_nowvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}
			}
			else 
			{
				if((r_c_nowvalue - r_c_needvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}				
			}			
			
			
		}
		if(r_w_needvalue!=r_w_nowvalue)
		{
			if(r_w_needvalue>r_w_nowvalue)
			{
				if((r_w_needvalue - r_w_nowvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}
			}
			else 
			{
				if((r_w_nowvalue - r_w_needvalue)<50)
				{
					r_bright = r_bright - 5;
					if(r_bright<10)
					{
						r_bright = 10;
					}							
				}				
			}			
			
		}		
	}	
			r_waittimes = 2;			
			F_Countdata(r_bright,r_tone);				
}




/*
void F_BrightDown()
{
	if(r_runningflag==0)
	{
		if(r_bright>5)
		{
			r_waittimes = 2;
			r_bright = r_bright-10;
			if(r_bright<10)
			{
						r_bright = 10;	
			}
			F_Countdata(r_bright,r_tone);					
		}
		else
		{
			if(r_bright>100)
			{
				r_bright = 100;
			}
		}
		
	}
}
/********************************************************************************
**	void F_ToneUp()
**	����ɫ�� + 

**	�������±���ֵ���ı����Ⱥ�ɫ��
		r_bright
		r_tone

**********************************************************************************/
static void F_ToneUp()
{
	if(r_runningflag==0)
	{
		if(r_tone<100)
		{
			r_waittimes = 2;
			r_tone = r_tone + 10;
			if(r_tone>100)
			{
				r_tone = 100;
			}		
		}
		else
		{
				r_tone = 100;
		}
			F_Countdata(r_bright,r_tone);					
	}
}
/********************************************************************************
**	void F_ToneDown()
**	����ɫ��- 

**	�������±���ֵ���ı����Ⱥ�ɫ��
		r_bright
		r_tone

**********************************************************************************/
static void F_ToneDown()
{
	if(r_runningflag==0)
	{
		if(r_tone>=10)
		{
			r_waittimes = 2;
			r_tone = r_tone-10;
			if(r_tone<10)
			{
				r_tone = 0;
			}		
		}
		else 
		{
				r_tone = 0;			
		}
			F_Countdata(r_bright,r_tone);				
	}
}
/********************************************************************************
**	void F_SetBrightToneValue()
**	����ɫ�� ���� 

**	�������±���ֵ���ı����Ⱥ�ɫ��
		r_bright
		r_tone

**********************************************************************************/
static void F_SetBrightToneValue()
{
	r_waittimes = 0;		
	F_Countdata(r_bright,r_tone);	
}

/********************************************************************************
**	void F_AcPowerDect()
**	��������
**	��1ms�ж�
**	r_acpower_ready = 1 �н�������
**	r_acpower_ready = 0 �޽�������
**	��Ҫ��������ȫ�ֱ���
**	unsigned char r_ac_dbs_in;
**	unsigned char r_ac_dbs_free;
**	unsigned char r_acpower_ready;
**********************************************************************************/

static void F_AcPowerDect()
{
	if(PIN_AC)
	{
		r_ac_dbs_in++;
		r_ac_dbs_free = 0;
		if(r_ac_dbs_in>2)
		{
			r_acpower_ready = 1;
		}		
	}
	else
	{
		r_ac_dbs_free++;
		r_ac_dbs_in = 0;
		if(r_ac_dbs_free>30)
		{
			r_acpower_ready = 0;
		}						
	}
}








void F_CWRunning(void)
{
		r_systemtime = 0xff;
		F_AcPowerDect();
		if(r_runflag==0) //;r_runningflag
		{
			if(r_nowcount>=r_waittimes)
			{
				r_nowcount=0;
				if(r_c_needvalue>r_c_nowvalue)
				{
					r_c_nowvalue++;
				}
				else if(r_c_needvalue<r_c_nowvalue)
				{
					r_c_nowvalue--;					
				}
				if(r_w_needvalue>r_w_nowvalue)
				{
					r_w_nowvalue++;
				}
				else if(r_w_needvalue<r_w_nowvalue)
				{
					r_w_nowvalue--;					
				}				
				
				C_VALUE_H = (unsigned char)((r_c_nowvalue>>8)&0X03);				
				C_VALUE_L = (unsigned char)(r_c_nowvalue&0X00FF);
				W_VALUE_H = (unsigned char)((r_w_nowvalue>>8)&0X03);
				W_VALUE_L = (unsigned char)(r_w_nowvalue&0X00FF);

//				if((r_c_nowvalue!=0)||(r_w_nowvalue!=0))
//				{
//					PWM0C 			= 0X03;           //  128��Ƶ
//					PWM0EN 			= 0x0F;			
//				}
//				else
//				{
//					PWM0C 			= 0;           //  128��Ƶ
//					PWM0EN 			= 0;			
//				}
				
				if((r_c_needvalue==r_c_nowvalue)&&(r_w_needvalue==r_w_nowvalue))
				{
						r_runningflag = 0;					
				}				
			}
			else
			{
				r_nowcount++;
			}
		}
		
}



/********************************************************************************
**	void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
**	output ram 
**	r_c_nowvalue = temp3;
**	r_w_nowvalue = temp4;	
		��Ҫ��������ȫ�ֱ���
**	input ram 
**	r_bright; ����ֵ��Χ  0 -- 100 
**	r_tone;	  ɫ��ֵ��Χ  0 -- 100 
**********************************************************************************/
/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101))
	{
		temp1 = (((float)x_Bright)/100)*0x3ff;
		if(y_Tone==50) 
		{
			temp3 = (unsigned short)temp1;  
			temp4 = (unsigned short)temp1;			
		}
		else if(y_Tone==0)
		{
			temp4 = 0;  
			temp3 = (unsigned short)temp1*2;							
		}
		else if(y_Tone==100)
		{
			temp3 = 0;  
			temp4 = (unsigned short)temp1*2;					
		}	
		
		else if(y_Tone<50)
		{
			temp1 = (((float)x_Bright)/100)*0x3ff;
			temp3 = (unsigned short)temp1*2;
			if(temp3>0x3ff)
			{
				temp3 = 0x3ff;
			}
			temp1 = (((float)y_Tone)/100)*temp3;		
			temp2 = (float)((100-y_Tone)/100)*temp3;				
			temp3 = (unsigned short)temp2;
			temp4 = (unsigned short)temp1	;					
		}
		else
		{
			temp1 = (((float)x_Bright)/100)*0x3ff;
			temp3 = (unsigned short)temp1*2;
			if(temp3>0x3ff)
			{
				temp3 = 0x3ff;
			}
			temp1 = (((float)y_Tone)/100)*temp3;		
			temp2 =  (float)((100-y_Tone)/100)*temp3;				
			temp3 = (unsigned short)temp2;
			temp4 = (unsigned short)temp1	;		
		}
			
		if(temp3>=0x3ff)
		{
			temp3 = 0x3ff;
		}
		if(temp4>0x3ff)
		{
			temp4 = 0x3ff;
		}	
	//	local_info.led_bright = temp3;
	//	local_info.led_tone = temp4;

		r_c_needvalue = temp3;  //ů��
		r_w_needvalue = temp4;	//���
	}
}

*/




/********************************************************************************
**	void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
**	output ram 
**	r_c_nowvalue = temp3;
**	r_w_nowvalue = temp4;	
		��Ҫ��������ȫ�ֱ���
**	input ram 
**	r_bright; ����ֵ��Χ  0 -- 100 
**	r_tone;	  ɫ��ֵ��Χ  0 -- 100 
**********************************************************************************/
/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101)){
		//1.����ɫ��
		//���㹫ʽ  PWM1 + PWM2 = 100%
		temp1 = y_Tone*511/100;
		temp2 = 511- temp1;
		
		//2.��������
		//���㹫ʽ PWM * ����%
		temp3 = temp1*x_Bright/100;
		temp4 = temp2*x_Bright/100;
		
		r_c_needvalue = temp4;  //ů��
		r_w_needvalue = temp3;	//���
	}
	
}


void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	
	if(y_Tone<=50)
	{
		temp1 = (((float)x_Bright)/100)*511;
		temp2 = (((float)y_Tone)/50)*(temp1);		
		temp4 = (unsigned short)temp2;
		temp3 = (unsigned short)(temp1 + (temp1 - temp2))	;			
		
	}
	else
	{
		temp1 = ((100-(float)y_Tone)/50)*((((float)x_Bright)/100)*511);		
		temp2 = (((float)x_Bright)/100)*511;		
		temp3 = (unsigned short)temp1;
		temp4 = (unsigned short)(temp2 + (temp2 - temp1))	;		
	}
		
	if(temp3>=511)
	{
		temp3 = 511;
	}
	if(temp4>511)
	{
		temp4 = 511;
	}	
//	local_info.led_bright = temp3;
//	local_info.led_tone = temp4;

	r_c_needvalue = temp3;  //ů��
	r_w_needvalue = temp4;	//���
		
}










*/

static void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	
	if(y_Tone<=50)
	{
		temp1 = (((float)x_Bright)/100)*1023;
		temp2 = (((float)y_Tone)/50)*(temp1);		
		temp4 = (unsigned short)temp2;
		temp3 = (unsigned short)(temp1 + (temp1 - temp2))	;			
		
	}
	else
	{
		temp1 = ((100-(float)y_Tone)/50)*((((float)x_Bright)/100)*1023);		
		temp2 = (((float)x_Bright)/100)*1023;		
		temp3 = (unsigned short)temp1;
		temp4 = (unsigned short)(temp2 + (temp2 - temp1))	;		
	}
		
	if(temp3>=1023)
	{
		temp3 = 1023;
	}
	if(temp4>1023)
	{
		temp4 = 1023;
	}	
//	local_info.led_bright = temp3;
//	local_info.led_tone = temp4;

	r_c_needvalue = temp3;  //ů��
	r_w_needvalue = temp4;	//���
		
}



/*
void F_Countdata(unsigned char x_Bright,unsigned char y_Tone)
{
	float temp1,temp2;
	unsigned short temp3,temp4;
	if((x_Bright<101)&&(y_Tone<101)){
		//1.����ɫ��
		//���㹫ʽ  PWM1 + PWM2 = 100%
		temp1 = y_Tone*1023.0/100;
		temp2 = 1023- temp1;
		
		//2.��������
		//���㹫ʽ PWM * ����%
		temp3 = (unsigned short)(temp1*x_Bright/100);
		temp4 = (unsigned short)(temp2*x_Bright/100);
		
		r_c_needvalue = temp4;  //ů��
		r_w_needvalue = temp3;	//���
	}
	
}

*/




static void PWM_Init(void)
{
//	P2M1 = (P2M1&0x0F)|0xC0; //P23  
//	P0M0 = (P0M1&0x00)|0xCC; //P00 , P01
//	PWM01_MAP = 0x23;					//PWM01ͨ��ӳ��P23��
//	PWM0_MAP = 0x01;					//PWM0ͨ��ӳ��P01��
//	P2M2 = (P2M2&0xF0)|0x08;
//	P2M3 = (P2M3&0xF0)|0x08;
//	PWM01_MAP = 0x26;					//PWM01ͨ��ӳ��P23��
//	PWM0_MAP = 0x24;					//PWM0ͨ��ӳ��P01��		
//	P2M3 = (P2M3&0x0F)|0x80;
//	P1M0 = (P1M0&0xF0)|0x08;
//	PWM01_MAP = 0x27;					//PWM01ͨ��ӳ��P23��
//	PWM0_MAP = 0x10;					//PWM0ͨ��ӳ��P01��				
	
	PWM0PH = 0X03;						
	PWM0PL = 0XFF;				
	
	PWM0DH = 0;   					
	PWM0DL = 0; 			
	PWM0DTH = 0;  					
	PWM0DTL = 0; 	

	P0M0 = 0x88;
	PWM01_MAP = 0x01;					//PWM01ͨ��ӳ��P23��
	PWM0_MAP = 0x00;					//PWM0ͨ��ӳ��P01��	

  PWM0C 			= 0X01;           //  128��Ƶ
	PWM0EN 			= 0x0F;			

}

