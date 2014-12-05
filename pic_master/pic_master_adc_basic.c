
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
#include <time.h>

// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

// Describe the state of the PIC MASTER
typedef enum _STATE
{
    READY_TO_LISTEN,
    LISTENING,
	WINDOWING,
    FILTERING,
    TRANSMITTING,
    // processing states
    PROCESSING_DATA,
    // SENDING_DATA_TO_FPGAS,
    // RECEIVED_DATA_FROM_ONE_FPGA,
    // RECEIVED_DATA_FROM_BOTH_FPGA,
    // SENDING_DATA_PIC,
    // RECEIVED_DATA_FROM_PIC,

    // completed processing
    COMPLETED_PROCESSING,
	DONE
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
// #define CONFIG1             (ADC_MODULE_OFF | ADC_IDLE_CONTINUE | ADC_FORMAT_INT16 | ADC_CLK_MANUAL |ADC_AUTO_SAMPLING_ON |ADC_SAMP_OFF)       // ADC sample hold
// #define CONFIG2             (ADC_SCAN_OFF | ADC_ALT_BUF_ON) 
// #define CONFIG3              (ADC_CONV_CLK_SYSTEM) // Use PBCLK
// #define CONFIGPORT          (ENABLE_AN0_ANA)    // AN0 in analog input pin mode in Analog mode
// #define CONFIGSCAN          (32'b0 & 32'b1)     // Select AN0 for input scan, skip rest

//                             // use AN0 for input, use ground as neg ref for AN0
// #define CHANNELCONFIG       (ADC_CH0_POS_SAMPLEA_AN0 | ADC_CH0_NEG_SAMPLEA_NVREF) 
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
    SPI2CONbits.MODE32 = 1; // 8-bit datawidth
    SPI2CONbits.MODE16 = 0; // 8-bit datawidth
}


unsigned int spi_send_receive(signed int send) {
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
// ADC
//******************************************************************************
//******************************************************************************


void initadc(void) {
	AD1CON1bits.ON = 0; // ADC is off
	AD1CHS = 0x00020000; // Connect RB2/AN2 as CH0 input
							// in this example RB2/AN2 is the input
	AD1PCFG =  0xFFFB; // PORTB = Digital; RB2 = analog
	AD1CON1bits.FORM = 0b100; // ' Integer 32-bit
	AD1CON1bits.ON = 1; 	 	// turn ADC on
	AD1CON1bits.SAMP = 1; 	 	// begin sampling
	AD1CON1bits.DONE = 0; 	 	// clear DONE flag
}
int readadc(void) {
	AD1CON1bits.SAMP = 0; 	 	// end sampling, star conversion
	while (!AD1CON1bits.DONE); // wait until DONE
	AD1CON1bits.SAMP = 1; 	 	// resume sampling
	AD1CON1bits.DONE = 0; 	 	// clear DONE flag
	return ADC1BUF0; 			// return result
}


//******************************************************************************
//******************************************************************************
// Timer
//******************************************************************************
//******************************************************************************

void initTMR45(void){
	T4CON = 0x0; // Stop any 16/32-bit Timer4 operation
	T5CON = 0x0; // Stop any 16-bit Timer5 operation
	T4CONbits.TCKPS = 0b111;	// 256 prescaler
	T4CONbits.T32 = 1;
	TMR4 = 0x0; // Clear contents of the TMR4 and TMR5
	PR4 = 0xFFFFFFFF; // Load PR4 and PR5 registers with 32-bit value

}

void startTMR45(void){
    T4CONbits.ON = 1;
	T5CONbits.ON = 1;
}

void stopTMR45(void){
    T4CONbits.ON = 0;
	T5CONbits.ON = 0;
//	TMR4 = 0x0; // Clear contents of the TMR4 and TMR5
//	TMR5 = 0x0; // Clear contents of the TMR4 and TMR5
}

// Use this timer for ADC sampling frequency
void initTMR2(void){
	T2CON = 0x0;	// Stop timer and clear control register
	T2CONbits.TCKPS = 0b111;	//256 prescaler
	TMR2 = 0x0;		// Clear contents of timer
	PR2 = 0xFFFF;	// Load period register
}

void startTMR2(void){
	TMR2 = 0x0;	// Clear contents
	T2CONbits.ON = 1;
}

void stopTMR2(void) {
	T2CONbits.ON = 0;
	
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
        unsigned int  offset; // points to the base of the idle buffer
        signed short ADC_offset;
        unsigned char audioData; // Connected to AN0
		char str[80];
        unsigned int rawAudio [14000];
		unsigned int windowedAudio [2000];
        short rawAudioIndex = 0;
		short i = 0;
		int max = 0;
		int currentAudio;
		int maxIndex;
        
        initspi();
        initUART();
        initadc();
		initTMR45();
		initTMR2();

           
        // set RD[7:0] to output, RD[11:8] to input
        //   RD[7:0] are LEDs
        //   RD[11:8] are switches
        TRISD = 0xFF00;

        // set RE[0] = 1 to input - for pushbutton enable
        // set RE[3:1] = 0to output - for slave select
		// set RB[4] = 0 for output for counter
        TRISE = 0x0001; 

		// set RB[7] = 1 for analog inputs
		TRISB = 0xFFFF;

		


        printf("\n\nI am configured via UART correctly!\n");
	
       while(1) {
			PORTE = SARAH_FGPA;
            enable = PORTE & 0x0001;   // pushbutton enable

			if (Master_State == DONE) {
				printf("done\n");
			/*	printf("print raw audio data \n");
				for (i=0; i < rawAudioIndex;  i++) {
						printf("%i, %i\n", i, rawAudio[i]);
					}
				
				printf("print windowed audio data \n");
				for (i=0; i < 2000;  i++) {
					printf("%i, %i\n", i, windowedAudio[i]);
				} */

				printf("The final audio index is %i\n", rawAudioIndex);
				printf("Final time is %i ms\n", ((TMR5 << 16 | TMR4)/78));	
				printf("The sampling frequency of the ADC is %f Hz",  (rawAudioIndex/((TMR5 << 16 | TMR4)/78.0))*1000.0);
				while(1);
			}

            switch(Master_State)
            {
                case READY_TO_LISTEN:
                    // if pushbutton is pressed, then record audio
					printf("Current state: Ready to Listen\n");
                    if (enable == 1) {
                        Master_State = LISTENING;
                        printf("Current state: Ready to Listen\n");
                    }
                    break;

                case LISTENING:
                    printf("Current state: Listening\n");
					startTMR45();	// sampling duration is 2 seconds
					
                	while ((TMR5 << 16 | TMR4) < 156250) {
						startTMR2(); // prepares the ADC sampling at 7khz

						// in case anything goes wrong, break before write outside of array
						if (rawAudioIndex == 14000){
							printf("Final time is %i ms\n", ((TMR5 << 16 | TMR4)/78));	
							Master_State = TRANSMITTING;
							break;
						}
						
						
						while(TMR2 < 11); // wait for 11 TMR2 cycles ~ 7kHz sampling

						rawAudio[rawAudioIndex++] = readadc();
						stopTMR2();				
            	   }
           			
                    stopTMR45();        
                    Master_State = TRANSMITTING;

                    break;
				case WINDOWING:
					printf("Current state: WINDOWING\n");
		
					max = 0;
					for (i = 0; i < 12000; i++) {
						currentAudio = rawAudio[i];
						printf("%i, %i\n", i, rawAudio[i]);
						if (currentAudio > max) {
						
							max = currentAudio;
							maxIndex = i;
						}							
					}
					
				
					printf("maxIndex is %i\n", maxIndex);
				
					
						if (maxIndex <= 500) {
							for (i = 0; i < 2000; i++){
								printf("second loop %i\n", i);
								windowedAudio[i] = rawAudio[i];
							}
						} else {
							for (i = 0; i < 2000; i++){
								printf("second loop %i\n", i);
								windowedAudio[i] = rawAudio[i-500];
							}
						}
					

					printf("Done windowing\n");

			
					Master_State = DONE;
					
					
					break;
                case FILTERING:

                    printf("Current state: Filtering\n");
                    break;

                case TRANSMITTING:
                    printf("Current state: TRANSMITTING\n");
					
					max = 0;
					for (i = 0; i < 12000; i++) {
						currentAudio = rawAudio[i];
						printf("%i, %i\n", i, rawAudio[i]);
						if (currentAudio > max) {
						
							max = currentAudio;
							maxIndex = i;
						}							
					}
					
				
					printf("maxIndex is %i\n", maxIndex);
				
					
						if (maxIndex <= 500) {
							for (i = 0; i < 2000; i++){
								printf("second loop %i, %i\n", i, rawAudio[maxIndex + i-500]);
							//	windowedAudio[i] = rawAudio[i];
							receivedSPI = spi_send_receive(rawAudio[i]);
							}
						} else {
							for (i = 0; i < 2000; i++){
								printf("second loop %i, %i\n", i, rawAudio[maxIndex + i-500]);
						//		windowedAudio[i] = rawAudio[i-500];
							receivedSPI = spi_send_receive(rawAudio[maxIndex + i-500]);
							}
						}

				/*	for (i = 0; i < 13000; i++) {

						receivedSPI = spi_send_receive(rawAudio[i]);	
					} */
					
					Master_State = DONE;
                    break;

                case PROCESSING_DATA:
                    break;

                case COMPLETED_PROCESSING:
                    break;
				case DONE:
					break;

            }
             
        } 

}


// Successful SPI Code
//-------------------------------------------------------------------------------
   /*         

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

    */
//-------------------------------------------------------------------------------