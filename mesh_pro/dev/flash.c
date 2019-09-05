/**
 * @file flash.c
 * @brief Flash操作文件
 * @details 实现Flash的读写擦除以及校验操作
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 */
#include "flash.h"
#include <string.h>

/**
 * @brief 擦除 flash
 * @param addr 擦除地址
 * @return 无
 * @note 一次擦除128 byte
 */
#pragma disable							//确保调整时不会进中断导致调整失败
void flash_erase_section(u16 addr)
{
	EA = 0;
	IAP_CMD = 0xF00F;				//Flash解锁
	IAP_ADDR = addr;			//写入擦除地址
	IAP_CMD = 0xD22D;				//选择操作方式， 扇区擦除
	IAP_CMD = 0xE11E; 				//触发后IAP_ADDRL&IAP_ADDRH指向0xFF，同时自动锁定
	EA = 1;
}

/**
 * @brief 从flash中读取数据
 * @param addr 起始地址
 * @param buf 读取缓存
 * @param len 读取长度
 * @return 无
 */
//#pragma disable							//确保调整时不会进中断导致调整失败
void flash_read_buf(u16 addr,u8 *buf,u8 len)
{

		while(len--)
	*(buf++)=*((unsigned char code *)(addr++));//读取数据
}

/**
 * @brief 往flash中写入数据
 * @param addr 起始地址
 * @param buf 数据缓存
 * @param len 写入长度长度
 * @return 无
 * @note 写之前要先判断是否需要擦除
 */
#pragma disable							//确保调整时不会进中断导致调整失败
void flash_write_buf(u16 addr,u8 *buf,u8 len)
{
	EA = 0;
	while(len--)
	{	 	
		IAP_DATA=*buf; 	//待编程数据，写入数据寄存器必须放在解锁之前
		IAP_CMD=0xF00F;				//Flash解锁
		IAP_ADDR=addr;		//写入地址
		IAP_CMD=0xB44B;				//字节编程
		IAP_CMD=0xE11E;				//触发一次操作
		addr++;				//地址加一
		buf++;				//数据后移一位
	}
	EA = 1;
}

/**
 * @brief 从flash中读取mac 地址
 * @param addr 起始地址
 * @param buf 读取缓存
 * @return 无
 */
void flash_read_mac_addr(u16 addr,u8 *buf)
{
	u8 i;
	for(i=0;i<6;i++){
		buf[5-i] = *((unsigned char code *)(addr++));//读取数据
	}
}

/**
 * @brief flash信息校验
 * @param dat 校验数据
 * @param len 数据长度
 * @return 校验值
 */
u8 flash_crc(u8 *dat,u8 len)
{
	u8 i;
	u8 crc = 0;
	for(i=0;i<len;i++){
		crc ^= dat[i];
	}
	
	return crc;
}
