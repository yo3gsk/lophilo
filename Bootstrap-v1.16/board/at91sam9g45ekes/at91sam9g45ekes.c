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
 * File Name           : at91sam9g45ekes.c
 * Object              :
 * Creation            : NFe Jan 2008
 *-----------------------------------------------------------------------------
 */
#include "../../include/part.h"
#include "../../include/gpio.h"
#include "../../include/pmc.h"
#include "../../include/debug.h"
#include "../../include/main.h"
#include "../../include/ddramc.h"
#ifdef CFG_NANDFLASH
#include "../../include/nandflash.h"
#endif
#ifdef CFG_DATAFLASH
#include "../../include/dataflash.h"
#endif

static inline unsigned int get_cp15(void)
{
	unsigned int value;
	__asm__("mrc p15, 0, %0, c1, c0, 0" : "=r" (value));
	return value;
}

static inline void set_cp15(unsigned int value)
{
	__asm__("mcr p15, 0, %0, c1, c0, 0" : : "r" (value));
}

#ifdef CFG_HW_INIT
/*----------------------------------------------------------------------------*/
/* \fn    hw_init							      */
/* \brief This function performs very low level HW initialization	      */
/* This function is invoked as soon as possible during the c_startup	      */
/* The bss segment must be initialized					      */
/*----------------------------------------------------------------------------*/
void hw_init(void)
{
	unsigned int cp15;

	/* Configure PIOs */
	const struct pio_desc hw_pio[] = {
#ifdef CFG_DEBUG
		{"RXD", AT91C_PIN_PB(12), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"TXD", AT91C_PIN_PB(13), 0, PIO_DEFAULT, PIO_PERIPH_A},
#endif
		{(char *) 0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Disable watchdog */
	writel(AT91C_WDTC_WDDIS, AT91C_BASE_WDTC + WDTC_WDMR);

	/* At this stage the main oscillator is supposed to be enabled
	 * PCK = MCK = MOSC */
	writel(0x00, AT91C_BASE_PMC + PMC_PLLICPR);

	/* Configure PLLA = MOSC * (PLL_MULA + 1) / PLL_DIVA */
	pmc_cfg_plla(PLLA_SETTINGS, PLL_LOCK_TIMEOUT);

	/* PCK = PLLA/2 = 3 * MCK */
	pmc_cfg_mck(MCKR_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Switch MCK on PLLA output */
	pmc_cfg_mck(MCKR_CSS_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Configure PLLB */
	//pmc_cfg_pllutmi(PLLUTMI_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Configure CP15 */
	cp15 = get_cp15();
	cp15 |= I_CACHE;
	set_cp15(cp15);

	/* Configure the PIO controller */
	pio_setup(hw_pio);

#ifdef CFG_DEBUG
	/* Enable Debug messages on the DBGU */
	dbg_init(BAUDRATE(MASTER_CLOCK, 115200));

	dbg_print("Start AT91Bootstrap...\n\r");
#endif /* CFG_DEBUG */

#ifdef CFG_DDRAM
	/* Configure DDRAM Controller */
	ddramc_hw_init();
#endif /* CFG_DDRAM */
}
#endif /* CFG_HW_INIT */

#ifdef CFG_DDRAM
static	SDdramConfig ddram_config = {	
	.ddramc_mdr  = (AT91C_DDRC2_DBW_16_BITS | AT91C_DDRC2_MD_DDR2_SDRAM),
		
	.ddramc_cr   = (AT91C_DDRC2_NC_DDR10_SDR9  |     	// 10 column bits (1K)
			AT91C_DDRC2_NR_14          |     	// 14 row bits    (8K)
			AT91C_DDRC2_CAS_3          |     	// CAS Latency 3
			AT91C_DDRC2_DLL_RESET_DISABLED),	// DLL not reset

	.ddramc_rtr  = 	0x24B,

	.ddramc_t0pr = (AT91C_DDRC2_TRAS_6  |		//  6 * 7.5 = 45   ns
			AT91C_DDRC2_TRCD_2  |		//  2 * 7.5 = 22.5 ns
			AT91C_DDRC2_TWR_2   |		//  2 * 7.5 = 15   ns
			AT91C_DDRC2_TRC_8  |		//  8 * 7.5 = 75   ns
			AT91C_DDRC2_TRP_2   |		//  2 * 7.5 = 22.5 ns
			AT91C_DDRC2_TRRD_1  |		//  1 * 7.5 = 7.5   ns
			AT91C_DDRC2_TWTR_1  |		//  1 clock cycle
			AT91C_DDRC2_TMRD_2),		//  2 clock cycles
		
	.ddramc_t1pr = (AT91C_DDRC2_TXP_2  |		//  2 * 7.5 = 15 ns
			200 << 16           |		// 200 clock cycles, TXSRD: Exit self refresh delay to Read command
			16 << 8             |		// 16 * 7.5 = 120 ns TXSNR: Exit self refresh delay to non read command
			AT91C_DDRC2_TRFC_14 << 0),	// 14 * 7.5 = 142 ns (must be 140 ns for 1Gb DDR)

	.ddramc_t2pr = (AT91C_DDRC2_TRTP_1   |		//  1 * 7.5 = 7.5 ns
			AT91C_DDRC2_TRPA_0   |		//  0 * 7.5 = 0 ns
			AT91C_DDRC2_TXARDS_7 |		//  7 clock cycles
			AT91C_DDRC2_TXARD_2),		//  2 clock cycles
};

/*------------------------------------------------------------------------------*/
/* \fn    ddramc_hw_init							*/
/* \brief This function performs DDRAMC HW initialization			*/
/*------------------------------------------------------------------------------*/
void ddramc_hw_init(void)
{
	// ENABLE DDR2 clock 
	writel(AT91C_PMC_DDR, AT91C_BASE_PMC + PMC_SCER);

	/* DDRAM2 controller */
	ddram_init(AT91C_BASE_DDR2C, AT91C_DDR2, &ddram_config);

	/* Setup Smart Media, first enable the address range of CS3 in HMATRIX user interface */
	writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) | AT91C_EBI_CS1A_SDRAMC, AT91C_BASE_CCFG + CCFG_EBICSA);
	
	/* EBI IO in 1.8V mode */
	writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) & ~(1<<16), AT91C_BASE_CCFG + CCFG_EBICSA);
	
	/* EBI DDRAM controller */
	ddram_init(AT91C_BASE_DDR2CP1, AT91C_EBI_CS1, &ddram_config);

}
#endif /* CFG_DDRAM */

#ifdef CFG_DATAFLASH

/*------------------------------------------------------------------------------*/
/* \fn    df_recovery								*/
/* \brief This function erases DataFlash Page 0 if BP4 is pressed 		*/
/*        during boot sequence							*/
/*------------------------------------------------------------------------------*/
void df_recovery(AT91PS_DF pDf)
{

}

/*------------------------------------------------------------------------------*/
/* \fn    df_hw_init								*/
/* \brief This function performs DataFlash HW initialization			*/
/*------------------------------------------------------------------------------*/
void df_hw_init(void)
{
	/* Configure PIOs */
	const struct pio_desc df_pio[] = {
		{"MISO",  AT91C_PIN_PB(0), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"MOSI",  AT91C_PIN_PB(1), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"SPCK",  AT91C_PIN_PB(2), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"NPCS0", AT91C_PIN_PB(3), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"NPCS1", AT91C_PIN_PC(11), 0, PIO_DEFAULT, PIO_PERIPH_B},
		{(char *) 0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Configure the PIO controller */
	pio_setup(df_pio);
}
#endif /* CFG_DATAFLASH */



#ifdef CFG_NANDFLASH
/*------------------------------------------------------------------------------*/
/* \fn    nand_recovery						*/
/* \brief This function erases NandFlash Block 0 if BP4 is pressed 		*/
/*        during boot sequence							*/
/*------------------------------------------------------------------------------*/
static void nand_recovery(void)
{

}

/*------------------------------------------------------------------------------*/
/* \fn    nandflash_hw_init							*/
/* \brief NandFlash HW init							*/
/*------------------------------------------------------------------------------*/
void nandflash_hw_init(void)
{
	/* Configure PIOs */
	const struct pio_desc nand_pio[] = {
		{"RDY_BSY", AT91C_PIN_PC(8), 0, PIO_PULLUP, PIO_INPUT},
		{"NANDCS",  AT91C_PIN_PC(14), 0, PIO_PULLUP, PIO_OUTPUT},
		{(char *) 0, 0, 0,  PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Setup Smart Media, first enable the address range of CS3 in HMATRIX user interface */
	writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) | AT91C_EBI_CS3A_SM, AT91C_BASE_CCFG + CCFG_EBICSA);
	
	/* EBI IO in 1.8V mode */
	writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) & ~(1<<16), AT91C_BASE_CCFG + CCFG_EBICSA);
	 
	/* Configure SMC CS3 */
 	writel((AT91C_SM_NWE_SETUP | AT91C_SM_NCS_WR_SETUP | AT91C_SM_NRD_SETUP | AT91C_SM_NCS_RD_SETUP), AT91C_BASE_SMC + SMC_SETUP3);
  	writel((AT91C_SM_NWE_PULSE | AT91C_SM_NCS_WR_PULSE | AT91C_SM_NRD_PULSE | AT91C_SM_NCS_RD_PULSE), AT91C_BASE_SMC + SMC_PULSE3);
	writel((AT91C_SM_NWE_CYCLE | AT91C_SM_NRD_CYCLE)						, AT91C_BASE_SMC + SMC_CYCLE3);
	writel((AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE | 
  		AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS | AT91C_SM_TDF)						, AT91C_BASE_SMC + SMC_CTRL3);

	/* Configure the PIO controller */
	writel((1 << AT91C_ID_PIOC), (PMC_PCER + AT91C_BASE_PMC));
	pio_setup(nand_pio);
	
	nand_recovery();
}

/*------------------------------------------------------------------------------*/
/* \fn    nandflash_cfg_16bits_dbw_init						*/
/* \brief Configure SMC in 16 bits mode						*/
/*------------------------------------------------------------------------------*/
void nandflash_cfg_16bits_dbw_init(void)
{
	writel(readl(AT91C_BASE_SMC + SMC_CTRL3) | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS, AT91C_BASE_SMC + SMC_CTRL3);
}

/*------------------------------------------------------------------------------*/
/* \fn    nandflash_cfg_8bits_dbw_init						*/
/* \brief Configure SMC in 8 bits mode						*/
/*------------------------------------------------------------------------------*/
void nandflash_cfg_8bits_dbw_init(void)
{
	writel((readl(AT91C_BASE_SMC + SMC_CTRL3) & ~(AT91C_SMC_DBW)) | AT91C_SMC_DBW_WIDTH_EIGTH_BITS, AT91C_BASE_SMC + SMC_CTRL3);
}

#endif /* #ifdef CFG_NANDFLASH */
