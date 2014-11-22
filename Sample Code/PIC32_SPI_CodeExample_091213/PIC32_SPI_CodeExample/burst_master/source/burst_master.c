/*********************************************************************
 *
 *   PIC32MX SPI Burst Master Example
 *
 *********************************************************************
 * FileName:        burst_master.c
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
 * This test assumes that a slave device is connected to our test
 * SPI device and that the slave device, after receiving the data
 * from the master, switches to master mode and relays data back
 * to us. We then verify the integrity of the packet. This is the
 * SPI master portion of the code.
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
int SpiDoBurstExampleMaster(SpiChannel chn, int nCycles);
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster);

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
 * Output: 0 if the SPI transfer failed,
 * 	   1 if the SPI transfer succeeded
 *
 * Side Effects: None
 *
 * Overview: Example for the usage of the SPI Peripheral Lib
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

    srand(ReadCoreTimer()); // Seed the pseudo random generator

    if(!SpiDoBurstExampleMaster(spiChn, 100))
    {
        return 0;   // Our example failed
    }

    return 1; // Our example was a success
}

/*********************************************************************
 * Function: int SpiDoBurstExampleMaster(SpiChannel chn, int nCycles)
 *
 * PreCondition: None
 *
 * Input: chn - the SPI channel to use
 * 	  nCycles - number of repeated transfers
 *
 * Output: 1 (true) if the SPI burst transfer succeeded,
 *         0 (false) otherwise
 *
 * Side Effects: None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib in a master/slave switch transfer mode
 *
 * Note: This test assumes that a slave device is connected to our test SPI device and that
 * 	 the slave device, after receiving the data from the master, switches to master mode and
 * 	 relays the data back to us. We then verify the integrity of the packet. This is the
 *       SPI master code.
 ********************************************************************/
int SpiDoBurstExampleMaster(SpiChannel chn, int nCycles)
{
    int fail = 0;

    SpiInitDevice(chn, 1, 1, 1); // Initialize the SPI channel as master, frame master

    while(nCycles-- && !fail) // Keep going until there is no data left to transfer / no transfer fails
    {
	int txferSize;
	SpiPkt*	pTxPkt;
	SpiPkt*	pRxPkt;

	txferSize=MIN_SPI_TXFER_SIZE+rand()%(MAX_SPI_TXFER_SIZE-MIN_SPI_TXFER_SIZE+1);	// Get a random transfer size

	pTxPkt=(SpiPkt*)malloc(txferSize*sizeof(short)+2);
	pRxPkt=(SpiPkt*)malloc(txferSize*sizeof(short)+2);  // We'll transfer 16 bits words; extra room for nChars

	if(pTxPkt && pRxPkt)
	{
            int	ix;

            for(ix=0; ix<txferSize; ix++)
            {
		pTxPkt->data[ix]=(unsigned short)rand(); // Fill buffer with some random data
            }

            pTxPkt->nChars=txferSize;

            SpiChnChangeMode(chn, 1, 1, 1); // Set master mode
            SpiChnPutS(chn, (unsigned int*)pTxPkt, txferSize+2);    // Write packet

            SpiChnChangeMode(chn, 0, 0, 1); // Set slave mode
            SpiChnGetS(chn, (unsigned int*)pRxPkt, txferSize);

            // Now verify the received buffer contents
            for(ix=0; ix<txferSize; ix++)
            {
		if(pTxPkt->data[ix]!=pRxPkt->data[ix])
		{
                    fail = 1; // Data mismatch
                    break;
		}
            }
	}

	else
	{	
            fail=1; // Memory allocation failed
	}

	free(pRxPkt);
	free(pTxPkt); // Free the allocated buffers
    }

    return !fail;
}

/*********************************************************************
 * Function: void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
 *
 * PreCondition: None
 *
 * Input: chn - the SPI channel to use, 1 or 2
 *        isMaster -  1: the device is to act as a bus master
 * 		      0: the device is an SPI slave
 * 	  frmEn -     1: frame mode is enabled
 * 		      0: frame mode is disabled
 * 	  frmMaster - 1: if frame mode is enabled, the device is a frame master (FRMSYNC is output)
 *                    0: if frame mode is enabled, the device is a frame slave (FRMSYNC is input)
 *
 * Output: None
 *
 * Side Effects: None
 *
 * Overview: Inits the SPI channel 1 to use 16 bit words
 *           Performs the device initialization in both master/slave modes.
 *
 * Note: None
 ********************************************************************/
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster)
{
    // 16 bits/char, input data sampled at end of data output time
    SpiOpenFlags oFlags=SPI_OPEN_MODE16|SPI_OPEN_SMP_END|SPI_OPEN_ON;

    if(isMaster)
    {
	oFlags|=SPI_OPEN_MSTEN; // Set as Master mode in this example
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
