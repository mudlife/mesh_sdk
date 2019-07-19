#ifndef __FLASH_H__
#define __FLASH_H__

#include "HC89S003F4.h"


void flash_erase_section(u16 addr);
void flash_read_buf(u16 addr,u8 *buf,u8 len);
void flash_write_buf(u16 addr,u8 *buf,u8 len);
void flash_read_mac_addr(u16 addr,u8 *buf);
u8 flash_crc(u8 *dat,u8 len);
#endif