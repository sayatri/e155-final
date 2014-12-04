#include <p32xxxx.h>

void initTimer(void)
{
    // Assumes peripheral clock at 20 MHz
    T2CONbits.ON = 0;        // Turn Timer off to reset previous state
    T2CONbits.TCKPS = 0b111; // 1:256 prescalar, 0.05us*256=12.8us
    T2CONbits.T32 = 0;       // Enable 16-bit timer
    T2CONbits.TCS = 0;       // Use internal peripheral clock
}

void initPWM(void)
{
    OC5CONbits.ON = 0;      // Disable OC to reset previous state
    OC5CONbits.OC32 = 0;    // Compare to a 16-bit timer source
    OC5CONbits.OCTSEL = 0;  // Use Timer2
    OC5CONbits.OCM = 0b000; // Turn off OC
    OC5CONbits.OCM = 0b111; // Enable PWM mode
}

int main(void) {
    // Set the period register of TMR2 to set the PWM period
    PR2 = 4995;

    // Set the initial PWM duty cycle
    OC5R = 440;

    // Enable timer and OC
    T2CONbits.ON = 1;
    OC5CONbits.ON = 1;

    while (1) {
        // Reset the timer to prevent overflow
        TMR2 = 0;

        // Set the buffer to set the next PWM duty cycle
        OC5RS = 

        while (OC5R <= TMR2) {}  // Wait for the current duty cycle to end
    }
}