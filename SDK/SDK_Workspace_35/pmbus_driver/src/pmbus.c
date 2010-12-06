/*** PMbus function set *******************************************************
 This file contains a set of PMBus functions for the MSP430 master device.
 PEC functionality is included.

 P.Thanigai
 Texas Instruments Inc.
 December 2007
 Built with IAR Embedded Workbench Version: 4.09
 *******************************************************************************/

/**
 * Modified by Mohamed A. Bamakhrama <mohamed@liacs.nl> for usage in
 * Xilinx ML605 board which contains TI UCD9240 controller
 */

#include <xparameters.h>
#include <xbasic_types.h>
#include <xiic.h>
#include <xio.h>

#include "../include/pmbus.h"
#include "../include/util.h"

/* This is the PMBus command look-up table. Do not modify---------------------*/
const u8 list_of_commands[120] = {
		0x00, // dummy byte
		0x19, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x98,
		0x79, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
		0x93, 0x94, 0x95, 0x96, 0x97, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
		0xA7, 0xA8, 0xA9, 0x13, 0x14, 0x17, 0x18, 0x3, 0x11, 0x12, 0x15, 0x16,
		0x0, 0x1, 0x2, 0x4, 0x10, 0x20, 0x3A, 0x3D, 0x41, 0x45, 0x47, 0x49,
		0x4C, 0x50, 0x54, 0x56, 0x5A, 0x5C, 0x63, 0x69, 0x21, 0x22, 0x23, 0x24,
		0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x31, 0x32, 0x33, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3B, 0x3C, 0x3E, 0x3F, 0x40, 0x42, 0x43, 0x44, 0x46, 0x48,
		0x4A, 0x4B, 0x4F, 0x51, 0x52, 0x53, 0x55, 0x57, 0x58, 0x59, 0x5B, 0x5D,
		0x5E, 0x5F, 0x60, 0x61, 0x62, 0x64, 0x65, 0x66, 0x68, 0x6A, 0x6B };

int PMBusInit(void) {
	int err_code;

	/* Initialize the PMBus/IIC interface */
	err_code = XIic_DynInit(PMBUS_BASE_ADDRESS);
	if (err_code == XST_SUCCESS) {
		pmbus_debug("PMBus initialized successfully\r\n");
		return 0;
	} else {
		pmbus_debug("Failed to initialized PMBus!\r\n");
		return -1;
	}
}

/* Make sure all the Fifo's are cleared and the Bus is Not busy.
 Adapted from code in XIicLowLevelEepromDump in the iic_fan.c example */
void i2c_wait_until_ready(void) {
	u32 status;
	do {
		status = XIic_ReadReg(PMBUS_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
	} while (status & XIIC_SR_BUS_BUSY_MASK);
}

/*------------------------------------------------------------------------------
 unsigned short crc8MakeBitwise(...)
 IN: u8 CRC           => Initial crc value
 u8 Poly          => CRC polynomial
 unsigned int *Pmsg          => Pointer to input data stream
 u8 Msg_Size      => No. of bytes
 OUT: unsigned short CRC         => Result of CRC function

 ------------------------------------------------------------------------------*/
static unsigned short crc8MakeBitwise(u8 CRC, u8 Poly, u8 *Pmsg,
		unsigned int Msg_Size) {
	unsigned int i, j, carry;
	u8 msg;

	CRC = *Pmsg++; // first byte loaded in "crc"
	for (i = 0; i < Msg_Size - 1; i++) {
		msg = *Pmsg++; // next byte loaded in "msg"

		for (j = 0; j < 8; j++) {
			carry = CRC & 0x80; // ckeck if MSB=1
			CRC = (CRC << 1) | (msg >> 7); // Shift 1 bit of next byte into crc
			if (carry)
				CRC ^= Poly; // If MSB = 1, perform XOR
			msg <<= 1; // Shift left msg byte by 1
		}
	}
	// The previous loop computes the CRC of the input bit stream. To this,
	// 8 trailing zeros are padded and the CRC of the resultant value is
	// computed. This gives the final CRC of the input bit stream.
	for (j = 0; j < 8; j++) {
		carry = CRC & 0x80;
		CRC <<= 1;
		if (carry)
			CRC ^= Poly;
	}

	return (CRC);
}

/*------------------------------------------------------------------------------
 void PMBus_PEC(...)
 IN: u8 Command_Byte    => PMBus command index
 u8 RWFlag          => R=1/W=0
 unsigned int  Message         => PMBus transaction message
 u8 *Received_Value => Array to store received bytes
 OUT: u8 result         => Status of PEC validation
 ------------------------------------------------------------------------------*/
u8 PMBus_PEC(u8 Command_Byte, u8 RWFlag, unsigned int Message,
		u8 *Received_Value) {
	u8 ReceivedByteCount;
	u8 SentByteCount;
	u8 RecvBuf[2];
	u16 StatusReg;
	int i;

	u8 Ack_Status[1] = { 0 };
	u8 result = 0;
	unsigned int temp = 0;
	u8 transmit_buffer[4];
	u8 receive_buffer[5];
	u8 rxcount = 0;
	u8 index = 0;
	u8 command_group = 0;
	u8 crc_msg_size = 0;
	u8 crc_msg[5];
	u8 crc_master_generated = 0;
	u8 crc_slave_generated = 0;
	index = Command_Byte; // Store PMBus command byte as ...
	transmit_buffer[0] = list_of_commands[index]; //...1st byte in Tx buffer
	pmbus_debug("%x - %x\r\n", Command_Byte, transmit_buffer[0]);
	crc_msg[0] = PMBUS_MAIN_SPLY_ADDR << 1; // 1st CRC byte = slave address...
	temp = Message; // ...<< by 1 + R/W bit

	transmit_buffer[1] = Message; //store lower byte of message
	temp = (Message) >> 8;
	transmit_buffer[2] = temp; // store higher byte of message

	if (index > 0 && index < 13) //read byte from slave device
		command_group = READBYTE;
	if (index > 12 && index < 40) // read word from slave device
		command_group = READWORD;
	if (index > 39 && index < 44) // write byte to slave device
		command_group = WRITEBYTE;
	if (index > 43 && index < 49) // send byte to slave device
		command_group = SENDBYTE;
	/* Read or write one byte of data. R/W oprn. decided based on RWFlag *******/
	if (index > 48 && index < 69) // R/W byte
	{
		if (RWFlag == 0) // write byte
			command_group = WRITEBYTE;
		else
			// read byte
			command_group = READBYTE;
	}
	/* Read or write one word of data. R/W oprn. decided based on RWFlag *******/
	if (index > 68 && index < 120) // R/W Word
	{
		if (RWFlag == 0) // write word (new command group)
			command_group = WRITEWORD;
		else
			// read word
			command_group = READWORD;
	}
	if (index >= 120)
		while (1)
			; //illegal index - command invalid
	//-------------------------------------------------------------------------
	switch (command_group) {
	case READBYTE: // read byte from slave device
		rxcount = 1;
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(1,transmit_buffer,2,receive_buffer,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); //  Wait in LPM
		/* Assembling bit stream for CRC check*/
		crc_msg[1] = transmit_buffer[0]; // store first tx byte
		crc_msg[2] = (PMBUS_MAIN_SPLY_ADDR << 1) + 1;// store slave addres + R/W=1
		crc_msg[3] = receive_buffer[0]; // store rx byte 1
		crc_slave_generated = receive_buffer[1]; //store PEC byte from slave
		crc_msg_size = 4; // # of bytes
		/* CRC function call, generate CRC byte to compare with slave CRC*/
		crc_master_generated = crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY,
				crc_msg, crc_msg_size);
		if (crc_master_generated == crc_slave_generated)
			result = PEC_PASS; // PEC byte was validated
		else
			result = PEC_FAIL; // failed PEC test
		break;

	case READWORD: // read word
		rxcount = 2;
		pmbus_debug("ReadWord\r\n");
		i2c_wait_until_ready();
		/* Position the Read pointer to specific location in the IIC Device. */
		SentByteCount = 0;
		do {
			StatusReg = XIo_In8(PMBUS_BASE_ADDRESS + XIIC_SR_REG_OFFSET);
			if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK)) {
				SentByteCount = XIic_DynSend(PMBUS_BASE_ADDRESS,
						PMBUS_MAIN_SPLY_ADDR, transmit_buffer, 1,
						XIIC_REPEATED_START);
				//SentByteCount = XIic_DynSend(PMBUS_BASE_ADDRESS, PMBUS_MAIN_SPLY_ADDR, transmit_buffer, 1, XIIC_STOP);
			}
		} while (SentByteCount != 1);

		/* Receive the data. */
		ReceivedByteCount = XIic_DynRecv(PMBUS_BASE_ADDRESS,
				PMBUS_MAIN_SPLY_ADDR, receive_buffer, 2);

		pmbus_debug("ReceivedByteCount = %d\r\n",ReceivedByteCount)
		;
		/*print_buffer(receive_buffer, ReceivedByteCount);*/

		/* Assembling bit stream for CRC check*/
		crc_msg[1] = transmit_buffer[0];
		crc_msg[2] = (PMBUS_MAIN_SPLY_ADDR << 1) + 1; // store slave addres + R/W=1
		crc_msg[3] = receive_buffer[0];
		crc_msg[4] = receive_buffer[1];
		crc_slave_generated = receive_buffer[2]; //store PEC byte from slave
		crc_msg_size = 5;
		/* CRC function call, generate CRC byte to compare with slave CRC*/
		crc_master_generated = crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY,
				crc_msg, crc_msg_size);
		if (crc_master_generated == crc_slave_generated)
			result = PEC_PASS;
		else
			result = PEC_FAIL;
		break;

	case WRITEBYTE: // write byte
		/* CRC function call, generate CRC byte to transmit to slave device*/
		crc_msg[1] = transmit_buffer[0];
		crc_msg[2] = transmit_buffer[1];
		crc_msg_size = 3;
		crc_master_generated = crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY,
				crc_msg, crc_msg_size);
		transmit_buffer[2] = crc_master_generated;
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(3,transmit_buffer,0,0,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // LPM0
		if (Ack_Status[0] == 0) // Slave Acked the PEC byte
			result = PEC_PASS;
		else
			result = PEC_FAIL;

		break;
	case SENDBYTE: // send byte
		/* CRC function call, generate CRC byte to transmit to slave device*/
		crc_msg[1] = transmit_buffer[0];
		crc_msg_size = 2;
		crc_master_generated = crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY,
				crc_msg, crc_msg_size);
		transmit_buffer[1] = crc_master_generated;
		i2c_wait_until_ready();
		//while ( TI_USCI_I2C_NotReady());
		//TI_USCI_I2C_Transmit(2,transmit_buffer,0,0,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); //  LPM0
		//__no_operation();
		//__no_operation();
		if (Ack_Status[0] == 0) // Slave Acked the PEC byte
			result = PEC_PASS;
		else
			result = PEC_FAIL;

		break;
	case WRITEWORD: // write word
		/* CRC function call, generate CRC byte to transmit to slave device*/
		crc_msg[1] = transmit_buffer[0];
		crc_msg[2] = transmit_buffer[1];
		crc_msg[3] = transmit_buffer[2];
		crc_msg_size = 4;
		crc_master_generated = crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY,
				crc_msg, crc_msg_size);
		transmit_buffer[3] = crc_master_generated;
		i2c_wait_until_ready();
		if (Ack_Status[0] == 0)
			result = PEC_PASS; // Slave Acked the PEC byte
		else
			result = PEC_FAIL;
		break;

	default:
		break;
	}
	if (Received_Value != 0) {
		*Received_Value++ = receive_buffer[0];
		if (rxcount > 1)
			*Received_Value = receive_buffer[1];
	}
	return (result);
}

u8 PMBus(u8 Command_Byte, u8 RWFlag, unsigned int Message, u8 *Received_Value) {
	u8 ReceivedByteCount;
	u8 SentByteCount;
	u8 RecvBuf[2];
	u16 StatusReg;
	int i;

	u8 Ack_Status[1] = { 0 };
	u8 transmit_buffer[4];
	u8 receive_buffer[5];
	u8 rxcount = 0;
	unsigned int temp = 0;
	u8 index;
	u8 command_group;
	index = Command_Byte; // Store PMBus command byte as ...
	transmit_buffer[0] = list_of_commands[index]; //...1st byte in Tx buffer
	pmbus_debug("%x - %x\r\n", Command_Byte, transmit_buffer[0]);
	temp = Message;
	transmit_buffer[1] = Message; //store lower byte of message
	temp = (Message) >> 8;
	transmit_buffer[2] = temp; // store higher byte of message

	if (index > 0 && index < 13) //read byte from slave device
		command_group = READBYTE;
	if (index > 12 && index < 40) // read word from slave device
		command_group = READWORD;
	if (index > 39 && index < 44) // write byte to slave device
		command_group = WRITEBYTE;
	if (index > 43 && index < 49) // send byte to slave device
		command_group = SENDBYTE;
	/* Read or write one byte of data. R/W oprn. decided based on RWFlag *******/
	if (index > 48 && index < 69) {
		if (RWFlag == 0) // write byte
			command_group = WRITEBYTE;
		else
			// read byte
			command_group = READBYTE;
	}
	/* Read or write one word of data. R/W oprn. decided based on RWFlag *******/
	if (index > 68 && index < 120) // R/W Word
	{
		if (RWFlag == 0) // write word (new command group)
			command_group = WRITEWORD;
		else
			// read word
			command_group = READWORD;
	}
	if (index >= 120)
		while (1)
			; //illegal index - invalid command
	//switch(__even_in_range(command_group,8))
	switch (command_group) {
	case READBYTE: // read byte
		rxcount = 1;
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(1,transmit_buffer,1,receive_buffer,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // Wait in LPM
		break;

	case READWORD: // read word
		rxcount = 2;
		pmbus_debug("ReadWord\r\n")
		;
		i2c_wait_until_ready();

		/* Position the Read pointer to specific location in the IIC Device. */
		SentByteCount = 0;
		do {
			StatusReg = XIo_In8(PMBUS_BASE_ADDRESS + XIIC_SR_REG_OFFSET);
			if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK)) {
				SentByteCount = XIic_DynSend(PMBUS_BASE_ADDRESS,
						PMBUS_MAIN_SPLY_ADDR, transmit_buffer, 1,
						XIIC_REPEATED_START);
				//SentByteCount = XIic_DynSend(PMBUS_BASE_ADDRESS, PMBUS_MAIN_SPLY_ADDR, transmit_buffer, 1, XIIC_STOP);
			}
		} while (SentByteCount != 1);

		/* Receive the data. */
		ReceivedByteCount = XIic_DynRecv(PMBUS_BASE_ADDRESS,
				PMBUS_MAIN_SPLY_ADDR, receive_buffer, 2);

		pmbus_debug("ReceivedByteCount = %d\r\n",ReceivedByteCount)
		;
		/*print_buffer(receive_buffer, ReceivedByteCount);*/

		//TI_USCI_I2C_Transmit(1,transmit_buffer,2,receive_buffer,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // Wait in LPM
		break;

	case WRITEBYTE: // write byte
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(2,transmit_buffer,0,0,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // Wait in LPM
		break;
	case SENDBYTE: // send byte
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(1,transmit_buffer,0,0,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // Wait in LPM
		break;
	case WRITEWORD: // write word
		i2c_wait_until_ready();
		//TI_USCI_I2C_Transmit(3,transmit_buffer,0,0,Ack_Status);
		//__bis_SR_register(LPM0_bits + GIE); // Wait in LPM
		break;
	default:
		break;
	}
	if (Received_Value != 0) {
		*Received_Value++ = receive_buffer[0];
		if (rxcount > 1)
			*Received_Value = receive_buffer[1];
	}
	return (Ack_Status[0]);
}

