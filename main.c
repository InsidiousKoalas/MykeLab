#include <msp430.h> 

/*
 * main.c
 *
 *
 *
 *       --------------- ADC Info -------------------
 *
 * ADC12 takes 13*ADC12CLK cycles for conversion. The total
 * time needed to take an analog sample to a digital representation
 * is 13*ADC12CLK + t_sample.
 *
 * t_sample is dictated by the ADC12SHT0x & ADC12SHT1x bits (sample &
 * hold time). t_sample abides by the following table:
 *
 * 							 |
 * 				ADC12SHTx    |  ADC12CLK Cycles
 *   		 ________________|___________________
 *   	 		  0000		 |		  4
 *   	  		  0001		 |		  8
 *   			  0010		 |		 16
 *   	 		  0100		 |		 32
 *   	 		 ...		 |		...
 *
 *   		  (table cont. on pg 482, slau208)
 *
 *
 * For continuous sampling, use repeat-single-channel-mode
 * of ADC.
 *
 *
 *
 */
#include <msp430.h>

#define   NUM_SAMPLES	32

volatile unsigned int samples[NUM_SAMPLES];
                                            // Needs to be global in this
                                            // example. Otherwise, the
                                            // compiler removes it because it
// is not used for anything.

int main(void)
{
	WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

	P4DIR |= BIT7;

	// PWM out
	P2DIR |= BIT0;              // P2.0 output
	P2SEL |= BIT0;              // P2.0 options select
	TA1CCR0 = 0xFFF;                          // PWM Period = 200 ns
	TA1CCR1 = 0x000;
	TA1CCTL1 = OUTMOD_6;                      // CCR1 reset/set
	TA1CTL = TASSEL_2 + MC_1 + TACLR + ID_0;  // SMCLK, up mode, clear TAR, div by 1

	
	// ADC
	P6SEL |= 0x02;                            // Enable A/D channel A0
	ADC12CTL0 = ADC12ON+ADC12SHT0_0+ADC12MSC; // Turn on ADC12, set sampling time (17 clk cycs/samp, ~57kHz)
	ADC12MCTL0 = ADC12INCH_1;
	// set multiple sample conversion
	ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2;       // Use sampling timer, set mode
	ADC12IE = 0x01;                           // Enable ADC12IFG.0
	ADC12CTL0 |= ADC12ENC;                    // Enable conversions
	ADC12CTL0 |= ADC12SC;                     // Start conversion

	__bis_SR_register(CPUOFF | GIE);       // Enter LPM4, Enable interrupts
//	__no_operation();                         // For debugger

	while(1){

		P4OUT ^= BIT7;
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
		__delay_cycles(0xFFFF);
	}

}


#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{



    samples[0] = ADC12MEM0;             // Move results
    TA1CCR1 = samples[0];

//    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)

}

