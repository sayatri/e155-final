/*********************************************************************
 *
 *   PIC32MX SPI Burst Slave Example
 *
 *********************************************************************
 * FileName:        burst_slave.c
 * Dependencies:    plib.h
 *
 * Processor:       PIC32
 *
 * Complier:        MPLAB Cxx
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PIC Microcontroller is intended
 * and supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *********************************************************************
 * $Id: spi_api_example.c 4172 2007-08-14 22:17:03Z aura $
 *********************************************************************
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Description:
 * This test assumes that a master device is connected to our test
 * SPI device and that it sends us data packets. The slave device,
 * after receiving the data from the master, switches to master mode
 * and relays data back to the master. The master can then verify
 * the integrity of the packet. This is the SPI slave portion of 
 * the code. The slave has to be launched first and has to be waiting
 * when the master begins transmitting data.
 ********************************************************************/
#include <plib.h>

#if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 80 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
#define SYS_FREQ (80000000L)

#elif defined (__32MX220F032D__) || (__32MX250F128D__)
// Configuration Bit settings
// SYSCLK = 48 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 48 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_24, FPLLIDIV = DIV_2, FPLLODIV = DIV_2, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
#define SYS_FREQ (48000000L)
#endif

// Function prototypes
void	SpiDoBurstExampleSlave(SpiChannel chn);
void	SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster);

// Some definitions
#define	MIN_SPI_TXFER_SIZE 8    // Min number of words per transfer
#define	MAX_SPI_TXFER_SIZE 512  // Max number of words per transfer

typedef struct
{
    unsigned short  nChars;  // Number of characters in the packet
    unsigned short  data[1]; // Data itself
}SpiPkt; // A transferred SPI packet...assumes we're working in 16 bit mode

/*********************************************************************
 * Function: int main(void)
 *
 * PreCondition: None
 *
 * Input: None
 *
 * Output: None
 *
 * Side Effects: None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib
 *
 * Note: None.
 ********************************************************************/
int main(void)
{
    SpiChannel spiChn=SPI_CHANNEL1; // The SPI channel to use

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above.
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    SpiDoBurstExampleSlave(spiChn);

    return 1;
}


/*********************************************************************
 * Function: void SpiDoBurstExampleSlave(SpiChannel chn)
 *
 * PreCondition: None
 *
 * Input: chn - the SPI channel to use
 *
 * Output: None
 *
 * Side Effects: None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib in a master/slave switch transfer mode
 *
 * Note: This test assumes that a master device is connected to our test SPI device and that
 * 	 it sends us data packets. The slave device, after receiving the data from the master switches
 *       to master mode and relays data back to the master. The master can then verify the integrity of
 *       the packet. This is the SPI slave code. The slave has to be launched first and has to be waiting
 *       when the master begins transmitting data.
 ********************************************************************/
void SpiDoBurstExampleSlave(SpiChannel chn)
{
    static unsigned short slaveBuff[MAX_SPI_TXFER_SIZE]; // Buffer for the slave to receive the data

    SpiInitDevice(chn, 0, 1, 0); // Initialize the SPI channel as slave, frame slave

    while(1)
    {
	int txferSize;

	SpiChnChangeMode(chn, 0, 0, 1);	// Set slave mode
	txferSize=SpiChnGetC(chn); // Receive the packet header
	SpiChnGetS(chn, (unsigned int*)slaveBuff, txferSize); // Get the rest of the packet

	// Switch to master mode
	SpiChnChangeMode(chn, 1, 1, 1);
	SpiChnPutC(chn, txferSize); // Transmit the packet header
	SpiChnPutS(chn, (unsigned int*)slaveBuff, txferSize); // Write the rest of the packet
    }
}

/*********************************************************************
 * Function: void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
 *
 * PreCondition: None
 *
 * Input: chn - the SPI channel to use
 * 	  isMaster - 1: the device is to act as a bus master
 * 		     0: the device is an SPI slave
 * 	  frmEn	- 1: frame mode is enabled
 * 		  0: frame mode is disabled
 * 	  frmMaster - 0: if frame mode is enabled, the device is a frame slave (FRMSYNC is input)
 * 		      1: if frame mode is enabled, the device is a frame master (FRMSYNC is output)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview: Inits the SPI channel 1 to use 16 bit words. Performs the device initialization
 *           in both master/slave modes.
 *
 * Note: None
 ********************************************************************/
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
{
    SpiOpenFlags oFlags=SPI_OPEN_MODE16|SPI_OPEN_SMP_END|SPI_OPEN_ON; // SPI open mode

    if(isMaster)
    {
	oFlags|=SPI_OPEN_MSTEN;
    }

    if(frmEn)
    {
        oFlags|=SPI_OPEN_FRMEN;

        if(!frmMaster)
	{
            oFlags|=SPI_OPEN_FSP_IN;
	}
    }
    
    // Open SPI module, use SPI channel 1, use flags set above, Divide Fpb by 4
    SpiChnOpen(chn, oFlags, 4);	
}


