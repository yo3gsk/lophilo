/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support  -  ROUSSET  -
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 * File Name           : at91sam9m10ekes.h
 * Object              :
 * Creation            : PVz June 2009
 *-----------------------------------------------------------------------------
 */
#ifndef _AT91SAM9M10EKES_H
#define _AT91SAM9M10EKES_H
 
/* ******************************************************************* */
/* PMC Settings                                                        */
/*                                                                     */
/* The main oscillator is enabled as soon as possible in the c_startup */
/* and MCK is switched on the main oscillator.                         */
/* PLL initialization is done later in the hw_init() function          */
/* ******************************************************************* */
#define MASTER_CLOCK		(132096000)
#define PLL_LOCK_TIMEOUT	1000000

#define BOARD_MAINOSC           12000000
#define BOARD_MCK               ((unsigned long)((BOARD_MAINOSC / 3 / 2 / 3) * 200 )) // 133MHz
#define BOARD_OSCOUNT           (AT91C_CKGR_OSCOUNT & (64 << 8))
#define BOARD_CKGR_PLLA         (AT91C_CKGR_SRCA | AT91C_CKGR_OUTA_0)
#define BOARD_PLLACOUNT         (0x3F << 8)
#define BOARD_MULA              (AT91C_CKGR_MULA & (199 << 16))
#define BOARD_DIVA              (AT91C_CKGR_DIVA & 3)
#define BOARD_PRESCALER         (0x00001300)

#define PLLA_SETTINGS           ( BOARD_CKGR_PLLA \
                                | BOARD_PLLACOUNT \
                                | BOARD_MULA \
                                | BOARD_DIVA)

//#define PLLA_SETTINGS	0x202A3F01
#define PLLUTMI
#define PLLUTMI_SETTINGS	0x10193F05

/* Switch MCK on PLLA output PCK = PLLA/2 = 3 * MCK */
#define MCKR_SETTINGS		(AT91C_PMC_CSS_MAIN_CLK | BOARD_PRESCALER)
#define MCKR_CSS_SETTINGS	(AT91C_PMC_CSS_PLLA_CLK | BOARD_PRESCALER)

/* ******************************************************************* */
/* DataFlash Settings                                                  */
/*                                                                     */
/* ******************************************************************* */
#define AT91C_BASE_SPI	AT91C_BASE_SPI0
#define AT91C_ID_SPI	AT91C_ID_SPI0

/* SPI CLOCK */
#define AT91C_SPI_CLK 		 33000000

/* AC characteristics */
/* DLYBS = tCSS= 250ns min and DLYBCT = tCSH = 250ns */
#define DATAFLASH_TCSS		(0x22 << 16)
#define DATAFLASH_TCHS		(0x1 << 24)

#define DF_CS_SETTINGS 		(AT91C_SPI_NCPHA | (AT91C_SPI_DLYBS & DATAFLASH_TCSS) | (AT91C_SPI_DLYBCT & DATAFLASH_TCHS) | ((MASTER_CLOCK / AT91C_SPI_CLK) << 8))

/* ******************************************************************* */
/* BootStrap Settings                                                  */
/*                                                                     */
/* ******************************************************************* */
#define AT91C_SPI_PCS_DATAFLASH		AT91C_SPI_PCS0_DATAFLASH	/* Boot on SPI NCS0 */

#define IMG_ADDRESS 		0x8400			/* Image Address in DataFlash */
#define	IMG_SIZE		0x33900			/* Image Size in DataFlash */

#define MACH_TYPE		0x9CD			/* AT91SAM9M10-EKES */
#define JUMP_ADDR		0x73F00000		/* Final Jump Address */

/* ******************************************************************* */
/* Application Settings                                                */
/* ******************************************************************* */
#undef DEBUG
#undef CFG_DEBUG
#undef CFG_NANDFLASH

#define CFG_DATAFLASH
#define	CFG_DDRAM
#define	CFG_HW_INIT

#endif	/* _AT91SAM9M10EKES_H */
