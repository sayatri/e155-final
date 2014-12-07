

#include <plib.h>
#include <stdio.h>
#include <p32xxxx.h>
#include <math.h>
#include "HelloByeYesNo.h"


/*********
 DEFINES
*********/
#define	HELLO_OFFSET		(0)
#define	BYE_OFFSET			(2000)
#define YES_OFFSET			(4000)
#define	NO_OFFSET			(6000)	

/*********
 GLOBALS
**********/

unsigned short NUM_DATA = 2000;

unsigned short i = 0;
unsigned int input_audio[2000];
unsigned int fpga_results[4];

unsigned int audio_delays[4];
float dtw_results[2];

unsigned short best_index[2] = {0,0};
unsigned short best_value[2] = {0,0};



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
    SPI2BRG = 7; //set BAUD rate to 1.25MHz, with Pclk at 20MHz 
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
    while (!SPI2STATbits.SPIRBF); // wait until received buffer fills, indicating data received 
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

float dtw(int *signal1, int *signal2, int signal1_start, int signal2_start, int length)
{
    int result = 0;
    int x;

    for(x = 0; x < length; x++)
    {
        result += (signal1[signal1_start + x] - signal2[signal2_start + x]) * (signal1[signal1_start + x] - signal2[signal2_start + x]);
    }

    // Normalize the result to accomodate variable lengths
    result = sqrt(result) / length;

    return result;
}


/***************
 MAIN
 ***************/

int main(void)
{
    // GET THE AUDIO FILE ---------

		
    // Initialize SPI/UART
    initUART();
    initSPI();

	int offset[4] = {HELLO_OFFSET, BYE_OFFSET, YES_OFFSET, NO_OFFSET};

	char names[4][5] = {"hello", "bye", "yes", "no"};

	printf("Slave PIC running with UART\n");
    printf("Waiting for input audio data...\n");

    // Receive the entirety of the input audio file
    unsigned int temp;
    unsigned int junk = 0;
	int j;
	int k;
 
    while (i < NUM_DATA) {
        temp = spi_send_receive(junk);
        input_audio[i] = temp;
        ++i;
    }

    //// Print out received data
    //unsigned short j = 0;
    //for ( ; j < NUM_DATA; j++) {
    //    printf("%i, %i\n", j, audio[j]);
    //}

    /*printf("Waiting for results of FPGA...\n");

    // Receive the results of cross-correlation via SPI
    unsigned int fpga_data[4];

    int j = 0;
    for ( ; j < 4; j++) {
        temp = spi_send_receive(junk);
        fpga_data[j] = temp;
    }

    printf("Parsing FPGA results...\n");

    // Parse the data, keep track of best 2
    // Data format:  [31:28] = identifier   mask: 0xF0000000
    //               [27:15] = max index    mask: 0x0FFF8000
    //               [14:0]  = max value    mask: 0x00007FFF
    j = 0;
    for ( ; j < 4; j++) {
        // Mask out the identifier to match the data to a bank audio file
        unsigned int bank_id = ((fpga[j] & 0xF0000000) >> 28);

        // Mask out the max index and max value and save the data
        audio_delays[bank_id] = NUM_DATA - ((fpga[j] & 0x0FFF8000) >> 15);
        fpga_results[bank_id] = (fpga[j] & 0x00007FFF);

        // Keep track of the best results
        if (fpga_results[bank_id] > best_value[0]) {
            best_index[1] = best_index[0];
            best_value[1] = best_value[0];
            best_index[0] = bank_id;
            best_value[0] = fpga_results[bank_id];
        }
        else if (fpga_results[bank_id] > best2) {
            best_index[1] = bank_id;
            best_value[1] = fpga_results[bank_id];
        }
    } */

    printf("Performing DTW...\n");

	j = 0;
	k = 0;

	int audiobankOffset;
	int max_dtw = 0;
	int best_match;

	// processing dtw
	for ( ; j < 4 ; j ++ ) {
		
		printf("Performing DTW for %s\n", names[j]);
		audiobankOffset = offset[j];
		dtw_results[j] = dtw(input_audio, HelloByeYesNo, 0, audiobankOffset, NUM_DATA);
		
		if (dtw_results[j] > max_dtw) {
			max_dtw = dtw_results[j] ;
			best_match = j;
		}
	}

	printf("I think you said %s!\n", names[j]);
}

	

    // Perform DTW
    /* j = 0;
    for ( ; j < 2; j++) {
        short delay = audio_delays[j];

        // If the delay is negative, the bank signal starts first
        //       ""        positive, the input signal starts first
        if (delay < 0) {
            dtw_results[j] = dtw(input_audio, audio_bank[best_index[j]], delay, 0, NUM_DATA + delay);
        }
        else {
            dtw_results[j] = dtw(input_audio, audio_bank[best_index[j]], 0, delay, NUM_DATA - delay);
        }  
    } */


    //Print out the best match using UART
    //TODO: Tolerance level for a "match"
/*
    if (dtw_results[0] > dtw_results[1]) {
        printf("The closest matched word was %s", audio_bank[best_index[0]]);
    }
    else {
        printf("The closest matched word was %s", audio_bank[best_index[1]]);
    } */
//}

    // PWM CODE BELOW
    //// Reset the index for playback
    //i = 0;
    //
    //// Enable system-wide interrupt to multivectored mode
    //INTEnableSystemMultiVectoredInt();
    //
    //initTimer();
    //initPWM();
    //
    //// Set initial values
    //PR2 = 2932;
    //OC5R = 0;
    //
    //setupInterrupt();
    //
    //// Turn on the timer and PWM
    //T2CONbits.ON = 1;
    //OC5CONbits.ON = 1;
    //
    // while (1) { }


//void __ISR(_TIMER_2_VECTOR, ipl7) T2_IntHandler (void)
//{   
//    if (i < NUM_DATA) {
//        OC5RS = PR2* (audio[i]/1024.0);          // Load the next duty cycle from the array
//        ++i;                       // Move to the next piece of data
//    }
//    else {
//        //OC5RS = 1000;                  // Stop audio after the signal is done
//        OC5RS = audio[0];                // Loop audio
//        i = 0;
//    }
//  
//    TMR2 = 0;                  // Reset Timer2
//    IFS0CLR = 0x0100;          // Clear the Timer2 interrupt flag
//}
