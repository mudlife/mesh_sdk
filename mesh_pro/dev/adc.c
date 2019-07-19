#include "adc.h"
#include "HC89S003F4.h"

unsigned int AdcValue = 0;			//���ڴ��ADC��ֵ
unsigned int oldAdcValue = 0;			//���ڴ��ADC��ֵ
u8 pwm_en;
char p1;
void adc_init(void)
{
	P0M0 = P0M0&0xF0|0x03;				//P00����Ϊģ������
 	ADCC0 = 0x80;						//��ADCת����Դ
	ADCC1 = 0x00;						//ѡ���ⲿͨ��0
	ADCC2 = 0x4D;						//ת�����12λ���ݣ������Ҷ��룬ADCʱ��16��Ƶ
	ADCC0 |= 0x40;					//����ADCת��
	p1 = -1;
}

void adc_on(void)
{
	ADCC0 |= 0x40;					//����ADCת��
}

void adc_off(void)
{
	ADCC0 &=~ 0x40;			
}

void adc_process(void)
{
	if((ADCC0&0x20)){
		ADCC0 &=~ 0x20;					//�����־λ
		AdcValue = ADCR;			//��ȡADC��ֵ
//		if(AdcValue> 1000 && pwm_en == 0){
//			pwm_en = 1;
//			led_on();
////			PWM0EN = 0x08;						
////			PWM0EN = 0x0F;						
//		}else 
		if(AdcValue < 3000 && oldAdcValue > AdcValue && p1 == -1){
			pwm_en = 0;
			p1 = 0;
			PWM0EN = 0x08;
//			led_off();
		}
		oldAdcValue = AdcValue;
		adc_on();
	}
}
