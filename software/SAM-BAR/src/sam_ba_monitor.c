/**
 * \file
 *
 * \brief Monitor functions for SAM-BA on SAM0
 * Port of rom monitor functions from legacy sam-ba software
 *
 * Copyright (c) 2015-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

#include <asf.h>
#include "sam_ba_monitor.h"

const char RomBOOT_Version[] = SAM_BA_VERSION;


/* b_terminal_mode mode (ascii) or hex mode */
volatile bool b_terminal_mode = false;

uint32_t PAGE_SIZE, PAGES, MAX_FLASH;


/**
 * \brief This function initializes the SAM-BA monitor
 *
 * \param com_interface  Communication interface to be used.
 */
void sam_ba_monitor_init(void)
{
	//const uint32_t pageSizes[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };
	//PAGE_SIZE = pageSizes[NVMCTRL->PARAM.bit.PSZ];
	PAGE_SIZE = 8 << NVMCTRL->PARAM.bit.PSZ;
	
	PAGES = NVMCTRL->PARAM.bit.NVMP;
	MAX_FLASH = PAGE_SIZE * PAGES;	
}

/**
 * Write to flash
 * size in bytes. Must be a multiple of 4
 */
void flash_write_to(uint32_t *dst_addr, uint32_t *src_addr, uint32_t size)
{
	uint32_t i;	
	
	size /= 4;

	// Set automatic page write
	NVMCTRL->CTRLB.bit.MANW = 0;

	// Do writes in pages
	while (size)
	{
		// Execute "PBC" Page Buffer Clear
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
		while (NVMCTRL->INTFLAG.bit.READY == 0)
			;

		// Fill page buffer
		for (i=0; i<(PAGE_SIZE/4) && i<size; i++)
		{
			dst_addr[i] = src_addr[i];
		}

		// Execute "WP" Write Page
		NVMCTRL->ADDR.reg = ((uint32_t)dst_addr) / 2;
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
		while (NVMCTRL->INTFLAG.bit.READY == 0)
			;

		// Advance to next page
		dst_addr += i;
		src_addr += i;
		size     -= i;
	}
}

/**
 * Erase flash
 * size in bytes. should be a multiple of the row size
 */
void flash_erase(uint32_t dst_addr, int32_t size)
{					
	while (size > 0)
	{
		if(dst_addr >= APP_START_ADDRESS && dst_addr < MAX_FLASH)   // protect the bootloader
		{	
			// Execute "ER" Erase Row
			NVMCTRL->ADDR.reg = dst_addr / 2;
			NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
			while (NVMCTRL->INTFLAG.bit.READY == 0)
				;
		}
		
		dst_addr += PAGE_SIZE * 4; // Skip a ROW
		size -= PAGE_SIZE * 4;
	}
}

static uint16_t crc16(const uint8_t* data_p, uint32_t length)
{
	uint16_t x; 
	uint16_t crc = 0;

	while (length--)
	{
		x = crc >> 8 ^ *data_p++;
		x ^= x>>4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
	}
	return crc;
}

// Prints a 32-bit integer in hex.
static void put_uint32(uint32_t n)
{
	char buff[8];
	int i;
	for (i=0; i<8; i++)
	{
		int d = n & 0XF;
		n = (n >> 4);

		buff[7-i] = d > 9 ? 'A' + d - 10 : '0' + d;
	}
	//ptr_monitor_if->putdata(buff, 8);
	udi_cdc_write_buf(buff, 8);
}

/**
 * \brief This function allows data rx by USART
 *
 * \param *data  Data pointer
 * \param length Length of the data
 */
void sam_ba_putdata_term(uint8_t* data, uint32_t length)
{
	uint8_t temp, buf[12], *data_ascii;
	uint32_t i, int_value;

	if (b_terminal_mode)
	{
		if (length == 4)
			int_value = *(uint32_t *) data;
		else if (length == 2)
			int_value = *(uint16_t *) data;
		else
			int_value = *(uint8_t *) data;

		data_ascii = buf + 2;
		data_ascii += length * 2 - 1;

		for (i = 0; i < length * 2; i++)
		{
			temp = (uint8_t) (int_value & 0xf);

			if (temp <= 0x9)
				*data_ascii = temp | 0x30;
			else
				*data_ascii = temp + 0x37;

			int_value >>= 4;
			data_ascii--;
		}
		buf[0] = '0';
		buf[1] = 'x';
		buf[length * 2 + 2] = '\n';
		buf[length * 2 + 3] = '\r';
		//ptr_monitor_if->putdata(buf, length * 2 + 4);
		udi_cdc_write_buf(buf, length * 2 + 4);
	}
	else
	{
		//ptr_monitor_if->putdata(data, length);
		udi_cdc_write_buf(data, length);
	}
	return;
}

volatile uint32_t sp;
/**
 * \brief Execute an applet from the specified address
 *
 * \param address Applet address
 */
void call_applet(uint32_t address)
{
	uint32_t app_start_address;

	cpu_irq_disable();

	sp = __get_MSP();

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) address);

	/* Load the Reset Handler address of the application */
	app_start_address = *(uint32_t *)(address + 4);

	/* Jump to application Reset Handler in the application */
	asm("bx %0"::"r"(app_start_address));
}


uint32_t current_number;
uint32_t i, length;
uint8_t *ptr, data[SIZEBUFMAX * 8];  // size of 8 endpoints
uint8_t j;
uint32_t u32tmp;

uint8_t *ptr_data = NULL;
uint8_t command = 'z';


// BOSSA uses a fixed APP_START_ADDRESS (0x2000) at the moment. If the bootloader is bigger, we have
// to add an offset to get the right target address. 
uint32_t additional_offset = 0;
// Detect first uploaded block after flash erase to calculate the additional_offset
bool start_upload_flag = true;

/**
 * \brief This function starts the SAM-BA monitor.
 */
void sam_ba_monitor_run(void)
{
	//length = ptr_monitor_if->getdata(data, sizeof(data));
	length = udi_cdc_read_no_polling(data, sizeof(data));

	ptr = data;
	for (i = 0; i < length; i++)
	{
		if (*ptr != 0xff)
		{
			if (*ptr == '#')
			{
				if (b_terminal_mode)
				{
					//ptr_monitor_if->putdata("\n\r", 2);
					udi_cdc_write_buf("\n\r", 2);
				}
				if (command == 'S')
				{
					// Check if some data are remaining in the "data" buffer
					if(length>i)
					{
						// Move current indexes to next avail data (currently ptr points to "#")
						ptr++;
						i++;
						// We need to add first the remaining data of the current buffer already read from usb
						// read a maximum of "current_number" bytes
						u32tmp = min((length-i), current_number);

						for(j=0; j<u32tmp; j++)
						{
							*ptr_data = *ptr;
							ptr_data++;
							ptr++;
							i++;
						}
					}
					// update i with the data read from the buffer
					i--;
					ptr--;
					// Do we expect more data ?
					if(j < current_number)
					{
						//ptr_monitor_if->getdata_xmd(ptr_data, current_number-j);
						udi_cdc_read_buf(ptr_data, current_number-j);
					}

				}
				else if (command == 'R')
				{
					//ptr_monitor_if->putdata_xmd(ptr_data, current_number);
					//ptr_monitor_if->putdata(ptr_data, current_number); // usb = no xmodem
					udi_cdc_write_buf(ptr_data, current_number);
				}
				else if (command == 'O')
				{
					*ptr_data = (char) current_number;
				}
				else if (command == 'H')
				{
					*((uint16_t *) ptr_data) = (uint16_t) current_number;
				}
				else if (command == 'W')
				{
					*((int *) ptr_data) = current_number;
				}
				else if (command == 'o')
				{
					sam_ba_putdata_term(ptr_data, 1);
				}
				else if (command == 'h')
				{
					current_number = *((uint16_t *) ptr_data);
					sam_ba_putdata_term((uint8_t*) &current_number, 2);
				}
				else if (command == 'w')
				{
					current_number = *((uint32_t *) ptr_data);
					sam_ba_putdata_term((uint8_t*) &current_number, 4);
				}
				else if (command == 'G')
				{
					call_applet(current_number);
					/* Rebase the Stack Pointer */
					__set_MSP(sp);
					cpu_irq_enable();
				}
				else if (command == 'T')
				{
					b_terminal_mode = 1;
					//ptr_monitor_if->putdata("\n\r", 2);
					udi_cdc_write_buf("\n\r", 2);
				}
				else if (command == 'N')
				{
					if (b_terminal_mode == 0)
					{
						//ptr_monitor_if->putdata("\n\r", 2);
						udi_cdc_write_buf("\n\r", 2);
					}
					b_terminal_mode = 0;
				}
				else if (command == 'V')
				{
					//ptr_monitor_if->putdata("v", 1);
					udi_cdc_write_buf("v", 1);
					//ptr_monitor_if->putdata((uint8_t *) RomBOOT_Version, strlen(RomBOOT_Version));
					udi_cdc_write_buf((uint8_t *) RomBOOT_Version, strlen(RomBOOT_Version));
					//ptr_monitor_if->putdata(" ", 1);
					udi_cdc_write_buf(" ", 1);
					ptr = (uint8_t*) &(__DATE__);
					i = 0;
					while (*ptr++ != '\0')
						i++;
					//ptr_monitor_if->putdata((uint8_t *) &(__DATE__), i);
					udi_cdc_write_buf((uint8_t *) &(__DATE__), i);
					//ptr_monitor_if->putdata(" ", 1);
					udi_cdc_write_buf(" ", 1);
					i = 0;
					ptr = (uint8_t*) &(__TIME__);
					while (*ptr++ != '\0')
						i++;
					//ptr_monitor_if->putdata((uint8_t *) &(__TIME__), i);
					udi_cdc_write_buf((uint8_t *) &(__TIME__), i);
					//ptr_monitor_if->putdata("\n\r", 2);
					udi_cdc_write_buf("\n\r", 2);
				}
				else if (command == 'X') // Arduino extension
				{
					// Syntax: X[ADDR]#
					// Erase the flash memory starting from ADDR to the end of flash.

					// Note: the flash memory is erased in ROWS, that is in block of 4 pages.
					//       Even if the starting address is the last byte of a ROW the entire
					//       ROW is erased anyway.

					flash_erase(current_number, MAX_FLASH - current_number);

					// Notify command completed
					//ptr_monitor_if->putdata("X\n\r", 3);
					udi_cdc_write_buf("X\n\r", 3);

					start_upload_flag = true;			// set flag 
				}
				else if (command == 'Y')  // Arduino extension
				{
					// This command writes the content of a buffer in SRAM into flash memory.

					// Syntax: Y[ADDR],0#
					// Set the starting address of the SRAM buffer.

					// Syntax: Y[ROM_ADDR],[SIZE]#
					// Write the first SIZE bytes from the SRAM buffer (previously set) into
					// flash memory starting from address ROM_ADDR

					static uint32_t *src_buff_addr = NULL;

					if (current_number == 0)
					{
						// Set buffer address
						src_buff_addr = (uint32_t*)ptr_data;
					}
					else
					{
						// BOSSA uses a fixed APP_START_ADDRESS (0x2000) at the moment. If the bootloader is bigger, we have 
						// to add an offset to get the right flash address.
						if(start_upload_flag)
						{
							start_upload_flag = false;
							if((uint32_t)ptr_data < APP_START_ADDRESS)
								additional_offset = APP_START_ADDRESS - (uint32_t)ptr_data;
							else
								additional_offset = 0;
						}

						// Write to flash
						flash_write_to((uint32_t*)((uint32_t)ptr_data + additional_offset), src_buff_addr, current_number);
					}

					// Notify command completed
					//ptr_monitor_if->putdata("Y\n\r", 3);
					udi_cdc_write_buf("Y\n\r", 3);
				}
				else if (command == 'Z')  // Arduino extension
				{
					// This command calculate CRC for a given area of memory.
					// It's useful to quickly check if a transfer has been done
					// successfully.

					// Syntax: Z[START_ADDR],[SIZE]#
					// Returns: Z[CRC]#

					uint8_t *data2 = (uint8_t *)(ptr_data + additional_offset);
					uint16_t crc = crc16(data2, current_number);

					// Send response
					//ptr_monitor_if->putdata("Z", 1);
					udi_cdc_write_buf("Z", 1);
					put_uint32(crc);
					//ptr_monitor_if->putdata("#\n\r", 3);
					udi_cdc_write_buf("#\n\r", 3);
				}

				command = 'z';
				current_number = 0;

				if (b_terminal_mode)
				{
					//ptr_monitor_if->putdata(">", 1);
					udi_cdc_write_buf(">", 1);
				}
			}
			else
			{
				if (('0' <= *ptr) && (*ptr <= '9'))
				{
					current_number = (current_number << 4) | (*ptr - '0');

				}
				else if (('A' <= *ptr) && (*ptr <= 'F'))
				{
					current_number = (current_number << 4)
							| (*ptr - 'A' + 0xa);

				}
				else if (('a' <= *ptr) && (*ptr <= 'f'))
				{
					current_number = (current_number << 4)
							| (*ptr - 'a' + 0xa);

				}
				else if (*ptr == ',')
				{
					ptr_data = (uint8_t *) current_number;
					current_number = 0;

				}
				else
				{
					command = *ptr;
					current_number = 0;
				}
			}
			ptr++;
		}
	}
}
