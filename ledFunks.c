#include <msp430f5529.h>
/*
 * ledFunks.c
 *
 *  Created on: Mar 24, 2019
 *      Author: bhunt
 */

#define		SCL		0x01
#define		SDA		0x02
#define		HOLD	25


int ledInit(int * commands){
	int i, ndx, ack=0;

	// hold before start
	P3OUT |= SCL;
	P3OUT |= SDA;
	__delay_cycles(HOLD);

	// start data transfer
	P3OUT &= ~SDA;
	__delay_cycles(HOLD);

	// send 3 initialization byte (Slave address, slave register, data byte)
	for(i=0; i<3; i++){
		// send byte to slave
		for(ndx=0; ndx<8; ndx++){

			P3OUT &=~ SCL;
			__delay_cycles(HOLD);
			if(0x80 & commands[i])(P3OUT |= SDA);	// set data line
			else(P3OUT &= ~SDA);				// clear data line


			P3OUT |= SCL; 		// toggle clock
			__delay_cycles(HOLD);
			if(ndx == 7){
				P3OUT &=~SCL;
			}
			commands[i] <<= 1;	// bitshift address
		}

		// receive acknowledge bit
		P3DIR &= ~SDA; 	// sda line as input to read acknowledge bit
		__delay_cycles(HOLD);
		P3OUT |= SCL;
		__delay_cycles(HOLD);
		ack = P3IN & SDA;	// 0x02 if P3IN is one, 0 otherwise

		// if no slave present, terminate I2C
		if (ack==1){
			break;
		}

		P3OUT &= ~SCL; 	// turn off clock

		// Hold SDA high and clock low, wait between bits
		P3DIR |= SDA;
		P3OUT |= SDA;
		__delay_cycles(HOLD*5);
	}

	// STOP CONDITION
	P3DIR |= SDA;
	P3OUT |= SCL + SDA;
	__delay_cycles(HOLD);

	// error statements
	return ack;
}


// This function converts hexadecimal values to values
// that can be transmitted via I2C using a lookup table
//int *flip(int *digits){
//	int l, i;
//	int toSend[16] = {0x7E, 0x30, 0x60, 0x79,
//					  0x33, 0x5B, 0x5F, 0x70,
//					  0x7F, 0x73, 0x77, 0x1F,
//					  0x0D, 0x3D, 0x4F, 0x47};
//
//
//	while(digits[l] != '\0'){
//		l++;
//	}
//	l--;
//
//	for(i = 0; i < l; i++){
//		digits[i] = toSend[i];
//	}
//	return digits;
//}



void ledWrite(int address, int * digits){
	int ndx, ack = 0,l = 0, startDigit=0x01;


	while(digits[l] != '\0'){
		l++;
	}


	int data[32] = {0};		// create array of data that can be transmitted
	int i = l;

	while (i>1){
		data[i] = digits[i-2];
		i--;
	}
	data[1] = startDigit;		// command for led bank 1
	data[0] = address;

//	comm[0] = 0x0

	// hold before start
	P3OUT |= SCL;
	P3OUT |= SDA;
	__delay_cycles(HOLD);

	// start data transfer
	P3OUT &= ~SDA;
	__delay_cycles(HOLD);

	// send 3 initialization byte (Slave address, slave register, data byte)
	for(i=0; i<l+2; i++){
		// send byte to slave
		for(ndx=0; ndx<8; ndx++){

			P3OUT &=~ SCL;
			__delay_cycles(HOLD);
			if(0x80 & data[i])(P3OUT |= SDA);	// set data line
			else(P3OUT &= ~SDA);				// clear data line


			P3OUT |= SCL; 		// toggle clock
			__delay_cycles(HOLD);
			if(ndx == 7){
				P3OUT &=~SCL;
			}
			data[i] <<= 1;	// bitshift address
		}

		// receive acknowledge bit
		P3DIR &= ~SDA; 	// sda line as input to read acknowledge bit
		__delay_cycles(HOLD);
		P3OUT |= SCL;
		__delay_cycles(HOLD);
		ack = P3IN & SDA;	// 1 if P3IN is one, 0 otherwise
		//	if (ack==1){
		//		return ack;
		//	}
		P3OUT &= ~SCL; 	// turn off clock

		// Hold SDA high and clock low, wait between bits
		P3DIR |= SDA;
		P3OUT |= SDA;
		__delay_cycles(HOLD*5);
	}

	// STOP CONDITION
	P3DIR |= SDA;
	P3OUT |= SCL + SDA;
	__delay_cycles(HOLD);

}
//
//
//void ledWrite();


