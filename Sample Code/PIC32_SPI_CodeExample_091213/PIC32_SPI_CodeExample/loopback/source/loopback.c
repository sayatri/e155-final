/*********************************************************************
 *
 *   PIC32MX SPI Loopback Example
 *
 *********************************************************************
 * FileName:        loopback.c
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
 *      This test assumes that the SPI SDO output is connected to
 *      the SDI input. If data is transferred successfully, the
 *      first LED on the Explorer-16 is turned on.
 *
 * Notes:
 *    - On PIC32MX 2xx PIMS, connect pin RA1(SDI1) to pin
 *      RA2(SDO1). On PIC32MX 430 PIM, connect RF2(SDI1) to
 *      RD4(SDO1). All other devices connect pin RF7(SDI1) to pin
 *      RF8(SDO1).
 *    - PIC32MX 2xx PIMS are unconnected to the Explorer-16 LEDs.
 *      They must be soldered to the test points on top of the PIM
 *      for proper functionality. The README file contains a list
 *      of the connections that need to be made.
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
int SpiDoLoopbackExample(SpiChannel chn, int nWords);
void SpiInitDevice(SpiChannel chn, int isMaster, int frmEn, int frmMaster);

/*********************************************************************
 * Function: int main(void)
 *
 * PreCondition: None
 *
 * Input: None
 *
 * Output: 0 if some SPI transfer failed,
 * 	   1 if the SPI transfers suceeded
 *
 * Side Effects:None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib
 *
 * Note: None
 ********************************************************************/
int main(void)
{  
    #if defined (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__)
    PPSInput(1,SDI1, RPF2);  //Assign SDI1 to pin RF2
    PPSOutput(3,RPD4,SDO1); // Set RD4 pin as output for SDO1
    #elif defined (__32MX220F032D__) || defined (__32MX250F128D__)
    PPSInput(2,SDI1, RPA1);  //Assign SDI1 to pin RA1
    PPSOutput(3,RPA2,SDO1); // Set RA2 pin as output for SDO1
    #endif

    SpiChannel spiChn=SPI_CHANNEL1; // The SPI channel to use

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above.
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    // Set up the first LED on the Explorer to use later on
    mJTAGPortEnable(DEBUG_JTAGPORT_OFF);
    mPORTAClearBits(BIT_0);
    mPORTASetPinsDigitalOut(BIT_0);

    srand(ReadCoreTimer()); // Seed the pseudo random generator

    if(!SpiDoLoopbackExample(spiChn, 1024))
    {
        return 0;	// Our example failed
    }

    mPORTASetBits(BIT_0); // Turn on first LED to indicate the example was successful
    return 1; // Our example was a success
}

/*********************************************************************
 * Function: int SpiDoLoopbackExample(SpiChannel chn, int nWords)
 *
 * PreCondition: None
 *
 * Input: chn - the SPI channel to use
 * 		nWords	- number of words to transmit and receive for this test
 *
 * Output: 1 (true) if the SPI loopback transfer succeeded,
 * 	   0 (false) otherwise
 *
 * Side Effects: None
 *
 * Overview: Examples for the usage of the SPI Peripheral Lib for in a simple loopback mode
 *
 * Note: This test assumes that the SPI SDO output is connected to the SDI input.
 ********************************************************************/
int SpiDoLoopbackExample(SpiChannel chn, int nWords)
{
    SpiInitDevice(chn, 1, 0, 0);  // Initialize the SPI channel as master, no frame mode

    while(nWords--) // Keep going until there is no data left to transfer
    {
        unsigned short	txData, rxData;

        txData=(unsigned short)rand(); // Put random data into txData

        SpiChnPutC(chn, txData);    // Send data

        rxData=SpiChnGetC(chn);    // Retrieve the received data

        if(rxData!=txData) // If sent data != received data, there was an error
	{
            return 0;
	}
    }

    return 1; // Data transfer was successful
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
    SpiOpenFlags oFlags=SPI_OPEN_MODE16|SPI_OPEN_SMP_END;

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


