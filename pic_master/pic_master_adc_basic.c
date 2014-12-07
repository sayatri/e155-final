
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
    FILTERING,
	CALCULATE_MAX,
	TRANSMIT_FPGA1,
	TRANSMIT_FPGA2,
	TRANSMIT_SLAVE_PIC,
    // processing states
    // PROCESSING_DATA,
    // SENDING_DATA_TO_FPGAS,
    RECEIVE_DATA_FROM_FPGA1,
	CONFIRM_DATA_FROM_FPGA1,
     SENDING_DATA_PIC,
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

#define LISTEN_ENABLE				(PORTEbits.RE0)		// for pushbotton enable to listen, input

// Flags for FPGA1 Communication
#define FPGA1_SS					(PORTEbits.RE5)		// (output) turn on slave select for FPGA1
#define	FPGA1_RECEIVE_READY		(PORTEbits.RE7)		// (input) FPGA ready to receive input
#define FPGA1_TRANSMIT_READY			(PORTEbits.RE1)		// (input) FPGA1 finished processing data
#define	READY_TO_LISTEN_TO_FPGA1	(PORTEbits.RE4)		// (output) ready to communicate with FPGA1
#define FPGA1_CONFIRMED_RESULT		(PORTEbits.RE2)		// (input) FPGA1 confirmed that correct input received

                                                


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
    SPI3CONbits.MODE32 = 1; // 8-bit datawidth
    SPI3CONbits.MODE16 = 0; // 8-bit datawidth
}


unsigned int spi_send_receive3(signed int send) {
    SPI3BUF = send; // send data to slave
    while (!SPI3STATbits.SPIBUSY); // wait until received buffer fills, indicating data received 
    return SPI3BUF; // return received data and clear the read buffer full
}








// for spi4 --------------------------------
void initspi4(void) {
    char junk;

    IEC0CLR=0x03800000; // disable all interrupts
    SPI4CONbits.ON = 0; // disable SPI to reset any previous state
    junk = SPI4BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
    SPI4BRG = 7; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
    SPI4CONbits.MSTEN = 1; // enable master mode
    SPI4CONbits.MSSEN = 1;
    SPI4CONbits.ENHBUF = 0;
    SPI4CONbits.SMP = 1; // input data sampled at end of data output time
    SPI4CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
    SPI4CONbits.ON = 1; // turn SPI on
    SPI4CONbits.MODE32 = 1; // 8-bit datawidth
    SPI4CONbits.MODE16 = 0; // 8-bit datawidth
}


unsigned int spi_send_receive4(signed int send) {
    SPI4BUF = send; // send data to slave
    while (!SPI4STATbits.SPIBUSY); // wait until received buffer fills, indicating data received 
    return SPI4BUF; // return received data and clear the read buffer full
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
        
		unsigned int receivedSPI;
		unsigned int receivedBUFFER;
        unsigned char switches;
		unsigned char switchBuffer;
        unsigned char enable, fpga1_flag, fpga1_confirm;
		unsigned int sendjunk;
		unsigned int fpga1_results;
		unsigned int fpga2_results;
	
   
        unsigned char audioData; // Connected to AN0
		char str[80];
        unsigned int rawAudio [14000];
		unsigned int windowedAudio [2000];
        short rawAudioIndex = 0;
		short i = 0;
		int max = 0;
	
		int currentAudio;
		int maxIndex;
        
		initUART();
        initspi2();
       // initspi3();
        //initspi4();
        
        initadc();
		initTMR45();
		initTMR2();

           
        // set RD[7:0] to output, RD[11:8] to input
        //   RD[7:0] are LEDs
        //   RD[11:8] are switches
        TRISD = 0xFF00;

        // set RE[0] = 1 to input - for pushbutton enable
        // set RE[1] = fpga1_flag, input
		// set RE[2] = fpga1_confirm, input
		// set RE[4] = ready to listen, output
		// set RE[5] = turn on slave select for FPGA1 state, output
		// set RE[7] = fpga ready to receive, input
        TRISE = 0b1000111; //0x0007; 

		// set RB[7] = 1 for analog inputs
		TRISB = 0xFFFF;

		


        printf("\n\nI am configured via UART correctly!\n");
		// default not ready to listen
		
       while(1) {
	

		/*	if (Master_State == DONE) {

				printf("done\n");

			/*	printf("print raw audio data \n");
				for (i=0; i < rawAudioIndex;  i++) {
						printf("%i, %i\n", i, rawAudio[i]);
					}
				
				printf("print windowed audio data \n");
				for (i=0; i < 2000;  i++) {
					printf("%i, %i\n", i, windowedAudio[i]);
				} 

				printf("The final audio index is %i\n", rawAudioIndex);
				printf("Final time is %i ms\n", ((TMR5 << 16 | TMR4)/78));	
				printf("The sampling frequency of the ADC is %f Hz",  (rawAudioIndex/((TMR5 << 16 | TMR4)/78.0))*1000.0);
				while(1);
			} */

            switch(Master_State)
            {
                case READY_TO_LISTEN:
                    // if pushbutton is pressed, then record audio
					printf("Current state: Ready to Listen\n");
				
					// clear FPGA1 flags
					READY_TO_LISTEN_TO_FPGA1 = 0;
					FPGA1_SS = 0; 

                    if (LISTEN_ENABLE == 1) {
                        Master_State = LISTENING;
                    }
                    break;

                case LISTENING:
				
                    printf("Current state: Listening\n");
					startTMR45();	// sampling duration is 2 seconds
					
                	while ((TMR5 << 16 | TMR4) < 156250) {			
						startTMR2(); // prepares the ADC sampling at 7khz
						while(TMR2 < 11); // wait for 11 TMR2 cycles ~ 7kHz sampling
						rawAudio[rawAudioIndex++] = readadc();
						stopTMR2();				
            	   }
           	
                    stopTMR45();        
                    Master_State = CALCULATE_MAX;
                    break;
	
                case FILTERING:
                    printf("Current state: Filtering\n");
                    break;

                case CALCULATE_MAX:
                    printf("Current state: CALCULATE_MAX\n");
					
					
					max = 0;
					printf("Calculating max index...\n");
					for (i = 0; i < 12000; i++) {
						currentAudio = rawAudio[i];
					
						if (currentAudio > max) {				
							max = currentAudio;
							maxIndex = i;
						}							
					}

					printf("Found max. maxIndex is %i\n", maxIndex);
					Master_State = TRANSMIT_FPGA1;
					break;

				case TRANSMIT_FPGA1:
					FPGA1_SS = 1; // turn on slave select
					
					while(!FPGA1_RECEIVE_READY){
						printf("pic state 1\n");
					}
					
					printf("FPGA to be ready to receive\n");	
					if (maxIndex <= 500) {
						// sent over spi2
						for (i = 0; i < 2000; i++){
							printf("second loop %i, %i\n", i, rawAudio[i]);
							receivedSPI = spi_send_receive2(rawAudio[i]);				
						}

					} else {
						
					   for (i = 0; i < 2000; i++){
							printf("second loop %i, %i\n", i, rawAudio[maxIndex - 500 + i]);
							receivedSPI = spi_send_receive2(rawAudio[maxIndex - 500 + i]);
						}
					}

					printf("finished SPI transmission to FPGA1 \n");
					FPGA1_SS = 0; //turn off slave select

					Master_State = RECEIVE_DATA_FROM_FPGA1;

                    break;

                case RECEIVE_DATA_FROM_FPGA1:
					printf("State: RECEIVE_DATA_FROM_FPGA1\n");		
					
					// check for fpga ready to transmit
					while(!FPGA1_TRANSMIT_READY){
						printf("pic state 3\n"); 
					}
					
					// ******* PIC SHOULD BE IN STATE 3
					printf("FPGAG1 input ready \n");

					

					FPGA1_SS = 1; // turn on slave select, PIC goes to state 4

					
					// ******* PIC SHOULD BE IN STATE 4
					while(!FPGA1_RECEIVE_READY){
						printf("pic state 4\n");
					}
				
					fpga1_results = spi_send_receive2(sendjunk);

					FPGA1_SS = 0; //turn off slave select
					
					printf("Received results from FPGA1 [%u], pic should be in state 5", fpga1_results);

					Master_State = DONE;
					break;

				//	FPGA1_SS = 0; //turn off slave select

					Master_State = CONFIRM_DATA_FROM_FPGA1;

					break;
			
				case CONFIRM_DATA_FROM_FPGA1:
					
					FPGA1_SS = 1; //turn off slave select, PIC state 5 -> state 6
					while(!FPGA1_RECEIVE_READY){ 
						printf("pic state 6\n");
					}
					
					receivedSPI = spi_send_receive2(fpga1_results);

					FPGA1_SS = 0;	// turn off slave select
						
					if (FPGA1_CONFIRMED_RESULT) {
						printf("results from FPGA 1 are confirmed and are %u\n", fpga1_results);
						Master_State = DONE;
					} else {
						printf("Confirmation was not correct [ %u ] ... receiving again.\n", fpga1_results);
						Master_State = RECEIVE_DATA_FROM_FPGA1;
					}

                    break;
				case SENDING_DATA_PIC:
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
            
            receivedBUFFER = spi_send_receive2(switchBuffer);
            //printf("Received this value from slave PIC: %d\n\n", receivedSPI);
            if (receivedBUFFER != receivedSPI){
                receivedSPI = receivedBUFFER;
                PORTD = ((receivedSPI << 4 ) & 0x00F0 ) | switches;
                printf("Received this value from slave PIC: %d\n\n", receivedSPI);
            }

    */
//-------------------------------------------------------------------------------