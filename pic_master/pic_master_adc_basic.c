
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
// Macros
// *****************************************************************************
// *****************************************************************************

#define LISTEN_ENABLE           (PORTBbits.RB3)	 // for pushbotton enable to listen, input
#define ENABLE_DOUBLE_REC       (PORTDbits.RD11) // enables error-checking double-recording mode
#define PIC_READY_TO_TRANSMIT   (PORTEbits.RE4)  // flag indicating the slave PIC has data to send
#define SYSTEM_RESET            (PORTEbits.RE5)

// System state codes to send to FPGA
#define CODE_READY_TO_LISTEN    (0b00000001)
#define CODE_LISTENING          (0b00000010)
#define CODE_PROCESSING         (0b00000011)
#define CODE_DONE               (0b00000100)
#define CODE_RESTART            (0b00000111)

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
    FILTERING,
	CALCULATE_MAX,
	TRANSMIT_SLAVE_PIC,
    RECEIVE_RESULTS_SLAVE_PIC,
    // processing states
    // PROCESSING_DATA,
    // SENDING_DATA_TO_FPGAS,
    // RECEIVED_DATA_FROM_PIC,

    // completed processing
    COMPLETED_PROCESSING,
	DONE
} STATE;                                                


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



// for spi2 -------------------------------------
void initspi2(void) {
    char junk;

    IEC0CLR=0x03800000; // disable all interrupts
    SPI2CONbits.ON = 0; // disable SPI to reset any previous state
    junk = SPI2BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
    SPI2BRG = 0; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
    SPI2CONbits.MSTEN = 1; // enable master mode
    SPI2CONbits.MSSEN = 1;
    SPI2CONbits.ENHBUF = 0;
    SPI2CONbits.SMP = 1; // input data sampled at end of data output time
    SPI2CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
    SPI2CONbits.ON = 1; // turn SPI on
    SPI2CONbits.MODE32 = 1; // 8-bit datawidth
    SPI2CONbits.MODE16 = 0; // 8-bit datawidth
}


unsigned int spi_send_receive2(signed int send) {
    SPI2BUF = send; // send data to slave
    while (!SPI2STATbits.SPIBUSY); // wait until received buffer fills, indicating data received 
    return SPI2BUF; // return received data and clear the read buffer full
}



// for spi3 ------------------------------
void initspi3(void) {
    char junk;

    IEC0CLR=0x03800000; // disable all interrupts
    SPI3CONbits.ON = 0; // disable SPI to reset any previous state
    junk = SPI3BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
    SPI3BRG = 7; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
    SPI3CONbits.MSTEN = 1; // enable master mode
    SPI3CONbits.MSSEN = 1;
    SPI3CONbits.ENHBUF = 0;
    SPI3CONbits.SMP = 1; // input data sampled at end of data output time
    SPI3CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
    SPI3CONbits.ON = 1; // turn SPI on
    SPI3CONbits.MODE32 = 0; // 8-bit datawidth
    SPI3CONbits.MODE16 = 0; // 8-bit datawidth
}


char spi_send_receive3(char send) {
    SPI3BUF = send; // send data to slave
    while (!SPI3STATbits.SPIRBF); // wait until received buffer fills, indicating data received 
    return SPI3BUF; // return received data and clear the read buffer full
}








// for spi4 --------------------------------
//void initspi4(void) {
//    char junk;
//
//    IEC0CLR=0x03800000; // disable all interrupts
//    SPI4CONbits.ON = 0; // disable SPI to reset any previous state
//    junk = SPI4BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
//    SPI4BRG = 7; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
//    SPI4CONbits.MSTEN = 1; // enable master mode
//    SPI4CONbits.MSSEN = 1;
//    SPI4CONbits.ENHBUF = 0;
//    SPI4CONbits.SMP = 1; // input data sampled at end of data output time
//    SPI4CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
//    SPI4CONbits.ON = 1; // turn SPI on
//    SPI4CONbits.MODE32 = 1; // 8-bit datawidth
//    SPI4CONbits.MODE16 = 0; // 8-bit datawidth/
//}


//unsigned int spi_send_receive4(signed int send) {
//    SPI4BUF = send; // send data to slave
//    while (!SPI4STATbits.SPIBUSY); // wait until received buffer fills, indicating data received 
//    return SPI4BUF; // return received data and clear the read buffer full
//}




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
	TMR4 = 0x0; // Stop any 16/32-bit Timer4 operation
	TMR5= 0x0; // Stop any 16-bit Timer5 operation
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
        
		unsigned int receivedSPI;
		unsigned int receivedBUFFER;
		unsigned int sendjunk;
   
        unsigned char audioData; // Connected to AN0
		char str[80];
        char junk;
        unsigned int rawAudio [14000];
        short rawAudioIndex = 0;
		short i = 0;
		int max = 0;
        unsigned int best_match = 0;

        short recorded_twice = 0;
	
		int currentAudio;

		int maxIndex1;
        int maxIndex2;
        
		initUART();
        initspi2();
        initspi3();
        //initspi4();
        
        initadc();
		initTMR45();
		initTMR2();

        TRISD = 0xFFF;
        TRISE = 0b00111111; 
		TRISB = 0xFFFC;     // 1111_1111_1111_1100

		
       while(1) {
			
            switch(Master_State)
            {
                case READY_TO_LISTEN:
                    // if pushbutton is pressed, then record audio
					printf("\nWaiting for audio input...\n");
				
                    while (!LISTEN_ENABLE && !SYSTEM_RESET) {
                       // Send state to FPGA
                       junk = spi_send_receive3(CODE_READY_TO_LISTEN);
                    }
					
                    if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                    }
                    else {
    					Master_State = LISTENING;
					}

                    break;

                case LISTENING:
                    // Send state to FPGA
                    junk = spi_send_receive3(CODE_LISTENING);

				    printf("Recording...\n");
					
                    rawAudioIndex = 0;
					startTMR45();	// sampling duration is 2 seconds
					 while ((TMR5 << 16 | TMR4) < 156250) {	
						startTMR2(); // prepares the ADC sampling at 7khz
						while(TMR2 < 11); // wait for 11 TMR2 cycles ~ 7kHz sampling
  
						rawAudio[rawAudioIndex++] = readadc();
						stopTMR2();				
                	}
           	
                    stopTMR45();   

                    if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                    }
                    else {
    					Master_State = CALCULATE_MAX;
					}

                    break;

                case CALCULATE_MAX:
                    // Send state to FPGA
                    junk = spi_send_receive3(CODE_PROCESSING);
									
					max = 0;

					for (i = 0; i < 12000; i++) {
                        if (SYSTEM_RESET) {
                            recorded_twice = 0;
                            break;
                        }
                        else {
						    currentAudio = rawAudio[i];
						    if (currentAudio > max) {				
						    	max = currentAudio;
							    maxIndex1 = i; 
						    }
                        }						
					}
                    
                    if (SYSTEM_RESET) {
                        recorded_twice = 0;
                        Master_State = READY_TO_LISTEN;
                    }
                    else {
    					Master_State = TRANSMIT_SLAVE_PIC;
					}

					break;
               
				case TRANSMIT_SLAVE_PIC:
                    // Send state to FPGA
                    junk = spi_send_receive3(CODE_PROCESSING);

                    printf("Transmitting to PIC for processing...\n");
						
					if (maxIndex1 <= 500) {
						for (i = 0; i < 2000; i++){
                            if (SYSTEM_RESET) {
                                break;
                            }

							receivedSPI = spi_send_receive2(rawAudio[i]);				
						}

					}
                    else {	
					   for (i = 0; i < 2000; i++){
                            if (SYSTEM_RESET) {
                                break;
                            }

							receivedSPI = spi_send_receive2(rawAudio[maxIndex1 - 500 + i]);
						}
					}

					printf("Finished transmission to slave PIC. \n");

                     if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                    }
                    else if (ENABLE_DOUBLE_REC & !recorded_twice) {
                        printf("Double recording mode enabled.\n");
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 1;
                    }
                    else {
    					Master_State = RECEIVE_RESULTS_SLAVE_PIC;
                       //Master_State = READY_TO_LISTEN
					}

					break;
                
                case RECEIVE_RESULTS_SLAVE_PIC:
                    printf("Waiting for results from PIC...\n");

                    // Send state to FPGA
                    junk = spi_send_receive3(CODE_PROCESSING);
 
                    // Wait for the PIC to be ready
                    while (!PIC_READY_TO_TRANSMIT && !SYSTEM_RESET);

                    if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                        break;
                    }

                    // Let FPGA know processing is done
                    junk = spi_send_receive3(CODE_DONE);

                    // Get the results from the PIC
                    //best_match = spi_send_receive2(0xFFFF);

                    //printf("PIC says %i was best.\n", best_match);

                    if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                    }
                    else {
    					Master_State = DONE;
					}
                    break;
                    
				case DONE:
                    printf("Finished!\n");

                    if (SYSTEM_RESET) {
                        Master_State = READY_TO_LISTEN;
                        recorded_twice = 0;
                        break;
                    }

                    // Send results to FPGA
                    junk = spi_send_receive3((char)best_match);

                    Master_State = READY_TO_LISTEN;
                    recorded_twice = 0;
					break;
            }
        } 
}
