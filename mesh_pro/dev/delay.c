/**
 * @file delay.c
 * @brief 延时文件
 * @details 实现微妙延时，毫秒延时
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 */
#include "delay.h"

/**
 * @brief 微妙延时
 *
 * @param t 延时时间
 * @return 无
 */
void delay_us(u16 t)
{
	while(t--);
}
/**
 * @brief 毫秒延时
 *
 * @param t 延时时间
 * @return 无
 */
void delay_ms(u16 t)
{
	u16 i;
	 for(i=0;i<t;i++)
		delay_us(1425);
}
/**
 * @brief 0.5毫秒延时
 *
 * @param t 延时时间
 * @return 无
 */
void delay_0_5_ms(u16 t)
{
	u16 i;
	 for(i=0;i<t;i++)
		delay_us(750);
}