/*
 * i2c_c.h
 *
 *  Created on: 5 Mar 2024
 *      Author: mhrehor
 *      VERSION: 1.0.1
 */

#ifndef I2C_C_H_
#define I2C_C_H_

#include <stdint.h>
#include "i2c.h"

/*ERROR CODES:
	0  - OK - No error occurred.
	8  - ARB_ERR - Arbitration lost (Master is unable to assert control of the bus).
	9  - TO_ERR - Timeout error occurred.
	10 - ADDR_NACK_ERR - Address NACK received from a slave.
	11 - DATA_ERR - Data NACK received from a slave.
*/


// =================================================================================
// ===================================== BASIC =====================================
// =================================================================================

/*
 * Get the result of the latest transaction (returns 0 if OK, otherwise error code, see error codes above)
 */
inline uint8_t GetStatus(mxc_i2c_regs_t* i2c)
{
	uint8_t flagPos = MXC_F_I2C_INTFL0_ARB_ERR_POS;
	while(flagPos <= MXC_F_I2C_INTFL0_DATA_ERR_POS) //Check error status bits
	{
		if(i2c->intfl0 & (1UL << flagPos))
			return flagPos; //Return error code if error occurred
		flagPos++; //next error bit
	}
	return 0; //Return OK
}
/*
 * Start transaction, issue a START condition
 */
void I2CC_IssueStart(mxc_i2c_regs_t* i2c)
{
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_START;
}
/*
 * Stop transaction, issue a STOP condition
 */
void I2CC_IssueStop(mxc_i2c_regs_t* i2c)
{
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP;
}
/*
 * Issue a Repeated START condition
 */
void I2CC_IssueRestart(mxc_i2c_regs_t* i2c)
{
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_RESTART;
}
/*
 * Halt program execution until the current transaction completes
*/
inline void I2CC_WaitUntilComplete(mxc_i2c_regs_t* i2c)
{
	while((i2c->status & MXC_F_I2C_STATUS_BUSY)); //wait until busy flag is cleared
}

/*
 * Read (unload) a single byte from the RX FIFO (A transaction must be first performed to receive the data from the slave)
 */
inline uint8_t I2CC_Read(mxc_i2c_regs_t* i2c)
{
	return i2c->fifo;
}

/*
 * Read (unload) a single byte from the RX FIFO *if available* (A transaction must be first performed to receive the data from the slave)
 * outData: pointer to byte that will be set to the read data value
 * Returns: 1 if data was read, 0 if no further data is stored in the FIFO
 */
inline uint8_t I2CC_ReadAvailable(mxc_i2c_regs_t* i2c, uint8_t* outData)
{
	if(i2c->status & MXC_F_I2C_STATUS_RX_EM) return 0;
	*outData = i2c->fifo;
	return 1;
}


// ================================================================================
// ================================= TRANSACTIONS =================================
// ================================================================================

/*
 * Initialize a send operation (address is 7-bit - !!! WITHOUT R/W padding/bit !!!)
 * Any data currently loaded in TX FIFO will be deleted
 */
inline void I2CC_InitSend(mxc_i2c_regs_t* i2c, uint8_t addr)
{
	i2c->txctrl0 |= MXC_F_I2C_TXCTRL0_FLUSH; //Flush TX FIFO
	while(i2c->txctrl0 & MXC_F_I2C_TXCTRL0_FLUSH); //Wait until flush is complete
	i2c->fifo = (addr << 1); //Pad slave address with 0 (W) and submit to FIFO
}

/*
 * Loads a byte to TX FIFO - to be send during upcoming transaction (assuming I2CC_InitSend has been issued successfully)
 * Maximum of 7 bytes can be loaded
 * Returns: 0 if success, 1 if failed (TX FIFO is full)
 */
inline uint8_t I2CC_LoadByte(mxc_i2c_regs_t* i2c, uint8_t data)
{
	if(i2c->status & MXC_F_I2C_STATUS_TX_FULL) return 0;
	i2c->fifo = data; //Load data to TX FIFO
	return 1;
}

/*
 * Perform an asynchronous write transaction, sending all bytes currently loaded in TX FIFO and issue an automatic STOP condition when done
 * (program execution will continue regardless of transaction status)
 */
void I2CC_DoWriteAsync(mxc_i2c_regs_t* i2c)
{
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_START; //Start transaction
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP; //Set STOP flag (this will issue a STOP condition after entire content of FIFO is transmitted)
}

/*
 * Perform a blocking write transaction, sending all bytes currently loaded in TX FIFO and issue an automatic STOP condition when done
 * (program execution will halt here until the transaction is complete)
 */
void I2CC_DoWrite(mxc_i2c_regs_t* i2c)
{
	I2CC_DoWriteAsync(i2c);
	I2CC_WaitUntilComplete(i2c);
}

/*
 * Perform an asynchronous receive operation (address is 7-bit - !!! WITHOUT R/W padding/bit !!!)
 * (program execution will continue regardless of the transaction status)
 * Any remaining data left in FIFO (both TX and RX) will be deleted
 * Received data will be available in the RX FIFO (I2CC_Read()) as the transaction progresses
 * count: Number of bytes to receive (1-255 - normal; value of 0 means receive 256 bytes)
 */
void I2CC_DoReceiveAsync(mxc_i2c_regs_t* i2c, uint8_t addr, uint8_t count)
{
	//Clear TX and RX FIFO
	i2c->txctrl0 |= MXC_F_I2C_TXCTRL0_FLUSH; //Flush TX FIFO
	i2c->rxctrl0 |= MXC_F_I2C_RXCTRL0_FLUSH; //Flush RX FIFO
	while(i2c->txctrl0 & MXC_F_I2C_TXCTRL0_FLUSH); //Wait until TX flush is complete
	while(i2c->rxctrl0 & MXC_F_I2C_RXCTRL0_FLUSH); //Wait until RX flush is complete

	i2c->rxctrl1 = count; //Set count of bytes to receive
	i2c->fifo = (addr << 1) | 1; //Submit slave address to FIFO, padded with read bit (1)
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_START; //Issue a START
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP; //Set STOP flag, such that an automatic STOP condition will be issued when the transaction is complete
}

/*
 * Perform a blocking receive operation (address is 7-bit - !!! WITHOUT R/W padding/bit !!!)
 * (program execution will halt here until the transaction is complete)
 * Any remaining data left in FIFO (both TX and RX) will be deleted
 * Received data will be available in the RX FIFO (I2CC_Read()) once the transaction completes
 * count: Number of bytes to receive (1-255 - normal; value of 0 means receive 256 bytes)
 */
void I2CC_DoReceive(mxc_i2c_regs_t* i2c, uint8_t addr, uint8_t count)
{
	I2CC_DoReceiveAsync(i2c, addr, count);
	I2CC_WaitUntilComplete(i2c);
}


// ====================================================================================================
// ================================= SLAVE-DEVICE REGISTER OPERATIONS =================================
// ====================================================================================================

/*
 * Perform a blocking standard write to 8-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
void I2CC_WriteRegister8(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr, uint8_t data)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_LoadByte(i2c, data);
	I2CC_DoWrite(i2c);
}

/*
 * Perform a blocking little-endian write to 16-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
void I2CC_WriteRegister16LE(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr, uint16_t data)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_LoadByte(i2c, data & 0xff);
	I2CC_LoadByte(i2c, (data >> 8) & 0xff);
	I2CC_DoWrite(i2c);
}

/*
 * Perform a blocking big-endian write to 16-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
void I2CC_WriteRegister16BE(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr, uint16_t data)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_LoadByte(i2c, (data >> 8) & 0xff);
	I2CC_LoadByte(i2c, data & 0xff);
	I2CC_DoWrite(i2c);
}


/*
 *  ████    ███   █   █   ███   ████  ████
 *  █   █  █   █  ██  █  █      █     █   █
 *  █   █  █████  █ █ █  █  ██  ████  █   █
 *  █   █  █   █  █  ██  █   █  █     ████
 *  ████   █   █  █   █   ███   ████  █  ██
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ███████████████████████████████████████████████████████
 * ██ I HAVE NO IDEA IF THE READ OPs WILL ACTUALLY WORK ██
 * ███████████████████████████████████████████████████████
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

/*
 * Perform a blocking standard write to 8-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
uint8_t I2CC_ReadRegister8(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_IssueStart(i2c);
	I2CC_IssueRestart(i2c);
	while(i2c->mstctrl & MXC_F_I2C_MSTCTRL_RESTART); //Wait until the repeated start is issued
	i2c->rxctrl1 = 1; //Set count of bytes to receive
	i2c->fifo = (deviceAddr << 1) | 1; //Submit slave address to FIFO, padded with read bit (1)
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP; //Set STOP flag, such that an automatic STOP condition will be issued when the transaction is complete
	I2CC_WaitUntilComplete(i2c);
	return I2CC_Read(i2c);
}

/*
 * Perform a blocking little-endian write to 16-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
uint16_t I2CC_ReadRegister16LE(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_IssueStart(i2c);
	I2CC_IssueRestart(i2c);
	while(i2c->mstctrl & MXC_F_I2C_MSTCTRL_RESTART); //Wait until the repeated start is issued
	i2c->rxctrl1 = 2; //Set count of bytes to receive
	i2c->fifo = (deviceAddr << 1) | 1; //Submit slave address to FIFO, padded with read bit (1)
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP; //Set STOP flag, such that an automatic STOP condition will be issued when the transaction is complete
	I2CC_WaitUntilComplete(i2c);
	uint16_t rxdata0 = I2CC_Read(i2c);
	uint16_t rxdata1 = I2CC_Read(i2c);
	return rxdata0 | (rxdata1 << 8);
}

/*
 * Perform a blocking big-endian write to 16-bit slave device register
 * deviceAddr: I2C address of the slave device
 * regAddr: 8-bit address of the register within the slave device
 * data: Data to be written to the slave device
 */
uint16_t I2CC_ReadRegister16BE(mxc_i2c_regs_t* i2c, uint8_t deviceAddr, uint8_t regAddr)
{
	I2CC_InitSend(i2c, deviceAddr);
	I2CC_LoadByte(i2c, regAddr);
	I2CC_IssueStart(i2c);
	I2CC_IssueRestart(i2c);
	while(i2c->mstctrl & MXC_F_I2C_MSTCTRL_RESTART); //Wait until the repeated start is issued
	i2c->rxctrl1 = 2; //Set count of bytes to receive
	i2c->fifo = (deviceAddr << 1) | 1; //Submit slave address to FIFO, padded with read bit (1)
	i2c->mstctrl |= MXC_F_I2C_MSTCTRL_STOP; //Set STOP flag, such that an automatic STOP condition will be issued when the transaction is complete
	I2CC_WaitUntilComplete(i2c);
	uint16_t rxdata1 = I2CC_Read(i2c);
	uint16_t rxdata0 = I2CC_Read(i2c);
	return rxdata0 | (rxdata1 << 8);
}

#endif /* I2C_C_H_ */
