/*
 * @file trx_access.c
 *
 * @brief Performs interface functionalities between the PHY layer and ASF
 * drivers
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "Components_Config.h"
#include "Components_Types.h"
#include "trx_access.h"
#include "spi.h"
#include "hermes-time.h"

extern REG_SPI_T *RF_SPI;

static irq_handler_t irq_hdl_trx = NULL;
static volatile bool irq_hdl_trx_is_enable = true;

void trx_spi_init(void)
{

}

void PhyReset(void)
{
	/* Ensure control lines have correct levels. */
	TRX_RST_HIGH();
	TRX_SLP_TR_LOW();

	/* Wait typical time of timer TR1. */
	TRX_DELAY(330);

	TRX_RST_LOW();
	TRX_DELAY(10);
	TRX_RST_HIGH();
}

void trx_irq_handler(void)
{
	if (irq_hdl_trx && irq_hdl_trx_is_enable)
	{
		irq_hdl_trx();
	}
}

void trx_enable_irq_handler(void)
{
	irq_hdl_trx_is_enable = true;
}

void trx_disable_irq_handler(void)
{
	irq_hdl_trx_is_enable = false;
}

int trx_spi_transfer(uint8_t* data, uint16_t size)
{
	RF_SPI->Control2.CurrentDataSize = size;
	RF_SPI->Control1.SpiEnable = true;
	TRX_SELECT_DEVICE();
	delay_us(1);
	RF_SPI->Control1.MasterTransferStart = true;

	while (size--)
	{
		while(!RF_SPI->Status.TxEmpty)
		{
			asm("nop");
		};

		RF_SPI->TxData.Byte = *data;

		while(!RF_SPI->Status.RxNotEmpty)
		{
			asm("nop");
		};

		*data = RF_SPI->RxData.Byte;

		data++;
	}

	RF_SPI->Clear.EndOfTransfer = true;
	RF_SPI->Clear.TransferFilled = true;
	RF_SPI->Control1.SpiEnable = false;

	delay_us(1);
	TRX_DESELECT_DEVICE();

	return 0;
}

uint8_t trx_reg_read(uint8_t addr)
{
	uint8_t register_value = 0;
	HAL_StatusTypeDef result = HAL_OK;

	//Saving the current interrupt status & disabling the global interrupt
	ENTER_TRX_CRITICAL_REGION();

	uint8_t packet[2] = { addr | READ_ACCESS_COMMAND };

	trx_spi_transfer(packet, sizeof(packet));

	register_value = packet[1];

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	return result == HAL_OK ? register_value : 0;
}

void trx_reg_write(uint8_t addr, uint8_t data)
{
	HAL_StatusTypeDef result;
	uint8_t packet[2] = { addr | WRITE_ACCESS_COMMAND, data };

	//Saving the current interrupt status & disabling the global interrupt
	ENTER_TRX_CRITICAL_REGION();

	trx_spi_transfer(packet, sizeof(packet));

	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

void trx_irq_init(FUNC_PTR trx_irq_cb)
{
	/*
	 * Set the handler function.
	 * The handler is set before enabling the interrupt to prepare for
	 * spurious
	 * interrupts, that can pop up the moment they are enabled
	 */
	irq_hdl_trx = (irq_handler_t)trx_irq_cb;
}

uint8_t trx_bit_read(uint8_t addr, uint8_t mask, uint8_t pos)
{
	uint8_t ret;
	ret = trx_reg_read(addr);
	ret &= mask;
	ret >>= pos;
	return ret;
}

void trx_bit_write(uint8_t reg_addr, uint8_t mask, uint8_t pos,
		uint8_t new_value)
{
	uint8_t current_reg_value;
	current_reg_value = trx_reg_read(reg_addr);
	current_reg_value &= ~mask;
	new_value <<= pos;
	new_value &= mask;
	new_value |= current_reg_value;
	trx_reg_write(reg_addr, new_value);
}

void trx_frame_read(uint8_t *data, uint8_t length)
{
	HAL_StatusTypeDef result;
	uint8_t command = TRX_CMD_FR;
	uint8_t buffer[0xff];

	buffer[0] = command;

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	memcpy(buffer + 1, data, length);

	trx_spi_transfer(buffer, sizeof(command) + length);

	memcpy(data, buffer + 1, length);

	/* Stop the SPI transaction by setting SEL high */
	//TRX_DESELECT_DEVICE();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

void trx_frame_write(uint8_t *data, uint8_t length)
{
	HAL_StatusTypeDef result;
	uint8_t command = TRX_CMD_FW;
	uint8_t buffer[0xff];

	buffer[0] = TRX_CMD_FW;

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	memcpy(buffer + sizeof(command), data, length);

	trx_spi_transfer(buffer, sizeof(command) + length);

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

/**
 * @brief Writes data into SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver
 *
 * @param addr Start address in the SRAM for the write operation
 * @param data Pointer to the data to be written into SRAM
 * @param length Number of bytes to be written into SRAM
 */
void trx_sram_write(uint8_t addr, uint8_t *data, uint8_t length)
{
	HAL_StatusTypeDef result;
	uint8_t command = TRX_CMD_SW;

	uint8_t buffer[0xff];
	buffer[0] = command;
	buffer[1] = addr;

	memcpy(buffer + sizeof(command) + sizeof(addr), data, length);

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	trx_spi_transfer(buffer, length + sizeof(command) + sizeof(addr));

	/* Stop the SPI transaction by setting SEL high */
	TRX_DESELECT_DEVICE();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

/**
 * @brief Reads data from SRAM of the transceiver
 *
 * This function reads from the SRAM of the transceiver
 *
 * @param[in] addr Start address in SRAM for read operation
 * @param[out] data Pointer to the location where data stored
 * @param[in] length Number of bytes to be read from SRAM
 */
void trx_sram_read(uint8_t addr, uint8_t *data, uint8_t length)
{
	HAL_StatusTypeDef result;
	uint8_t command = TRX_CMD_SR;

	uint8_t buffer[0xff];
	buffer[0] = command;
	buffer[1] = addr;

	memcpy(buffer + sizeof(command) + sizeof(addr), data, length);

	delay_us(1);

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	trx_spi_transfer(buffer, length + sizeof(command) + sizeof(addr));

	memcpy(data, buffer + sizeof(command) + sizeof(addr), length);

	/* Stop the SPI transaction by setting SEL high */
	TRX_DESELECT_DEVICE();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

/**
 * @brief Writes and reads data into/from SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver and
 * simultaneously reads the bytes.
 *
 * @param addr Start address in the SRAM for the write operation
 * @param idata Pointer to the data written/read into/from SRAM
 * @param length Number of bytes written/read into/from SRAM
 */
void trx_aes_wrrd(uint8_t addr, uint8_t *idata, uint8_t length)
{
	HAL_StatusTypeDef result;
	uint8_t *odata;
	uint8_t temp = TRX_CMD_SW;

	TRX_DELAY(1); /* wap_rf4ce */

	ENTER_TRX_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
	TRX_SELECT_DEVICE();

	/* Send the command byte */
	result = HAL_SPI_Transmit(&hspi3, &temp, sizeof(temp), 100);

	/* write SRAM start address */
	result = HAL_SPI_Transmit(&hspi3, &addr, sizeof(addr), 100);

	/* now transfer data */
	odata = idata;

	result = HAL_SPI_TransmitReceive(&hspi3, idata, odata, length, 100);

	/* Stop the SPI transaction by setting SEL high */
	TRX_DESELECT_DEVICE();

	LEAVE_TRX_CRITICAL_REGION();

	UNUSED(result);
}

void trx_spi_disable(void)
{

}

void trx_spi_enable(void)
{

}
