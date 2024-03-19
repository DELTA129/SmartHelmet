/*
 * spi.h
 *
 *  Created on: 12 Mar 2024
 *      Author: mhrehor
 */

#ifndef SPI_C_H_
#define SPI_C_H_

#include <stdint.h>
#include "spi.h"

static inline int SPIC_Init(mxc_spi_regs_t* spi)
{
	mxc_spi_pins_t pins;
	pins.clock = 1;
	pins.miso = 1;
	pins.mosi = 1;
	pins.sdio2 = 0;
	pins.sdio3 = 0;
	pins.ss0 = 1;
	pins.ss1 = 0;
	pins.ss2 = 0;
	pins.vddioh = 1;

	int result = MXC_SPI_Init(spi, 1, 0, 0, 0, 200000, pins);
		result |= MXC_SPI_SetDataSize(spi, 8);
		result |= MXC_SPI_SetWidth(spi, SPI_WIDTH_STANDARD);
	return result;
}

#define SPI_SLAVE0 (1 << MXC_F_SPI_CTRL0_SS_ACTIVE_POS)
#define SPI_SLAVE1 (1 << (MXC_F_SPI_CTRL0_SS_ACTIVE_POS+1))
#define SPI_SLAVE2 (1 << (MXC_F_SPI_CTRL0_SS_ACTIVE_POS+2))
#define SPI_SLAVE3 (1 << (MXC_F_SPI_CTRL0_SS_ACTIVE_POS+3))

uint8_t _SPIC_TransferRaw8(mxc_spi_regs_t* spi, uint8_t dataIn);

/*static inline uint16_t _SPIC_TransferRaw16(mxc_spi_regs_t* spi, uint16_t dataIn)
{
	spi->fifo16[0] = dataIn; //Write data to FIFO
	//Set slave and start transmission
	spi->ctrl1 = 2 | (2 << 16); //Set num of tx/rx characters to 1
	spi->ctrl0 = MXC_F_SPI_CTRL0_EN |  MXC_F_SPI_CTRL0_MST_MODE | MXC_F_SPI_CTRL0_START;
	SPIC_WaitTransferComplete(spi);
	uint8_t rxData = spi->fifo16[0];
	return rxData;
}*/
/*
 * Perform a blocking duplex transfer, transmitting dataIn and returning the received data (8 bits)
 */
static inline uint8_t SPIC_Transfer8(mxc_spi_regs_t* spi, uint8_t dataIn)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN; //Enable Rx and Tx
	return _SPIC_TransferRaw8(spi, dataIn);
}

/*
 * Perform a blocking duplex transfer, transmitting dataIn and returning the received data (16 bits)
 */
/*static inline uint16_t SPIC_Transfer16(mxc_spi_regs_t* spi, uint16_t dataIn)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN; //Enable Rx and Tx
	return _SPIC_TransferRaw16(spi, dataIn);
}*/

/*
 * Perform a blocking one-way transmission, only transmitting dataIn (8 bits)
 */
static inline void SPIC_Send8(mxc_spi_regs_t* spi, uint8_t dataIn)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN; //Enable Rx and Tx
	_SPIC_TransferRaw8(spi, dataIn);
}

/*
 * Perform a blocking one-way transmission, only transmitting dataIn (16 bits)
 */
/*static inline void SPIC_Send16(mxc_spi_regs_t* spi, uint16_t dataIn)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN; //Enable Rx and Tx
	_SPIC_TransferRaw16(spi, dataIn);
}*/

/*
 * Perform a blocking one-way receive, transmitting 0x00 and returning received data (8 bits)
 */
static inline uint8_t SPIC_Receive8(mxc_spi_regs_t* spi)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN; //Enable Rx and Tx
	return _SPIC_TransferRaw8(spi, 0x00);
}

/*
 * Perform a blocking one-way receive, transmitting 0x0000 and returning received data (16 bits)
 */
/*static inline uint16_t SPIC_Receive16(mxc_spi_regs_t* spi)
{
	spi->dma = MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN; //Enable Rx and Tx
	return _SPIC_TransferRaw16(spi, 0x0000);
}*/

#endif /* SPI_C_H_ */
