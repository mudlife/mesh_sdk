/**
 * @file system.c
 * @brief 系统文件
 * @details 用于芯片系统初始化，系统时钟初始化和时钟中断处理
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 */ 
#include "system.h"
#include "network_conf.h"

data u16 g_time=0;          ///<全局时间
//extern u8 adv_flag;
extern u8 task_tick; 
//extern u8 update_time_flag;




/**
 * @brief 系统初始化
 * 
 * @return 无
 * @note 初始化定时器0，用作滴答定时器
 */
void system_init(void)
{
	
	WDTCCR = 0x00;						//关闭看门狗
	while((CLKCON&0x20)!=0x20);			//等待内部高频RC起振
	CLKSWR = 0x51;						//选择内部高频时钟为主时钟，内部高频RC2分频，Fosc=16MHz
	while((CLKSWR&0xC0)!=0x40);			//等待内部高频切换完成
	CLKDIV = 0x01;						//Fosc1分频得到Fcpu，Fcpu=16MHz 
	FREQ_CLK = 0x10;					//指明当前系统时钟8
	

	TCON1 = 0x00;						//Tx0定时器时钟为Fosc
	TMOD = 0x00;						//16位重装载定时器/计数器

	//Tim0计算时间 	= (65536 - 0xFACB) * (1 / (Fosc /Timer分频系数))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//定时1ms
	//反推初值 	= 65536 - ((1/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB
	TH0 = 0xFA;
	TL0 = 0xCB;							//T0定时时间1ms
	IE |= 0x02;							//打开T0中断
	TCON |= 0x10;						//使能T0
    
	EA = 1;								//打开总中断
	
	P2M1 = (P2M1&0XF0)|0X0C;
	P2_0 = 0;
}

/**
 * @brief 看门狗初始化
 * 
 * @return 无
 */
void wdt_init(void)
{
	WDTC = 0x57;   //允许WDT复位，允许掉电/空闲模式下运行，1024分频
	WDTCCR = 0xFF;  //5.93S
}

/**
 * @brief T0中断服务函数
 *
 * @return 无
 */
void TIMER0_Rpt(void) interrupt TIMER0_VECTOR
{
	g_time++;
	g_time = g_time%local_info.mod_time ;
	task_tick = 1;
	
	if(g_time%50 == 0){
		system_tdma();
//		P0_0 = ~P0_0;
	}
	
}

/**
 * @brief  	跳频
 *
 * @return 无
 */
void system_tdma(void)
{
	local_info.channel = ++local_info.channel%3;
}


/**
 * @brief  	复位不重读代码选项
 *
 * @return 无
 */
void ResetNoReadOption(void)
{
	IAP_CMD = 0xF00F;
	IAP_CMD = 0x8778;
}


/**
  * @说明  	复位重读代码选项
  * @参数  	无
  * @返回值 无
  * @注		无
  */
//void ResetReadOption(void)
//{
//	IAP_CMD = 0xF00F;
//	IAP_CMD = 0x7887;
//}