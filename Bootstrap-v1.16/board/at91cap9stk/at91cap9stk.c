/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support  -  ROUSSET  -
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Olivier Arnal <olivier.arnal@atmel.com>
 * Copyright (c) 2007, Stelian Pop <stelian.pop@leadtechdesign.com>
 * Copyright (c) 2007 Lead Tech Design <www.leadtechdesign.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
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
 * File Name           : at91cap9adk.c
 * Object              :
 * Creation            :
 *-----------------------------------------------------------------------------
 */
#include "../../include/part.h"
#include "../../include/gpio.h"
#include "../../include/pmc.h"
#include "../../include/debug.h"
#include "../../include/sddrc.h"
#include "../../include/main.h"
#ifdef CFG_NANDFLASH
#include "../../include/nandflash.h"
#endif
#ifdef CFG_DATAFLASH
#include "../../include/dataflash.h"
#endif
#ifdef CFG_NORFLASH
#include "../../include/norflash.h"
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


unsigned char at73c224_configure(void);

#ifdef CFG_HW_INIT
/*---------------------------------------------------------------------------- */
/* \fn    hw_init							       */
/* \brief This function performs very low level HW initialization	       */
/* This function is invoked as soon as possible during the c_startup	       */
/* The bss segment must be initialized					       */
/*---------------------------------------------------------------------------- */
void hw_init(void)
{
	/* Configure PIOs */
	const struct pio_desc hw_pio[] = {
#ifdef CFG_DEBUG
		{"RXD", AT91C_PIN_PC(30), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"TXD", AT91C_PIN_PC(31), 0, PIO_DEFAULT, PIO_PERIPH_A},
#endif
		{(char *) 0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Disable watchdog */
	writel(AT91C_WDTC_WDDIS, AT91C_BASE_WDTC + WDTC_WDMR);

	/* Ensure RTT interrupts are disabled, or
	 * Linux won't know how to handle them */
	writel(readl(AT91C_BASE_RTTC + RTTC_RTMR) & 0xFFFCFFFF, AT91C_BASE_RTTC + RTTC_RTMR);

	/* At this stage the main oscillator is supposed to be enabled
	 * PCK = MCK = MOSC */

	/* Configure PLLA = MOSC * (PLL_MULA + 1) / PLL_DIVA */
	pmc_cfg_plla(PLLA_SETTINGS, PLL_LOCK_TIMEOUT);

	/* PCK = PLLA = 2 * MCK */
	pmc_cfg_mck(MCKR_SETTINGS, PLL_LOCK_TIMEOUT);
	/* Switch MCK on PLLA output */
	pmc_cfg_mck(MCKR_CSS_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Configure PLLB */
	pmc_cfg_pllb(PLLB_SETTINGS, PLL_LOCK_TIMEOUT);

	/* Configure the PIO controller to output PCK0 */
	pio_setup(hw_pio);

#ifdef CFG_DEBUG
	/* Enable Debug messages on the DBGU */
	dbg_init(BAUDRATE(MASTER_CLOCK, 115200));
	dbg_print("Start AT91Bootstrap...\n\r");
#endif /* CFG_DEBUG */

#ifdef CFG_SDDRC
	/* Initialize the matrix */
	/* VDDIOMSEL = 1 -> Memories are 3.3V powered */
	writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) | AT91C_EBI_SUP_3V3, AT91C_BASE_CCFG + CCFG_EBICSA);

	/* Configure SDRAM Controller */
	sddrc_init(0x00000039, 781);
#endif	/* CFG_SDDRC */

	/* confiogure power management */
	at73c224_configure();

}
#endif /* CFG_HW_INIT */


#ifdef CFG_SDDRC
//*----------------------------------------------------------------------------
//* \fn    sdddrc_hw_init
//* \brief This function performs SDDRC HW initialization
//*----------------------------------------------------------------------------*/
void sddrc_hw_init(void)
{
	/* Configure PIOs */
	const struct pio_desc sddrc_pio[] = {
		{"D16", AT91C_PIN_PD(16), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D17", AT91C_PIN_PD(17), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D18", AT91C_PIN_PD(18), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D19", AT91C_PIN_PD(19), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D20", AT91C_PIN_PD(20), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D21", AT91C_PIN_PD(21), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D22", AT91C_PIN_PD(22), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D23", AT91C_PIN_PD(23), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D24", AT91C_PIN_PD(24), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D25", AT91C_PIN_PD(25), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D26", AT91C_PIN_PD(26), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D27", AT91C_PIN_PD(27), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D28", AT91C_PIN_PD(28), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D29", AT91C_PIN_PD(29), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D30", AT91C_PIN_PD(30), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{"D31", AT91C_PIN_PD(31), 0, PIO_DEFAULT, PIO_PERIPH_A},
		{(char *) 0, 0, 0, PIO_DEFAULT, PIO_PERIPH_A},
	};

	/* Configure the SDDRC PIO controller */
	pio_setup(sddrc_pio);
}
#endif

#ifdef CFG_NANDFLASH
/*------------------------------------------------------------------------------*/
/* \fn    nandflash_hw_init                         */
/* \brief NandFlash HW init                         */
/*------------------------------------------------------------------------------*/
void nandflash_hw_init(void)
{
    /* Configure PIOs */
    const struct pio_desc nand_pio[] = {
        {"NANDCS",  AT91C_PIN_PD(15), 0, PIO_DEFAULT, PIO_OUTPUT},
        {(char *) 0, 0, 0,  PIO_DEFAULT, PIO_PERIPH_A},
    };


    /* Setup Smart Media, first enable the address range of CS3 in HMATRIX user interface */
    writel(readl(AT91C_BASE_CCFG + CCFG_EBICSA) | AT91C_EBI_CS3A_SM, AT91C_BASE_CCFG + CCFG_EBICSA);

    /* Configure SMC CS3 */
    writel((AT91C_SM_NWE_SETUP | AT91C_SM_NCS_WR_SETUP | AT91C_SM_NRD_SETUP | AT91C_SM_NCS_RD_SETUP), AT91C_BASE_SMC + SMC_SETUP3);
    writel((AT91C_SM_NWE_PULSE | AT91C_SM_NCS_WR_PULSE | AT91C_SM_NRD_PULSE | AT91C_SM_NCS_RD_PULSE), AT91C_BASE_SMC + SMC_PULSE3);
    writel((AT91C_SM_NWE_CYCLE | AT91C_SM_NRD_CYCLE)                        , AT91C_BASE_SMC + SMC_CYCLE3);
    writel((AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE |
        AT91C_SMC_DBW_WIDTH_EIGTH_BITS | AT91C_SM_TDF)                        , AT91C_BASE_SMC + SMC_CTRL3);

    /* Configure the PIO controller */
    writel((1 << AT91C_ID_PIOABCDE), PMC_PCER + AT91C_BASE_PMC);
    pio_setup(nand_pio);
}

void nandflash_cfg_16bits_dbw_init(void)
{
    writel(readl(AT91C_BASE_SMC + SMC_CTRL3) | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS, AT91C_BASE_SMC + SMC_CTRL3);
}

/*------------------------------------------------------------------------------*/
/* \fn    nandflash_cfg_8bits_dbw_init                      */
/* \brief Configure SMC in 8 bits mode                      */
/*------------------------------------------------------------------------------*/
void nandflash_cfg_8bits_dbw_init(void)
{
    writel((readl(AT91C_BASE_SMC + SMC_CTRL3) & ~(AT91C_SMC_DBW)) | AT91C_SMC_DBW_WIDTH_EIGTH_BITS, AT91C_BASE_SMC + SMC_CTRL3);
}

#endif /* #ifdef CFG_NANDFLASH */


#ifdef CFG_AT73C224

/// Slave address of AT73C224 chips.
#define AT73C224_SLAVEADDRESS_U4   0x48    // 1001000
#define AT73C224_SLAVEADDRESS_U5   0x49    // 1001001

/*------------------------------------------------------------------------------*/
/* \fn    at73c224_initialize                                                   */
/* \brief Initialize AT73C224                                                   */
/*------------------------------------------------------------------------------*/
void at73c224_initialize(void)
{
    /* configure Pio pins */
    const struct pio_desc twi_pio[] = {
        {"TWD",  AT91C_PIN_PB(4), 0, PIO_DEFAULT, PIO_PERIPH_B},
        {"TWCK", AT91C_PIN_PB(5), 0, PIO_DEFAULT, PIO_PERIPH_B},
        {(char *) 0, 0, 0,  PIO_DEFAULT, PIO_PERIPH_A},
    };

    pio_setup(twi_pio);

    writel(1 << AT91C_ID_TWI, AT91C_BASE_PMC + PMC_PCER);

    /* Reset the TWI */
    writel(AT91C_TWI_SWRST, AT91C_BASE_TWI + TWI_CR);
    /* Set master mode */
    writel(AT91C_TWI_MSEN, AT91C_BASE_TWI + TWI_CR);
    /* Set Clock Waveform Generator Register */
    /* MCK  = 100000000 */
    /* TWCK = 400000 */
    writel(0x00007A7A, AT91C_BASE_TWI + TWI_CWGR);    
}

/*------------------------------------------------------------------------------*/
/* \fn    at73c224_read                                                         */
/* \brief Read data from AT73C224                                               */
/*------------------------------------------------------------------------------*/
void at73c224_read(
    unsigned char *pByte,
    unsigned char registerAddress,
    unsigned char address
    )
{
    // Set STOP signal if only one byte is sent
    //--------------------------------------------------------------------------  
    writel(AT91C_TWI_STOP, AT91C_BASE_TWI + TWI_CR);

    // Start read
    //--------------------------------------------------------------------------
    // Set slave address and number of internal address bytes
    writel((1 << 8) | AT91C_TWI_MREAD | (address << 16), AT91C_BASE_TWI + TWI_MMR);
    // Set internal address bytes
    writel(registerAddress, AT91C_BASE_TWI + TWI_IADR);
    // Send START condition
    writel(AT91C_TWI_START, AT91C_BASE_TWI + TWI_CR);
    
    // Read all bytes, setting STOP before the last byte
    //--------------------------------------------------------------------------    
    writel(AT91C_TWI_STOP, AT91C_BASE_TWI + TWI_CR);

    // Wait for byte then read and store it
    //--------------------------------------------------------------------------    
    while (! (( readl(AT91C_BASE_TWI + TWI_SR) & AT91C_TWI_RXRDY) == AT91C_TWI_RXRDY)) ;
    *pByte = readl(AT91C_BASE_TWI + TWI_RHR);
    
    // Wait for transfer to be complete
    //--------------------------------------------------------------------------    
    while (! (( readl(AT91C_BASE_TWI + TWI_SR) & AT91C_TWI_TXCOMP_MASTER) == AT91C_TWI_TXCOMP_MASTER));
}

/*------------------------------------------------------------------------------*/
/* \fn    at73c224_write                                                        */
/* \brief Write data to  AT73C224                                               */
/*------------------------------------------------------------------------------*/
void at73c224_write(
    unsigned char byte,
    unsigned char registerAddress,
    unsigned char address
    )
{
    // Set slave address and number of internal address bytes
    writel((1 << 8) | (address << 16), AT91C_BASE_TWI + TWI_MMR);
    // Set internal address bytes
    writel(registerAddress, AT91C_BASE_TWI + TWI_IADR);
    // Write the byte
    writel(byte, AT91C_BASE_TWI + TWI_THR);
    
    // Wait for transfer to be complete
    while (! (( readl(AT91C_BASE_TWI + TWI_SR) & AT91C_TWI_TXCOMP_MASTER) == AT91C_TWI_TXCOMP_MASTER));
}

/*------------------------------------------------------------------------------*/
/* \fn    at73c224_configure                                                    */
/* \brief configure Power management                                            */
/*------------------------------------------------------------------------------*/
unsigned char at73c224_configure(void)
{
    unsigned char buff;
    
    at73c224_initialize();

    // Check that we are communicating with the good device
    at73c224_read(&buff, 0x01, AT73C224_SLAVEADDRESS_U4);
    //dbg_print("device adresse = 0x%X\n\r", buff);
    if(buff != 0x48) {

        dbg_print("-E- Wrong device adressed\n\r");
        return 0;
    }
    // Check that the BUCK default voltage is 0x09 (1.8V)
    at73c224_read(&buff, 0x15, AT73C224_SLAVEADDRESS_U4);   // BCK_VOLT : 0x15 ;
    //dbg_print("BCK_VOLT = 0x%X\n\r", buff);
    if(buff != 0x09) {

        dbg_print("-E- Wrong default output voltage\r\n");
        return 0;
    }

    // Check that the BUCK converter is started
    at73c224_read(&buff, 0x13, AT73C224_SLAVEADDRESS_U4);  // BCK_CTRL : 0x13 ;
    //dbg_print("BCK_CTRL = 0x%X\n\r", buff);
    if(buff != 0x01) {

        dbg_print("-E- BUCK converter is started\n\r");
        return 0;
    }

    // Check that all interrupts are masked
    at73c224_read(&buff, 0x32, AT73C224_SLAVEADDRESS_U4);  // IRQ_MSK : 0x32 ;
    //dbg_print("IRQ_MSK = 0x%X\n\r", buff);
    if(buff != 0x00) {

        dbg_print("-E- not all interrupts are masked\n\r");
        return 0;
    }

    // Check that the boost output voltage is configured in 5V mode
    at73c224_read(&buff, 0x12, AT73C224_SLAVEADDRESS_U4);   // BST_VOLT : 0x12 ;
    //dbg_print("BST_VOLT = 0x%X\n\r", buff);
    // resut: BST_VOLT = 0x28

    // Check that the current limitation is not too low
    at73c224_read(&buff, 0x11, AT73C224_SLAVEADDRESS_U4);   // BST_CFG : 0x11 ;
    //dbg_print("BST_CFG = 0x%X\n\r", buff);
    // result: BST_CFG = 0x1B
    if(buff < 0x01) {

        dbg_print("-E- Too low current limitation\n\r");
        return 0;
    }

    // Setting the ramp for optimal operation for the application CAP
    at73c224_write(0x40, 0x72, AT73C224_SLAVEADDRESS_U4);   // RTRIM : 0x72 ;

    // 0x0B @ 0x11 : on 2 chips
    at73c224_write(0x0B, 0x11, AT73C224_SLAVEADDRESS_U4);
    at73c224_write(0x0B, 0x11, AT73C224_SLAVEADDRESS_U5);

    // Passage of Buck in PWM mode
    // Write 0xD8 @ 0x14           
    at73c224_write(0xD8, 0x14, AT73C224_SLAVEADDRESS_U4);

    // Setting the ramp for optimal operation for the application CAP
    at73c224_write(0x40, 0x72, AT73C224_SLAVEADDRESS_U5);   // RTRIM : 0x72 ;

    // Passage of Buck in PWM mode
    // Write 0xD8 @ 0x14                 
    at73c224_write(0xD8, 0x14, AT73C224_SLAVEADDRESS_U5);

    // Init for USB Host
#ifdef CFG_AT73C224_POWER_USB_HOST
    // At the start-up, it is recommended to put 1Amp over current threshold 
    // in order not to generate a reset of the product.
    at73c224_write(0x01, 0x11, AT73C224_SLAVEADDRESS_U4);

    // setting VBOOST = 5V
    at73c224_write(0x28, 0x12, AT73C224_SLAVEADDRESS_U4);

    // Start VBOOST
    at73c224_write(0x01, 0x10, AT73C224_SLAVEADDRESS_U4);

    // put 6Amp over current threshold.
    at73c224_write(0x0B, 0x11, AT73C224_SLAVEADDRESS_U4);
#endif /* #ifdef CFG_AT73C224_POWER_USB_HOST*/

    return 1;
}

#endif /* #ifdef CFG_AT73C224 */ 

