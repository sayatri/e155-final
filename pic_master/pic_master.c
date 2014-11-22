
/*

PIC Master Configuration
Amy Ngai
mngai@hmc.edu

This code:
-   Configures the ADC to receive audio from 
-   Configures the SPI Master to transmit data to 3 different
    SPI slaves (another PIC and two FPGAs)
-

*/

// *****************************************************************************
// *****************************************************************************
// Includes
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
#include <plib.h>
#include <p32xxxx.h>

// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

// Describe the state of the PIC MASTER
typedef enum _STATE
{
    READY_TO_LISTEN,
    LISTENING_SENDING,
    // processing states
    PROCESSING_DATA,
    // SENDING_DATA_TO_FPGAS,
    // RECEIVED_DATA_FROM_ONE_FPGA,
    // RECEIVED_DATA_FROM_BOTH_FPGA,
    // SENDING_DATA_PIC,
    // RECEIVED_DATA_FROM_PIC,

    // completed processing
    COMPLETED_PROCESSING,
} STATE;


// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************

// Turns on the slave select for each slave
#define SLAVE_PIC_ON        (0x0002) // SS = RD1
#define AMY_FPGA            (0x0004) // SS = RD2
#define SARAH_FGPA          (0x0008) // SS = RD3

// Macros for ADC Configuration

// #define CONFIG1             (ADC_MODULE_OFF |   // Turns module off
//                             ADC_IDLE_CONTINUE | // module continues in Idle Mode
//                             ACD_FORMAT_INT16 |  // Data output integer 16-bit
//                             ADC_CLK_MANUAL |    // Clearing SAMP bit ends sampling and starts conversion
//                             ADC_AUTO_SAMPLING_ON |// Begins when SAMP bit is set
//                             ADC_SAMP_OFF)       // ADC sample hold
// #define CONFIG2             (ADC_SCAN_OFF |     // Scan select off
//                             ADC_ALT_BUF_ON)     // Buffer configured as two 8-word buffers?????
// #define CONFIG3             (ACD)CONV_CLK_SYSTEM)// Use PBCLK
// #define CONFIGPORT          (ENABLE_AN0_ANA)    // AN0 in analog input pin mode in Analog mode
// #define CONFIGSCAN          (SCAN_SCAN_AN0)     // Select AN0 for input scan
// #define CHANNELCONFIG       (ADC_CH0_POS_SAMPLEA_AN0 | // Use AN0 for input
//                            ADC_CH0_NEG_SAMPLEA_NVREF) /// Use ground as neg ref for AN0
//#define TWOSEC              (0x?????) // TODO
//)
                                                


// *****************************************************************************
// *****************************************************************************
// Global Variables
// *****************************************************************************
// *****************************************************************************

STATE Master_State = READY_TO_LISTEN;

// *****************************************************************************
// *****************************************************************************
// SPI Functions for PIC Master
// *****************************************************************************
// *****************************************************************************

void initspi(void) {
    char junk;

    IEC0CLR=0x03800000; // disable all interrupts
    SPI2CONbits.ON = 0; // disable SPI to reset any previous state
    junk = SPI2BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
    SPI2BRG = 7; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
    SPI2CONbits.MSTEN = 1; // enable master mode
    SPI2CONbits.MSSEN = 1;
    SPI2CONbits.ENHBUF = 0;
    SPI2CONbits.SMP = 1; // input data sampled at end of data output time
    SPI2CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
    SPI2CONbits.ON = 1; // turn SPI on
    SPI2CONbits.MODE32 = 0; // 8-bit datawidth
    SPI2CONbits.MODE16 = 0; // 8-bit datawidth
}


char spi_send_receive(signed char send) {
    SPI2BUF = send; // send data to slave
    while (!SPI2STATbits.SPIBUSY); // wait until received buffer fills, indicating data received 
    return SPI2BUF; // return received data and clear the read buffer full
}


// *****************************************************************************
// *****************************************************************************
// UART
// *****************************************************************************
// *****************************************************************************

void initUART(void) 
{
    // Configure UART
    // Using UART2 since nothing else uses PORTF

    TRISFbits.TRISF5 = 0; // RF5 is UART2 TX (output) 
    TRISFbits.TRISF4 = 1; // RF4 is UART2 RX (input)

    // Want rate of 115.2 Kbaud
    // Assuming PIC peripheral clock Fpb = Fosc / 2 = 20 MHz 
    // based on default instructions in lab 1.
    // U2BRG = (Fpb / 4*baud rate) - 1
    // -> U2BRG = 10 (decimal)
    // Actual baud rate 113636.4 (-1.2% error)
    U2BRG = 10;

    // UART2 Mode Register
    // bit 31-16: unused
    // bit 15:  ON = 1: enable UART
    // bit 14:  FRZ = 0: don't care when CPU in normal state
    // bit 13:  SIDL = 0: don't care when CPU in normal state
    // bit 12:  IREN = 0: disable IrDA
    // bit 11:  RTSMD = 0: don't care if not using flow control 
    // bit 10:  unused
    // bit 9-8: UEN = 00: enable U1TX and U1RX, disable U1CTSb and U1RTSb
    // bit 7:   WAKE = 0: do not wake on UART if in sleep mode 
    // bit 6:   LPBACK = 0: disable loopback mode
    // bit 5:   ABAUD = 0: don't auto detect baud rate
    // bit 4:   RXINV = 0: U1RX idle state is high
    // bit 3:   BRGH = 0: standard speed mode
    // bit 2-1: PDSEL = 00: 8-bit data, no parity 
    // bit 0:   STSEL = 0: 1 stop bit 
    U2MODE = 0x8000;

    // UART3 Status and control register 
    // bit 31-25: unused
    // bit 13: UTXINV = 0: U1TX idle state is high 
    // bit 12: URXEN = 1: enable receiver
    // bit 11: UTXBRK = 0: disable break transmission 
    // bit 10: UTXEN = 1: enable transmitter
    // bit 9: UTXBF: don't care (read-only)
    // bit 8: TRMT: don't care (read-only)
    // bit 7-6: URXISEL = 00: interrupt when receive buffer not empty 
    // bit 5: ADDEN = 0: disable address detect
    // bit 4: RIDLE: don't care (read-only)
    // bit 3: PERR: don't care (read-only)
    // bit 2: FERR: don't care (read-only)
    // bit 1: OERR = 0: reset receive buffer overflow flag
    // bit 0: URXDA: don't care (read-only)
    U2STA = 0x1400;
}



// Configure Timer Type B
//void initTMR2(void){
//    T1CONbits.ON = 1; // turn timer on
//    T1CONbits.TCKPS = 7; // prescale value is 256
//    T1CONbits.TCS = 0; // Use internal peripheral clock
//}
char getcharserial(void) 
{ 
	while (!U2STAbits.URXDA); 	// wait until data available
	return U2RXREG;			// return character received from serial port
}


void getstrserial(char *str) 
{
	int i = 0;
	do { 			// read an entire string until detecting
		str[i] = getcharserial(); 
	} while (str[i++] != '\r'); // look for return 
	str[i-1] = 0;	// null terminate the string
}
//******************************************************************************
//******************************************************************************
// Main
//******************************************************************************
//******************************************************************************

int main (void)
{
        
		unsigned char receivedSPI;
		unsigned char receivedBUFFER;
        unsigned char switches;
		unsigned char switchBuffer;
        unsigned char enable;
        unsigned int offset; // points to the base of the idle buffer
        unsigned char audioData; // Connected to AN0
		char str[80];
        
        initspi();
        initUART();
		// initTMR2(); //???

        // Configure and enable the ADC
		// CloseADC10();   // Ensure the ADC is off before setting
                        // the configuration
		// SetChanADC(CHANNELCONFIG); //Configure AN0 for input

        // Configure ADC using parameters above
		// OpenADC10(CONFIG1, CONFIG2, CONFIG3, CONFIGPORT, CONFIGSCAN);



   
        // set RD[7:0] to output, RD[11:8] to input
        //   RD[7:0] are LEDs
        //   RD[11:8] are switches
        TRISD = 0xFF00;

        // set RE[0] to output - for pushbuttom1
        // set RE[3:1] to output - for slave select
        TRISE = 0x0000; 



        printf("\nI am configured via UART correctly!\n");
        while(1) {
			PORTE = SARAH_FGPA;
		
            switchBuffer = (PORTD >> 8) & 0x000F;
			if (switchBuffer != switches){
				switches = switchBuffer;
    	      	PORTD = switches; // Read and mask RD[7:4]
                                         // display on LED
				printf("Switches are set to %d.\n", switches);
			}
		    
          	receivedBUFFER = spi_send_receive(switchBuffer);
//printf("Received this value from slave PIC: %d\n\n", receivedSPI);
			if (receivedBUFFER != receivedSPI){
				receivedSPI = receivedBUFFER;
				PORTD = ((receivedSPI << 4 ) & 0x00F0 ) | switches;
                printf("Received this value from slave PIC: %d\n\n", receivedSPI);
			}

            // switch(Master_State)
            // {
            //     case READY_TO_LISTEN:
            //         // if pushbutton is pressed, then record audio
            //         if (enable == 1) {
            //             Master_State = LISTENING;
            //         }
            //         break;

            //     case LISTENING_SENDING:
            //         EnableADC10(); // Enable the ADC
            //         AcquireADC10(); // Starts manual sample mode
                   
            //         // Determine which buffer is idle and create an offset
            //         // ReadActiveBufferADC10() = 0 when buffer locations
            //         // 0-7 are being written to ACD module; = 1 when 
            //         // locations 8-F are being written by the ADC module

            //         while(timer3 < 2SEC){ //TODO set a timer to count to 2sec
                    
            //             offset = 8 * ((~ReadActiveBufferADC10() & 0x01));


            //             // ReadADC10 returns int. Only want lower 8 bits
            //             // for SPI transmit. ???????
            //             audioData = (ReadADC10(offset) & 0xFF);
            //             clearSPI = spi_send_receive(audioData);
            //         }

            //         CloseADC10(); //turns off ADC module
            //         Master_State = PROCESSING_DATA;

            //         break;

            //     case PROCESSING_DATA:
            //         break;

            //     case COMPLETED_PROCESSING:
            //         break;

            //}
             
        }

}