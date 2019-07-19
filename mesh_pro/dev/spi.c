/**
 * @file spi.c
 * @brief SPI操作文件
 * @details SPI初始化，SPI读写操作
 * @author mudlife
 * @par Copyright (c):
 * 			上海晶曦微电子科技有限公司
 * @version 1.0.0
 * @date 2019-06-02
 */
#include "spi.h"
#include "HC89S003F4.h"

/** The dummy byte to be written while data is being received */
#define SPI_DUMMY_BYTE      (0xFF)

/** SPI configurations */
#define SPI_CONFIG  (bmSPI_SSIG | bmSPI_SPEN | bmSPI_MSTR | bmSPI_CPHA |0x00)


/**
 * SPI interrupt service routine.
 */
//void spi_isr(void) interrupt SPI_VECTOR
//{
//    /* NOP */
//}


/**
 * @brief SPI GPIO初始化
 * 
 * @return 无
 */
static void spi_config_gpio(void)
{
	  P0M1 = (P0M1 & 0x0F) | 0xC0; // CE: P0.3 - push pull

	
		P0M2 = (P0M2 & 0X0F) | 0XC0; // SS P0.4 - output push pull; SCK P0.5 - output push pull
	  P0M3 = 0xCC;
    P2M0 = 0xC6; // MOSI P0.6 - output push pull; MISO P0.7 - input pull-up

    SS_MAP = 0x05;
    SCK_MAP = 0x21;
    MOSI_MAP = 0x07;
    MISO_MAP = 0x20;

}


/**
 * @brief SPI初始化
 * 
 * @return 无
 */
void spi_init(void)
{
    /* GPIO configuration */
    spi_config_gpio();
		SPI_SS = 1;
		SPCTL = SPI_CONFIG;

}

/**
 * @brief 读写一个byte
 * 
 * @param val 写入值
 * @return 读取到的值
 */
u8 spi_read_write(u8 val)
{
		data u8 dat;
    SPDAT = val;
    while (!(SPSTAT & bmSPI_SPIF));
    dat = SPDAT;
    SPSTAT = 0x80;
    return (dat);
}



//u8 spi_read_write(u8 val)
//{
//		u8 dat,i;
//		SPI_SCK = 0;
//		for(i=0;i<8;i++){
//			dat <<= 1;
//			SPI_SCK = 1;
//			if(SPI_MISO)
//				dat |= 0x01;
//			if(val&0x80)
//				SPI_MOSI = 1;
//			else
//				SPI_MOSI = 0;
//			SPI_SCK = 0;
//			val <<= 1;
//		}
//    return (dat);
//}


