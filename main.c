#include <msp430.h>
#include "ledFunks.h"

#define NUM_SAMPLES 	32

int sample, samp_vals[NUM_SAMPLES];
int avg(int*);
int *toDisp(int);


int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // LED Init
  P3DIR |= 0x03;		// Set P6.0 & P6.1 as outs
  P3DIR &= ~0xFC;		// init other pins as ins

  int sListFlag, disp[4];
  int* dispBuf;
  unsigned int i=0;

  int coms[3] = {0x76, 0x00, 0x37};
  sListFlag = ledInit(coms);

  // initialize display to dashes
	disp[0] = 0x80;
	disp[1] = 0x80;
	disp[2] = 0x80;
	disp[3] = 0x80;

	ledWrite(0x76,disp);


  // ADC Init

  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12MCTL0 = ADC12INCH_1;					// multiplex ADC to P6.1
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x02;                            // P6.1 ADC option select
  P1DIR |= 0x01;                            // P1.0 output
  P4DIR |= BIT7;

  int avgVal,ndx=0;

  // Thresholds
  int ths[9] = {0x000,0x150,0x250,0x300,0x350,0x400,0x500,0x600,0x700};


  while (1)
  {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __bis_SR_register(CPUOFF + GIE);     // LPM0, ADC12_ISR will force exit
    P1OUT ^= BIT0;

    samp_vals[ndx] = sample;
    ndx++;
    if(ndx==NUM_SAMPLES)(ndx=0);


    avgVal = avg(samp_vals);

    for(i=0; i<7; i++){
    	if(avgVal<ths[i]){
    		break;
    	}
    }

    dispBuf = toDisp(5);
    ledWrite(0x76,dispBuf);


  }
}


#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
	sample = ADC12MEM0;
	__bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
}



int avg(int* samp_reg){
	int i, sum;

	for(i=0; i<32; i++){
		samp_reg[i] -= 0x700;	// 'AC couple'

		// rectify
		if(samp_reg[i]<0){
			int t = samp_reg[i] >> 31;
			samp_reg[i] = (samp_reg[i]^t) - t;
		}

		sum += samp_reg[i];			// sum vals in samp_reg
	}

	return (int) sum*.03125;		// cast avg val to int
}


int * toDisp(int ndx){
	int vals[3] = {0x80,0xB0,0xB6};
	int poss[9][4] = {{0,0,0,0},{0,0,0,1},{0,0,0,2},
					  {0,0,1,2},{0,0,2,2},{0,1,2,2},
					  {0,2,2,2},{1,2,2,2},{2,2,2,2}};
	int i,temp;
	static int disp[4];


	for(i=0; i<4; i++){
		temp = poss[ndx][i];

		disp[i] = vals[temp];
	}

	return disp;


}
