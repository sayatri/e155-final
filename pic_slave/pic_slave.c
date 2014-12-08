

#include <plib.h>
#include <stdio.h>
#include <p32xxxx.h>
#include <math.h>
#include <string.h>
#include "YellowGreenRedOrange.h"

// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

// Describe the state of the PIC SLAVE
typedef enum _STATE
{
	WAITING_FOR_INPUT,
	PROCESS_DATA,
	DONE_PROCESSING,
	RESET_PRESSED
} STATE;


/*********
 GLOBALS
**********/


unsigned int fpga_results[4];

unsigned int audio_delays[4];


unsigned short best_index[2] = {0,0};
unsigned short best_value[2] = {0,0};

STATE Slave_State = WAITING_FOR_INPUT;

#define	DATA_READY	 		(PORTEbits.RE1)
#define SYSTEM_RESET 		(PORTEbits.RE0)
#define DOUBLE_INPUT 		(PORTEbits.RE2)
#define CORRELATION_ON 		(PORTDbits.RD11)
#define DEBUGGING_ON		(PORTDbits.RD10)

/***********
 PWM
************/

void initTimer(void)
{
    // Assumes peripheral clock at 20 MHz
    T2CONbits.ON = 0;          // Turn off timer to reset previous states
    T2CONbits.TCKPS = 0b000;   // 1:1 prescalar to preserve PWM resolution
    T2CONbits.T32 = 0;         // Enable 16-bit timer
    T2CONbits.TCS = 0;         // Use internal peripheral clock
}

void initPWM(void)
{
    OC5CONbits.ON = 0;         // Disable OC to reset previous state
    OC5CONbits.OC32 = 0;       // Compare to 16-bit timer source
    OC5CONbits.OCTSEL = 0;     // Use Timer 2
    OC5CONbits.OCM = 0b000;    // Turn off OC
    OC5CONbits.OCM = 0b110;    // Enable PWM mode without fault pin
}

void setupInterrupt(void)
{
    IFS0CLR = 0x00000100;      // Clear the Timer2 interrupt flag
    IEC0SET = 0x00000100;      // Enable the Timer2 interrupt
    IPC2SET = 0x0000001C;      // Set Timer2 interrupt priority to 7
}


/***********
 SPI
 ***********/

void initSPI(void) {
    char junk;

    IEC0CLR=0x03800000; // disable all interrupts
    SPI2CONbits.ON = 0; // disable SPI to reset any previous state
    junk = SPI2BUF; // read SPI buffer to clear the receive  SPI2SPIROV = 0; // clear the receive overflow flag
    SPI2BRG = 0; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
	SPI2CONbits.ENHBUF = 0;
    SPI2CONbits.MSTEN = 0; // enable master mode
    SPI2CONbits.SSEN = 1;   // enable SS pin for slave mode
	SPI2CONbits.SMP = 1; // input data sampled at end of data output time
    SPI2CONbits.CKE = 1; // set clock-to-data timing (data centered on rising SCK edge) 
    SPI2CONbits.ON = 1; // turn SPI on
    SPI2CONbits.MODE32 = 1; // 8-bit datawidth
    SPI2CONbits.MODE16 = 0; // 8-bit datawidth
}


unsigned int spi_send_receive(unsigned int send) {
    SPI2BUF = send; // send data to slave
    while (!SPI2STATbits.SPIRBF){
		if (SYSTEM_RESET) break; 	//until received buffer fills, indicating data received 
	}

    return SPI2BUF; // return received data and clear the read buffer full
}


/***********
 UART
 ***********/

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

/***************
 DTW ALGORITHM
 ***************/

float dtw(unsigned int *signal1, unsigned int *signal2, 
			int signal1_start, int signal2_start, int length)
{
    float result = 0;
    int x = 0;

    //for(x = 0; x < length; x++)
	while(x < length)
    {	
		if (SYSTEM_RESET) {
			Slave_State = RESET_PRESSED;
			break;
		}

        result += (signal1[signal1_start + x] - signal2[signal2_start + x]) * (signal1[signal1_start + x] - signal2[signal2_start + x]);
		x++;
    }
	
	if (SYSTEM_RESET) Slave_State = RESET_PRESSED;
	
    // Normalize the result to accomodate variable lengths
    result = sqrt(result) / (length * 1.0);

    return result;
}


/***************
 xcorrelation algorithm
 ***************/

void xcorr( unsigned int *a, unsigned int *b, int b_start, 
				int signalLength, long long* max, int* maxIndex){
	int n,k;
	int ca, cb;

	*max = 0;
	*maxIndex = 0;
	unsigned long long current;

	for( n = 0; n < 2*signalLength ; n++ ){
		
		current = 0;
		for (k = 0; k <= n; k++ ){
			if (n-k < signalLength){
				ca = a[n-k];
			} else {
				ca = 0;
			} 

			if (signalLength-1 >= k ) {
				cb = b[signalLength - k-1];
			} else {
				cb = 0;
			}

			current = current + ca*cb;
	//		results[n] = current;
			
		}
		if (current > *max) {
			*max = current;
			*maxIndex = n;
		}		
	}
}

/***************
 MAIN
 ***************/

int main(void)
{
	const char *names[4];
	names[0] = "yellow";
	names[1] = "green";
	names[2] = "red";
	names[3] = "orange";

	int encoding[4];
	encoding[0] = 3;
	encoding[1] = 2;
	encoding[2] = 1;
	encoding[3] = 4;

    

		
    // Initialize SPI/UART
    initUART();
    initSPI();

    //float dtw_results[4] = {0, 0, 0, 0};

	printf("\n\n==============================\n");
	printf("Slave PIC running with UART\n");

	printf("Turn on SW4 on the MASTER BOARD for double input. \n");
	printf("Turn on SW4 on the SLAVE BOARD for correlation on. \n");
	printf("Turn on SW3 on the SLAVE BOARD for verbose mode. \n");
	printf("Please reset the system after changing the settings.\n");
	printf("*Note, correlation is not time-efficient. \n\n");
	
	// Flag
	TRISE = 0b000101;

	// Switches input 
	// LEDS output
	TRISD = 0xFF00;
	PORTD = 0xFFFF;



	unsigned int input_audio[2000];	
	unsigned int input_audio2[2000];	
	unsigned int temp;
	unsigned int junk;
	int i;
	int j;
	int maxIndex;
	int maxIndex2;
	long long maxValue;
	long long maxValue2;
	int double_input;
	int correlation_on;
	int debugging_on;

	float min_dtw;
	float min_dtw2;
	int best_match;
	int best_match2;
	int delay;
	float dtwResult;



DATA_READY = 0;
while(1) { 

	
	switch(Slave_State)
	{
		// For debouncing the reset switch
		case RESET_PRESSED:
			i = 0;
			printf("Reset pressed \n\n");
			while(i < 99999) i++;

			Slave_State = WAITING_FOR_INPUT;
			break;
		case WAITING_FOR_INPUT:
			DATA_READY = 0; 
			// Receive the entirety of the input audio file
			printf("------------------------\n");
		
			double_input = DOUBLE_INPUT;
			correlation_on = CORRELATION_ON;
			debugging_on = DEBUGGING_ON;
			if (double_input) printf("\n*Double input is mode on. \n");
			if (correlation_on) printf("*Correlation mode is on. \n");
			if (debugging_on) printf("Debugging output is on. \n\n");
			
			
			junk = 0;
			i = 0;
			j = 0;
			unsigned short NUM_DATA = 2000; 

			// Takes in audio data from master PIC via SPI
			printf("\nReady for audio input.\n");
			while (i < NUM_DATA) {
				if (SYSTEM_RESET) break;
			
				temp = spi_send_receive(junk);
				input_audio[i] = temp;
				++i;
			}

			// If in double input, get second audio data
			if (double_input) {
				if (!SYSTEM_RESET) printf("\nReady for audio input 2.\n");
			
				junk = 0;
				i = 0;
				j = 0;
				unsigned short NUM_DATA = 2000; 
	
				while (i < NUM_DATA) {
					if (SYSTEM_RESET) break;
				
					temp = spi_send_receive(junk);
					input_audio2[i] = temp;
					++i;
				}
			}
			
			
			// check if system reset before changing states
			if (SYSTEM_RESET) {
				Slave_State = RESET_PRESSED;
				break;
			} else {
				Slave_State = PROCESS_DATA;
				break;
			}
			break;
			
		case PROCESS_DATA:
			printf("Processing audio input...\n");
			
			maxIndex = 0;
			maxValue = 0;

			min_dtw = 0xFFFF;
			min_dtw2 = 0xFFFF;
			best_match2 = 0;
			best_match = 0;
			delay;
			dtwResult;

			for (j = 0 ; j < 4 ; j ++ ) {
				maxValue = 0;
				delay = 0;
				
				// use correlation to find the "delay" of the two signals to best align
				// them before calculating dtw		
				if (correlation_on){
				//	printf("%i\n", j);
					printf("Performing correlation...\n");
					xcorr( input_audio, YellowGreenRedOrange, j*2000, NUM_DATA, &maxValue, &maxIndex);
	
					if (debugging_on) printf("the max index %i, and the max value %llu for %i\n", maxIndex, maxValue, j);
	
					delay = NUM_DATA - maxIndex; 
	
					if (debugging_on) printf("the delay is %i\n", delay);
					
				}
				
				// calculate dtw for input audio with audio in the bank
				if (debugging_on)printf("Performing DTW...\n"); 
				if (delay > 0) dtwResult = dtw(input_audio, YellowGreenRedOrange, 0, j*2000 + delay, NUM_DATA - delay);
				else dtwResult = dtw(input_audio, YellowGreenRedOrange, -delay, j*2000, NUM_DATA + delay);

				if (debugging_on) printf("     Result for %s: %f\n", names[j], dtwResult);

				if (dtwResult < min_dtw) {
					min_dtw = dtwResult;
					best_match = j; 
				}
			}

		
			// if in double-input mode
			if (double_input) {
				printf("Processsing audio input 2...\n");

				for (j = 0 ; j < 4 ; j ++ ) {
					maxValue = 0;
					delay = 0;
							
					if (correlation_on) {
						if (debugging_on)printf("Performing correlation...\n");
						xcorr( input_audio, YellowGreenRedOrange, j*2000, NUM_DATA, &maxValue, &maxIndex);
		
						if (debugging_on) printf("the max index %i, and the max value %llu for %i\n", maxIndex, maxValue, j);
		
						delay = NUM_DATA - maxIndex; 
		
						if (debugging_on) if (debugging_on) printf("the delay is %i\n", delay);
						
					}

					if (debugging_on)printf("Performing DTW...\n"); 
					if (delay > 0) dtwResult = dtw(input_audio2, YellowGreenRedOrange, 0, j*2000 + delay, NUM_DATA - delay);
					else dtwResult = dtw(input_audio2, YellowGreenRedOrange, -delay, j*2000, NUM_DATA + delay);
	
					if (debugging_on) printf("     Result for audio 2 %s: %f\n", names[j], dtwResult);
	
					if (dtwResult < min_dtw2) {
						min_dtw2 = dtwResult;
						best_match2 = j; 
					}
				}

			}
			
			if (SYSTEM_RESET) {
				Slave_State = RESET_PRESSED;
			} else {
				Slave_State = DONE_PROCESSING;
			}
			break;
		
		case DONE_PROCESSING:

			// what to print if only one audio
			if (!double_input) {
				if (debugging_on) printf("best match is %i\n", best_match);
				if (debugging_on) printf("min_dtw for %s: %f\n", names[best_match], min_dtw);

				printf("\nI think you said %s!\n", names[best_match]);
			
				if (debugging_on) printf("I am sending... %i \n", encoding[best_match]);

				// send result back to master pic
				DATA_READY = 1;
			//	spi_send_receive(encoding[best_match]);
				
				PORTD = encoding[best_match];
				printf("set porte to %i\n", encoding[best_match]);

			// if double audio
			} else {
				if (debugging_on){
					printf("best match for audio 1 is %i\n", best_match);
					printf("min_dtw for audio 1 is %s: %f\n", names[best_match], min_dtw);

				}

				if (debugging_on) printf("best match for audio 2 is is %i\n", best_match2);
				if (debugging_on) printf("min_dtw for audio 2 %s: %f\n", names[best_match2], min_dtw2);
				
				// if both audio matched, then we have a result
				if (best_match == best_match2) {
					printf("\nBoth recordings matched!\n");
					printf("I think you said %s!\n", names[best_match]);
					if (debugging_on) printf("I am sending... %i \n", encoding[best_match]);

					DATA_READY = 1;
				//	spi_send_receive(encoding[best_match]);
				//	DATA_READY = 0;
					PORTD = encoding[best_match];

					printf("set porte to %i\n", encoding[best_match]);

				// if audio did not match :(
				} else {
					printf("\nRecordings did not match. No match found! :(\n");
					if (debugging_on) printf("I am sending... %i \n", 0b1111);

					DATA_READY = 1;
				//	spi_send_receive(0b1111);
				//	DATA_READY = 0;
					PORTD = 0;
				} 
			}

  
			Slave_State = WAITING_FOR_INPUT;
			break;
	}
}	
}

