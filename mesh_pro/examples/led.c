#include "led.h"
#include "HC89S003F4.h"

/**
 * 功能：LED 初始化
 * 参数：无
 * 返回：无
 * 备注：P0_0 作为LED的控制端口
 */
void led_init(void)
{
		//P20 作为 led控制端口
		P2M1 = (P2M1&0xFF)|0xC0;  //配置为推挽输出
//		P2M3 = (P2M3&0xFF)|0xC0;  //配置为推挽输出
	
}

/**
 * 功能：led打开
 * 参数：无
 * 返回：无
 */
void led_on(void)
{
	P2_3 = 1;
}
/**
 * 功能：led关闭
 * 参数：无
 * 返回：无
 */
void led_off(void)
{
	P2_3 = 0;
}
/**
 * 功能：led反转
 * 参数：无
 * 返回：无
 */
void led_toggle(void)
{
  P2_3 = ~P2_3;
}

