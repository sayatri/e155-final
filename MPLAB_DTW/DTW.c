/*  DTW.c
    Receives an audio signal from the master PIC
    Performs dynamic time warping to find similarity to signals in bank
    Sends string corresponding to signal of closest-matched signal
*/

#include <P32xxxx.h>
#include <plib.h>

void initspi(void) {
    char junk;

    SPI2CONbits.ON = 0;     // disable SPI to reset previous state
    junk = SPI2BUF;         // read SPI buffer to clear receive buffer
    SPI2BRG = 7;            // set BAUD rate to 1.25MHz, with Pclk at 20MHz
    //SPI2CONbits.MSSEN = 1;  // enable slave select SPI support
    SPI2CONbits.SSEN = 1;   // enable SS pin for slave mode
    SPI2CONbits.MSTEN = 0;  // enable slave mode
    SPI2CONbits.CKE = 1;    // set clock-to-data timing (centered on rising SCK)
    SPI2CONbits.MODE32 = 0; // set SPI to 8 bit data width 
    SPI2CONbits.MODE16 = 0; //
    SPI2CONbits.ON = 1;     // turn SPI on
}

unsigned char spireceivebyte(void) {
    while (!SPI2STATbits.SPIRBF);  // wait until receive buffer fills
    return SPI2BUF;                 // return the received data
}

void spisendbyte(unsigned char data) {
    SPI2BUF = data;
    while (!SPI2STATbits.SPIBUSY); // wait until received buffer fills
    char received = SPI2BUF;       // clear the read buffer full
    //PORTBbits.RB3 = 0;             // no more message to transmit
}

char processAudio(char* inputAudio, char bankSignalA, char bankSignalB) {
    // TODO: Do some dynamic time warping against the bank here
    return bankSignalA;
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

int main(void) {
   // SpiChannel spiChn = SPI_CHANNEL1;

    unsigned char result = 0x0;
	unsigned char oldSPIvalue;
    PORTD = result;

    // Configure TRISB/TRISD to be all outputs
    TRISB = 0x0000;
    TRISD = 0x0000;
    TRISF = 0x0007;

    // Set transmit flag to low
    //PORTBbits.RB3 = 0;

    // Create a char to store the received byte in
    // char data;

    while (1) {
        // Create a char array to store the bytes of data in
        //char inputSignal[5000] = {0};
        //char bankSignalA;
        //char bankSignalB;

        //while (PORTBbits.RB4 != 1);    // Wait for the master to be ready

        // Once receiving begins, receive the entire audio file and the two
           // chars representing which files in the bank had the highest scores
        // for (int i = 0; i < 5000; i++) {
            // inputSignal[i] = spireceivebyte();
        // }

        // bankSignalA = spireceivebyte();
        // bankSignalB = spireceivebyte();

        // // Process the audio
        // char result = processAudio(inputSignal, bankSignalA, bankSignalB);

        //TODO: TESTING WITH LEDS
        //result = spireceivebyte();
		PORTD = 0x005A;
		SPI2BUF = 0x00;
		oldSPIvalue = 0x00;
		if (SPI2BUF != oldSPIvalue) {
        	PORTD = SPI2BUF; //result;
			oldSPIvalue = PORTD;
		}
        // SpiChnChangeMode(chn, 0, 0, 1);
        // result = SpiChnGetC(chn);
        // SpiChnChangeMode(chn, 1, 1, 1);
        // SpiChnPutC(chn, result);

        // Set the flag to indicate to the master that PIC is ready to transmit
        //PORTBbits.RB3 = 1;

        //while (PORTBbits.RB4 != 1);    // Wait for the master to pay attention
        //spisendbyte(result);     // Then transmit the result

        // Receive the data
        // if (SPISS3 == 0) {
            // data = spireceivebyte();
        // }

        // Write the received data to the LEDs
        // PORTD = data;
    }
}