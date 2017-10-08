//***************************************************************************************
// ADC (analog-to-digital converter) for MSP430 (tested with an MSP430G2231)
//
// Description:
//  Reads voltage on analog pins and outputs digital on digital pins.
//
// References:
//  http://www.ti.com/lit/ug/slau144j/slau144j.pdf
//  https://www.embeddedrelated.com/showarticle/199.php and 179.php
//***************************************************************************************
//
// ADC10 Operation:
// The ADC converts an analog input to its 10-bit digital representation and stores the result in the ADC10MEM register.
// The ADC10 core is configured by two control registers, ADC10CTL0 and ADC10CTL1. It is enabled with the ADC10ON bit.
// The control bits can only be modified when ENC = 0. ENC must be set to 1 before any conversion can take place.
// Clock: The clock must remain active until the end of a conversion, otherwise the result is invalid.
// Reference voltage: Setting REFON = 1 enables the internal reference. When REF2_5V = 1, the internal reference
// is 2.5 V. When REF_2_5V = 0, the reference is 1.5 V.
// Low-Power Features: When REFON = 1, but no conversion is active, the buffer is automatically disabled and automatically re-
// enabled when needed. When the buffer is disabled, it consumes no current. When REFOUT = 1 and REFBURST = 1, the buffer is
// automatically disabled when the ADC10 is not actively converting and is automatically re-enabled when needed.
// Analog pin selection: the INCHx number corresponds to the analog pin (e.g. INCH_0 == A0 == P1.0; INCH_7 == A7 == P1.7)
//
//
// Which pins to use as analog inputs:
// In theory, all P1 pins (P1.0 to P1.7) can be used as analog inputs. But P1.1 and P1.2 are used for the backchannel UART and are connected to the Launchpad's FET.
// P1.3 has a pull-up and capacitor on it to supply the push button on the Launchpad.
// So P1.4, P1.5, P1.7 (A4, A5, A7) are candidates for undisturbed analog input.
// Ref: https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/160610


//#include <msp430g2230.h>
#include <msp430.h>

#ifndef TIMER0_A1_VECTOR
    #define TIMER0_A1_VECTOR TIMER0_A1_VECTOR
    #define TIMER0_A0_VECTOR TIMER0_A0_VECTOR
#endif

volatile long value;
int main(void)
{
    // Setup
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

    // Configure ADC10
    ADC10CTL1 = CONSEQ_0 +          // CONSEQ_0 = Single channel single-conversion
                INCH_7;             // INCH_x = Analog input pin number

    ADC10CTL0 = ADC10SHT_2 +        // Select the sample period (ADC10SHT_2 = 16 ADC10CLK cycles)
                ADC10ON +           // Enable ADC10
                ADC10IE +           // Enable ADC10 interrupt
                SREF_0;             // Reference voltage (SREF_0 = Use Vcc and Vss)

    ADC10AE0 = /*BIT4 + BIT5 + */BIT7;  // Enable ADC10 on analog pins A4, A5 and A7

    // TODO: send current to sensors only when performing a reading to prevent corrosion

    while(1)
    {
        // Read
        ADC10CTL0 |= ENC + ADC10SC;     // Enable and start the conversion

        value = ADC10MEM;

        // TODO: output digital values on pins x, x and x

        // Stop reading
        ADC10CTL0 &= ~ENC;              // Stop the conversion (negate the ENC bit of the ADC10CTL0 register)
        while (ADC10CTL1 & BUSY);       // Wait if ADC10 busy
    }
}



volatile long tempRaw;
int oldmain(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
	// P1DIR |= 0x01;					// Set P1.0 to output direction

	// P1DIR is an 8-bit register that controls the I/O direction of the Port 1 pins. If a bit is set to 0, it is configured as an input, otherwise it is an output.
	// The BIT0 constant is simply the 0x01 hex number
	P1DIR = BIT0;//0x41;
	P1OUT = 0;

	// Initialize unused Port 2 to avoid ULP Advisor Rule 4.1 remark during compilation.
	P2OUT = 0;
	P2DIR = 0xFF;

	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	BCSCTL3 |= LFXT1S_2;
	IFG1 &= ~OFIFG;
	BCSCTL2 |= SELM_0 + DIVM_3 + DIVS_3;

	while(1)
	{
	    ADC10CTL1 = INCH_10 + ADC10DIV_0;
	    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
	    _delay_cycles(5);
	    ADC10CTL0 |= ENC + ADC10SC;
	    P1OUT = 0x40;
	    _delay_cycles(10);
	    ADC10CTL0 &= ~ENC;
	    ADC10CTL0 &= ~(REFON + ADC10ON);

	    tempRaw = ADC10MEM;

	    P1OUT=0;
	    _delay_cycles(125000);
	}
}

void ConfigureADC(void)
{
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE; // Vcc & Vss as reference
    ADC10CTL1 = INCH_5 + ADC10DIV_3; // Channel 5, ADC10CLK/4
}
