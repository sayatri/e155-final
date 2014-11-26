
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
// ADC
//******************************************************************************
//******************************************************************************

// 5ksps for a system clock of 40 MHz, and PBCLKDIV = 2;

// Calculate PB clock time (T_PB) + sample plus convert time
//     T_PB = (1/40 MHz) * 2 = 50 ns
//     1/(5ksps) = .2 ms + converttime

// To calculate the ideal TAD. ADC min requirements for TAD.
//     \frac{.2 ms}{12+1} = 15.385 us = TAD = desired ADC clock period

// Double sample period to 2*TAD
//     \frac{.2 ms}{12+2} = 14.2857 us = TAD
//     2*TAD = 28.5814 us = sample time

// Calculate ADC clock divisor value
//     14.2857 ms/ (1/20Mhz) = 285.714; 
//     --> 284 --> ADC = 141
//     --> 286 --> ADC = 142

// Calculate sample rate using ADCS divisors
//     --> 284 --> \frac{1}{284*(12+2)*50ns} = 5030.181 sps
//     --> 286 --> \frac{1}{286*(12+2)*50ns} = 4995.005 sps


// Verify
//     TPB = (1/40 MHz) * 2 = 50 ns
//     TAD = 50 ns * 286 = 14.2 us
//     TAD * 2 = 28.6 us = sampletime

// Summary:
//     ACD = 142; ADC clock is PB divided by 286
//     SAMPC = 2; Sample time is 2 TAD periods 


void initADC(void) {
    
    // ADC Control Register 1
    AD1CON1bits.ON = 0; // ADC is off
    AD1CON1bits.SIDL = 0; // Continue module operation in Idle mode
    AD1CON1bits.FORM = 5; // ' Signed Integer 32-bit (DOUT = ssss ssss ssss ssss ssss sssd dddd dddd)
    AD1CON1bits.SSRC = 7;       // End sampling and start conversion when SAMP bit cleared
    AD1CON1bits.CLRASAM = 0;    // Buffer contents overwritten by next conversion sequence
    AD1CON1bits.ASAM = 0;       //  Sampling begins immediately after last conversion completes; 
                                // SAMP bit is automatically set
    AD1CON1bits.SAMP = 0;       // Don't start sampling upon initialization

    // AD1CON2: ADC Control Register 2
    AD1CON2bits.VCFG = 1;  //' Voltage reference - Vr+ (external), Vr- (AVss)
    AD1CON2bits.OFFCAL = 0;     // disable offset calibration mode
    AD1CON2bits.CSCNA = 0;      // do not scan inputs for mux
    AD1CON2bits.BUFM = 1;       // 1 = two 8-word buffers, 0 = one 16-word buffer
    AD1CON2bits.ALTS = 0;       // Always use MUX A input settings
	AD1CON2bits.SMPI = 7;        // Eight samples per interrupt

    // AD1CON3: ADC Control Register 3
    AD1CON3bits.ADRC = 0;       // Use PBCLK
    AD1CON3bits.SAMC = 3;       // Time audio sample bits
    AD1CON3bits.ADCS = 0b10001110;    //ADC Conversion Clock Select (142)
 
/*  Don't have these registers?
    // AD1CHS: ADC Input Select Register
    AD1CHS.CH0NA = 0;           // Channel 0 neg input is VR-
    AD1CHS.CH0SA = 0;           // Channel 0 pos input is AN0

	// TODO: Set TRISx to 1 
*/

    // AD1PCFG: ADC Port Configuration Register
    AD1PCFG = 0xFFFE;       // set AN0 as an analog input

    // Interrupt, set when SMPI condition met
    IFS1CLR = 2;                 // clear the ADC conversion interrupt
    IEC1SET = 2;                 // Enable ADC conversion interrupt                
}

void ADC_off(void){
    AD1CON1bits.ON = 0; // ADC is off
}

void ADC_on(void){
    AD1CON1bits.ON = 1; // ADC is on
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

void ADC_startManuAcq(void) {
 	delay(100);
    AD1CON1bits.SAMP = 1;   // the ADC SHA is sampling
}

void ADC_startAutoAcq(void) {
    delay(100);
    AD1CON1bits.ASAM = 1;
}

// stops sampling and starts conversion
void ADC_stopManuAcq(void) {
    AD1CON1bits.SAMP = 0; // 
}

// ONLY VALID FOR MANUAL MODE
// // return 1 when ADC conversion done
// // return 0 when conversion not done or not started
// char ADC_done(void) {
//     return AD1CON1bits.DONE; 
// }

//Interrupts disabled
// When the AD1IF flag is set, then....
// and then clears the flag
char ADC_getFlag(void) {
    return (!IFS1 & 0x0002);
}

void ADC_clearFlag(void) {
    IFS1CLR = 2; // clear interrupt
}

char ADC_done(void){
	return AD1CON1bits.DONE;
}
// Only valid when BUFM = 1
// 1 = ADC is currently filling buffer 0x8-0xF, user should access data in 0x0-0x7
// 0 = ADC is currently filling buffer 0x0-0x7, user should access data in 0x8-0xF
char ADC_bufferStatus(void) {
    return AD1CON2bits.BUFS;
}

//Check ADC Audo-Sample Time Bits
char ADC_sampleTime(void){
    return AD1CON3bits.SAMC;
}

// Calibrate for offset
short ADC_calibrateOffset(void){
    ADC_off();
    AD1CON2bits.OFFCAL = 1;

    ADC_on();

    ADC_startManuAcq();

    delay(100); // wait .1 seconds;

    ADC_stopManuAcq();


    while(!ADC_done());     // check if conversion done
   
    int offset = ADC1BUF0;

    ADC_off();              // Don't write to registers when ADC is on
    AD1CON2bits.OFFCAL = 0; // clear calibration mode

    return (short) offset;
}


//******************************************************************************
//******************************************************************************
// Timer
//******************************************************************************
//******************************************************************************

void initTMR1(void){
    T1CONbits.ON = 0; // disable timer for now
    T1CONbits.TCKPS = 0b11;    // 256 prescaler value
    T1CONbits.TCS = 0;      // Internal peripheral clock
}

void startTMR1(void){
    T1CONbits.ON = 1;
}

void stopTMR1(void){
    T1CONbits.ON = 0;
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
        signed short rawAudio [2000];
        short rawAudioIndex;
        
        initspi();
        initUART();
        initADC();

        ADC_offset = ADC_calibrateOffset();
           
        // set RD[7:0] to output, RD[11:8] to input
        //   RD[7:0] are LEDs
        //   RD[11:8] are switches
        TRISD = 0xFF00;

        // set RE[0] to input - for pushbutton enable
        // set RE[3:1] to output - for slave select
        TRISE = 0x0001; 




        printf("\nI am configured via UART correctly!\n");
        while(1) {
			PORTE = SARAH_FGPA;
            enable = PORTE;   // pushbutton enable
		
// ---- insert successful SPI code ----- //

            switch(Master_State)
            {
                case READY_TO_LISTEN:
                    // if pushbutton is pressed, then record audio
                    if (enable == 1) {
                        Master_State = LISTENING;
                        printf("Current state: Ready to Listen\n");
                    }
                    break;

                case LISTENING:
                    printf("Current state: Listening\n");

                    startTMR1();
                    ADC_on();
                    ADC_startAutoAcq();

                    while (TMR1 < 156250) {

                        while( (!IFS1 & 0x0002)){   // Use interrupt flag check if conversion done

    // 1 = ADC is currently filling buffer 0x8-0xF, user should access data in 0x0-0x7
    // 0 = ADC is currently filling buffer 0x0-0x7, user should access data in 0x8-0xF

                            if (ADC_bufferStatus()){
                                // store value from 0x0 - 0x7
                                rawAudio[rawAudioIndex] = (signed short) ADC1BUF0;
                                rawAudio[rawAudioIndex + 1] = (signed short) ADC1BUF1;
                                rawAudio[rawAudioIndex + 2] = (signed short) ADC1BUF2;
                                rawAudio[rawAudioIndex + 3] = (signed short)ADC1BUF3;
                                rawAudio[rawAudioIndex + 4] = (signed short) ADC1BUF4;
                                rawAudio[rawAudioIndex + 5] = (signed short) ADC1BUF5;
                                rawAudio[rawAudioIndex + 6] = (signed short) ADC1BUF6;
                                rawAudio[rawAudioIndex + 7] = (signed short)ADC1BUF7;                               
                            } else {
                                // store value from 0x8-0xF
                                rawAudio[rawAudioIndex] = (signed short) ADC1BUF8;
                                rawAudio[rawAudioIndex + 1] = (signed short) ADC1BUF9;
                                rawAudio[rawAudioIndex + 2] = (signed short) ADC1BUFA;
                                rawAudio[rawAudioIndex + 3] = (signed short)ADC1BUFB;
                                rawAudio[rawAudioIndex + 4] = (signed short) ADC1BUFC;
                                rawAudio[rawAudioIndex + 5] = (signed short) ADC1BUFD;
                                rawAudio[rawAudioIndex + 6] = (signed short) ADC1BUFE;
                                rawAudio[rawAudioIndex + 7] = (signed short)ADC1BUFF;    
                            }
                            
                            ADC_clearFlag();
                            rawAudioIndex += 9;
                        }
                    }

                    ADC_off();
                    stopTMR1();

                    if (ADC_bufferStatus()){
                        // store value from 0x0 - 0x7
                    } else {
                        // store value from 0x8-0xF
                    }
                            



                    
   

                    Master_State = FILTERING;

                    break;
                case FILTERING:
                    printf("You are done sampling");
                    printf("Current state: Filtering\n");
                    break;

                case TRANSMITTING:
                    printf("Current state: TRANSMITTING\n");
                    break;

                case PROCESSING_DATA:
                    break;

                case COMPLETED_PROCESSING:
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