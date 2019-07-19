#include "adc.h"
#include "HC89S003F4.h"

unsigned int AdcValue = 0;			//用于存放ADC的值
unsigned int oldAdcValue = 0;			//用于存放ADC的值
u8 pwm_en;
char p1;
void adc_init(void)
{
	P0M0 = P0M0&0xF0|0x03;				//P00设置为模拟输入
 	ADCC0 = 0x80;						//打开ADC转换电源
	ADCC1 = 0x00;						//选择外部通道0
	ADCC2 = 0x4D;						//转换结果12位数据，数据右对齐，ADC时钟16分频
	ADCC0 |= 0x40;					//启动ADC转换
	p1 = -1;
}

void adc_on(void)
{
	ADCC0 |= 0x40;					//启动ADC转换
}

void adc_off(void)
{
	ADCC0 &=~ 0x40;			
}

void adc_process(void)
{
	if((ADCC0&0x20)){
		ADCC0 &=~ 0x20;					//清除标志位
		AdcValue = ADCR;			//获取ADC的值
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
