#ifndef __SPI_H__
#define __SPI_H__
#include <HC_DEFINE.H>


/*
 * SPI pins.
 */
//#define SPI_SS      P0_4
//#define SPI_SCK     P0_5
//#define SPI_MOSI    P0_6
//#define SPI_MISO    P0_7


#define SPI_SS      P0_5
#define SPI_SCK     P2_1
#define SPI_MOSI    P0_7
#define SPI_MISO    P2_0



/* PKT_FLAG interrupt */
//#define RF_PKT_FLAG_PIN     P2_6

/* Chip enable */
#define RF_CE_PIN           P0_6

/* Enable RF */
#define RF_ENABLE() \
    RF_CE_PIN = 1; \
		P0_0 = ~P0_0

/* Disable RF */
#define RF_DISABLE() \
    RF_CE_PIN = 0


/* Select WL1670 SPI */
#define RF_SPI_SELECT() \
    SPI_SS = 0

/* Deselect WL1670 SPI */
#define RF_SPI_DESELECT() \
    SPI_SS = 1; 





void spi_init(void);

u8 spi_read_write(u8 val);


#endif