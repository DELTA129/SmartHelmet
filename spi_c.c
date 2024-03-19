/*
 * spi_c.c
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#include "spi_c.h"

#include <stdio.h> //TEPORARY
#include "mxc_delay.h"

uint8_t _SPIC_TransferRaw8(mxc_spi_regs_t* spi, uint8_t dataIn)
{
	spi->fifo8[0] = dataIn; //Write data to FIFO
	//Set slave and start transmission
	spi->ctrl1 = 1 | (1 << 16); //Set num of tx/rx characters to 1
	spi->ctrl0 = MXC_F_SPI_CTRL0_EN |  MXC_F_SPI_CTRL0_MST_MODE | MXC_F_SPI_CTRL0_START;
	while(!(spi->intfl & MXC_F_SPI_INTFL_RX_THD)); //Wait until RxFIFO is loaded
	while(!(spi->intfl & MXC_F_SPI_INTFL_MST_DONE)); //Wait until transmission is done
	uint8_t rxData = spi->fifo8[0]; //Load Rx byte from FIFO
	spi->intfl = MXC_F_SPI_INTFL_MST_DONE | MXC_F_SPI_INTFL_RX_THD; //Clear transmission done and RxFIFO THD flag
	return rxData;
}

