#include "led.h"
#include "HC89S003F4.h"

/**
 * ���ܣ�LED ��ʼ��
 * ��������
 * ���أ���
 * ��ע��P0_0 ��ΪLED�Ŀ��ƶ˿�
 */
void led_init(void)
{
		//P20 ��Ϊ led���ƶ˿�
		P2M1 = (P2M1&0xFF)|0xC0;  //����Ϊ�������
//		P2M3 = (P2M3&0xFF)|0xC0;  //����Ϊ�������
	
}

/**
 * ���ܣ�led��
 * ��������
 * ���أ���
 */
void led_on(void)
{
	P2_3 = 1;
}
/**
 * ���ܣ�led�ر�
 * ��������
 * ���أ���
 */
void led_off(void)
{
	P2_3 = 0;
}
/**
 * ���ܣ�led��ת
 * ��������
 * ���أ���
 */
void led_toggle(void)
{
  P2_3 = ~P2_3;
}

