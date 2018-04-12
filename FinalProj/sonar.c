/*
 * Sonar.c
 *
 *  Created on: Nov 3, 2016
 *      Author: gemullen
 */

#include "timer.h"
#include "lcd.h"
#include "math.h"
//#include "driverlib/interrupt.h"
/*LAB7*/
void ping_init(){

	SYSCTL_RCGCGPIO_R |= BIT1; //Turn on PORTB sys clock
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Turn on clock to TIMER3

	GPIO_PORTB_DIR_R |= BIT3;
	GPIO_PORTB_DEN_R |= BIT3;

	TIMER3_CTL_R &= ~(0x100); 		//disable clock
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~(15 << 12)) | (7 << 12);

	TIMER3_CTL_R  |= (0b11 << 10); 	//set edge trigger to both positive and negative edge

	TIMER3_CFG_R |= 0x04;
	TIMER3_TBMR_R |= 0b11;    		//enable capture mode
	TIMER3_TBMR_R |= 0b100;   		// enable edge-time mode
	TIMER3_TBMR_R |= 0b10000; 		//set to count up
}


int ClkCyclesToCM(int clockCycles){ //distance away from object
	return 0.5 * (clockCycles /16000000.0) * 34000.0;
}

void send_pulse(){
	TIMER3_CTL_R &= ~TIMER_CTL_TBEN;
	GPIO_PORTB_AFSEL_R &= ~0x08;		//disable alt functions
	GPIO_PORTB_PCTL_R &= ~(7 << 12);

	GPIO_PORTB_DIR_R |= 0x08; 		// set PB3 as output
	GPIO_PORTB_DATA_R |= 0x08; 		// set PB3 to high
	timer_waitMicros(5);
	GPIO_PORTB_DATA_R &= 0xF7; 		// set PB3 to low
	GPIO_PORTB_DIR_R &= 0xF7; 		// set PB3 as input

	GPIO_PORTB_PCTL_R |= (7 << 12);
	GPIO_PORTB_AFSEL_R |= (0x08);	//enable alt functions
	TIMER3_CTL_R |= TIMER_CTL_TBEN;
}

unsigned ping_read(){
	send_pulse();
	while(!(TIMER3_RIS_R & TIMER_RIS_CBERIS)); 	// while BIT 3  is 0
	int wentHigh = TIMER3_TBR_R;					//get time of the rising edge of the pulse
	TIMER3_ICR_R |= TIMER_ICR_CBECINT;
	while(!(TIMER3_RIS_R & TIMER_RIS_CBERIS)); 	//while BIT 3 is 1
	int wentLow = TIMER3_TBR_R;						//get time of the falling edge of the pulse
	TIMER3_ICR_R |= TIMER_ICR_CBECINT;
	//lcd_printf("%d   %d", wentHigh, wentLow);
	if(wentHigh > wentLow){ // if one value has overflowed shift them both until the difference is positive //shifting will not affect the difference
		return 0xFFFFFF-wentHigh + wentLow;
	}
	else
	{
		return wentLow - wentHigh; 						//clock cycles between ping sent and recieved
	}
}


