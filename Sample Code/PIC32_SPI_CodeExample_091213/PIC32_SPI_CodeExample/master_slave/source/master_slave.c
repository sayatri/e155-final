/*********************************************************************
 *
 *   PIC32MX SPI Master/Slave Example
 *
 *********************************************************************
 * FileName:        master_slave.c
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
 *
 *    This test uses both SPI channels. The master channel
 *    (SPI1) sends data to a slave device (SPI2). The slave
 *    device relays data back to the master. This way we can
 *    verify that the connection to the slave is ok.
 *    Hardware connections have to be made:
 *	- SCK1 <-> SCK2
 *	- SDO1 <-> SDI2
 *	- SDI1 <-> SDO2
 *	- SS1  <-> SS2 (needed only if we use the framed mode)
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
int SpiDoMasterSlaveExample(int nCycles);
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster);

// Definitions
#define	MIN_SPI_TXFER_SIZE 8 // min number of words per transfer
#define	MAX_SPI_TXFER_SIZE 512 // max number of words per transfer

int main(void)
{
    #if defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    PPSInput(1,SDI1, RPF2);  //Assign SDI1 to pin RF2
    PPSInput(1,SDI2, RPD3);  //Assign SDI2 to pin RD3
    PPSOutput(3,RPD4,SDO1); // Set RD4 pin as output for SDO1
    PPSOutput(2,RPB1,SDO2); // Set RB1 pin as output for SDO2
    #elif defined (__32MX220F032D__) || defined (__32MX250F128D__)
    PPSInput(2,SDI1, RPA1);  //Assign SDI1 to pin RA1
    PPSInput(3,SDI2, RPA4);  //Assign SDI2 to pin RA4
    PPSOutput(3,RPA2,SDO1); // Set RA2 pin as output for SDO1
    PPSOutput(2,RPA9,SDO2); // Set RA9 pin as output for SDO2
    #endif

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above.
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    srand(ReadCoreTimer()); // Seed the pseudo random generator

    if(!SpiDoMasterSlaveExample(100))
    {
	return 0;   // Our example failed
    }

    return 1; // Our example was a success
}

/*********************************************************************
 * Function: int SpiDoMasterSlaveExample(int nCycles)
 *
 * PreCondition: None
 *
 * Input: nCycles - number of repeated transfers
 *
 * Output: 1 (true) if the SPI transfer succeeded
 * 	   0 (false) otherwise
 *
 * Side Effects: None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib in a simple master/slave transfer mode
 *
 * Note: This test uses both SPI channels.
 *       The master channel (SPI1) sends data to a slave device (SPI2).
 *       The slave device relays data back to the master.
 *       This way we can verify that the connection to the slave is ok.
 * 	 Hardware connections have to be made:
 * 	 - SCK1 <-> SCK2
 * 	 - SDO1 <-> SDI2
 * 	 - SDI1 <-> SDO2
 * 	 - SS1  <-> SS2 (needed only if we use the framed mode)
 *
 ********************************************************************/
int SpiDoMasterSlaveExample(int nCycles)
{
    int fail = 0; // Overall result

    SpiInitDevice(SPI_CHANNEL1, 1, 1, 1);   // Initialize the SPI channel 1 as master, frame master
    SpiInitDevice(SPI_CHANNEL2, 0, 1, 0);   // Initialize the SPI channel 2 as slave, frame slave

    while(nCycles-- && !fail)
    {
        unsigned int	txferSize;
        unsigned short*	pTxBuff;
        unsigned short*	pRxBuff;

        txferSize=MIN_SPI_TXFER_SIZE+rand()%(MAX_SPI_TXFER_SIZE-MIN_SPI_TXFER_SIZE+1);	// Get a random transfer size

        pTxBuff=(unsigned short*)malloc(txferSize*sizeof(short));
        pRxBuff=(unsigned short*)malloc(txferSize*sizeof(short));   // We'll transfer 16 bits words

        if(pTxBuff && pRxBuff)
        {
            unsigned short*	pSrc=pTxBuff;
            unsigned short*	pDst=pRxBuff;
            int ix;
            int rdData;

            for(ix=0; ix<txferSize; ix++)
            {
                pTxBuff[ix]=(unsigned short)rand(); // Fill buffer with some random data
            }

            ix=txferSize+1; // Transfer one extra word to give the slave the possibility to reply back the last sent word

            while(ix--)
            {
                SpiChnPutC(1, *pSrc++);	// Send data on the master channel, SPI1
                rdData=SpiChnGetC(1);	// Get the received data

                if(ix!=txferSize)
                {   // Skip the first received character, it's garbage
                    *pDst++=rdData; // Store the received data
                }

                rdData=SpiChnGetC(2);   // Receive data on the slave channel, SPI2
                SpiChnPutC(2, rdData);	// Relay back data
            }

            // Now let's check that the data was received ok
            pSrc=pTxBuff;
            pDst=pRxBuff;

            for(ix=0; ix<txferSize; ix++)
            {
                if(*pDst++!=*pSrc++)
                {
                    fail=1; // Data mismatch
                    break;
                }
            }
        }

        else
        { // Memory allocation failed
            fail=1;
        }

        free(pRxBuff);
        free(pTxBuff);	// Free the allocated buffers
    }

    return !fail;
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
    SpiOpenFlags    oFlags=SPI_OPEN_MODE16|SPI_OPEN_SMP_END;    // SPI open mode

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
    // Not using SS in this example
    SpiChnOpen(chn, oFlags, 4);	
}


