/**
 * \file
 *
 * \brief Main functions for SAM-BAR
 *
 * Portitions of this code are:
 * Copyright (c) 2016-2017 Watterott electronic (www.watterott.com)
 * Copyright (c) 2014-2016 Justin Mattair (www.mattairtech.com)
 * Copyright (c) 2015 Arduino LLC (www.arduino.cc)
 * Copyright (c) 2011-2015 Atmel Corporation (www.atmel.com)
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this 
 *   list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * - Neither the name of the copyright holders nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <asf.h>
#include "sam_ba_monitor.h"

static volatile bool main_b_msc_enable = false;
static volatile bool main_b_cdc_enable = false;


/**
 * \brief Check and set boot protection fuse bits.
 *
 */
static void check_boot_protection(void)
{
	uint32_t bp, bp_new, f0, f1;

	#define exec_nvm_cmd(cmd)                                         \
			do{                                                       \
				NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;           \
				NVMCTRL->ADDR.reg    = (uint32_t)NVMCTRL_USER / 2;    \
				NVMCTRL->CTRLA.reg   = NVMCTRL_CTRLA_CMDEX_KEY | cmd; \
				while(NVMCTRL->INTFLAG.bit.READY == 0);               \
			}while(0)

	while(NVMCTRL->INTFLAG.bit.READY == 0);

	f0 = *((uint32_t *)NVMCTRL_AUX0_ADDRESS);
	f1 = *(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1);

	bp = (f0 & NVMCTRL_FUSES_BOOTPROT_Msk) >> NVMCTRL_FUSES_BOOTPROT_Pos; // get current bits

	#if   APP_START_ADDRESS == 0x1000
		bp_new = 3; // 4k
	#elif APP_START_ADDRESS == 0x2000
		bp_new = 2; // 8k
	#elif APP_START_ADDRESS == 0x4000
		bp_new = 1; // 16k
	#elif APP_START_ADDRESS == 0x8000
		bp_new = 0; // 32k
	#else
		bp_new = 7; // no protection
	#endif

	if(bp == bp_new) // bits already set
	{
		return;
	}

	f0 = (f0 & ~NVMCTRL_FUSES_BOOTPROT_Msk) | (bp_new << NVMCTRL_FUSES_BOOTPROT_Pos);

	NVMCTRL->CTRLB.reg = NVMCTRL->CTRLB.reg | NVMCTRL_CTRLB_CACHEDIS | NVMCTRL_CTRLB_MANW; // "CACHEDIS" Cache Disable | "MANW" Manual Write

	exec_nvm_cmd(NVMCTRL_CTRLA_CMD_EAR); // run "EAR" Erase Auxiliary Row
	exec_nvm_cmd(NVMCTRL_CTRLA_CMD_PBC); // run "PBC" Page Buffer Clear

	*((uint32_t *)NVMCTRL_AUX0_ADDRESS) = f0;
	*(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1) = f1;

	exec_nvm_cmd(NVMCTRL_CTRLA_CMD_WAP); // run "WAP" Write Auxiliary Page

	NVIC_SystemReset();
}

/**
 * \brief Check the application startup condition.
 *
 */
static void check_start_application(void)
{
	/*
	 * Read the first location of application section
	 * which contains the address of stack pointer.
	 * If it is 0xFFFFFFFF then the application section is empty.
	 */
	if (*((uint32_t *) APP_START_ADDRESS) == 0xFFFFFFFF)
	{
		#ifndef DEBUG
			// check boot protection fuses
			check_boot_protection();
		#endif
		return; // stay in bootloader
	}

  /*
   * Test vector table address of sketch @ &__sketch_vectors_ptr
   * Stay in SAM-BA if this function is not aligned enough, ie not valid
   */	
	if ( ((*((uint32_t *) APP_START_ADDRESS)) & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
	{
		return; // stay in bootloader
	}

#if defined(BOOT_DOUBLE_TAP_ADDRESS)
	#define DOUBLE_TAP_MAGIC 0x07738135
	if (PM->RCAUSE.bit.POR)
	{
		/* On power-on initialize double-tap */
		BOOT_DOUBLE_TAP_DATA = 0;
	}
	else
	{
		if (BOOT_DOUBLE_TAP_DATA == DOUBLE_TAP_MAGIC)
		{
			/* Second tap, stay in bootloader */
			BOOT_DOUBLE_TAP_DATA = 0;

			return;
		}

		/* First tap */
		BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;

		/* Wait 0.5sec to see if the user tap reset again.
		 * The loop value is based on SAMD21 default 1MHz clock @ reset.
		 */
		for (uint32_t i=0; i<125000; i++) /* 500ms */
			/* force compiler to not optimize this... */
			__asm__ __volatile__("");
	
		/* Timeout happened, continue boot... */
		BOOT_DOUBLE_TAP_DATA = 0;
	}
#endif

#ifdef LED_BOOT
	LED_BOOT_OFF();
#endif

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) APP_START_ADDRESS);
		
	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);
		
	/* Jump to user Reset Handler in the application */
	asm("bx %0"::"r"(*(unsigned *)(APP_START_ADDRESS + 4)));	
}


/*! \brief Main function.
 */
int main(void)
{
#ifndef DEBUG
	#if defined(BOARD_EITECH_ROBOTICS) || defined(BOARD_WATTUINO_RC)
		PORT->Group[0].PINCFG[PIN_PA30].bit.PMUXEN = 0; //PA30/SWCLK
		PORT->Group[0].PINCFG[PIN_PA31].bit.PMUXEN = 0; //PA31/SWDIO
	#endif
#endif
#ifdef LED_BOOT
	LED_BOOT_INIT(); // initialize LED
#endif
#ifdef LED_BOOT
	LED_BOOT_ON(); // LED on
#endif

	check_start_application();

	// start bootloader								
	irq_initialize_vectors();
	cpu_irq_enable();

	// init system
	system_init();

	// start USB stack to authorize VBus monitoring
	udc_start();
	
	/* Set 1 Flash Wait State for 48MHz, cf tables 20.9 and 35.27 in SAMD21 Datasheet */
	NVMCTRL->CTRLB.bit.RWS = NVMCTRL_CTRLB_RWS_HALF_Val;
	
	sam_ba_monitor_init();
	
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) 
	{
		if(main_b_cdc_enable) 
			sam_ba_monitor_run();
		
		if (main_b_msc_enable) 
			udi_msc_process_trans();
	}
}

void main_sof_action(void)
{
	if ((!main_b_msc_enable) || (!main_b_cdc_enable)) // MSC or CDC enabled?
		return;

#ifdef LED_BOOT
	/* uint16_t framenumber = udd_get_frame_number();
	if ((framenumber % 1000) == 0) 
		LED_BOOT_OFF();
	else if ((framenumber % 1000) == 500) 
		LED_BOOT_ON();*/

	if (udd_get_frame_number() & 512)
		LED_BOOT_OFF();
	else
		LED_BOOT_ON();
#endif
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
}
