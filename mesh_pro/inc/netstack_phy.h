#ifndef __NETSTACK_PHY_H__
#define __NETSTACK_PHY_H__
#include <HC_DEFINE.H>



u8 phy_init(void);
u8 phy_read_reg(u8 reg);
void phy_write_reg(u8 reg,u8 val);
//u8 phy_read_reg_bit(u8 reg,u8 bits);
void phy_write_reg_bit(u8 reg,u8 bits,u8 val);
void phy_read_fifo(u8 reg, u8 *buf, u8 len);
void phy_write_fifo(u8 reg, u8 *buf, u8 len);
void phy_set_channel(u8 channel);



#endif