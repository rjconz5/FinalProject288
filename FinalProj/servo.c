/*
 * servo.c
 *
 *  Created on: Nov 10, 2016
 *      Author: gemullen
 */

#include "timer.h"

#define ZEROD 0xC2C0
#define ONEEIGHTYD 0x5560

void Timer1_init(void){
	SYSCTL_RCGCGPIO_R |= BIT1; //Turn on PORTB sys clock
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Turn on clock to TIMER1

	GPIO_PORTB_DIR_R |= BIT5;
	GPIO_PORTB_DEN_R |= BIT5;


	GPIO_PORTB_AFSEL_R |= BIT5;
	GPIO_PORTB_PCTL_R |= (0b0111 << 20); //enable alt functions


	TIMER1_CTL_R &= ~(1 << 8); // BIT 8  //disable TBEN in GPTMCTL //Initialization and config for PWM Mode p716 book
	TIMER1_CFG_R = 0x04; //setup all timers

	TIMER1_TBMR_R = 0x2; //periodic timer mode
	TIMER1_TBMR_R &= ~(0 << 2); //edge count mode //clear bit 2
	TIMER1_TBMR_R |= (1 << 3); //PWM mode

	TIMER1_CTL_R &= ~(1 << 14); //make sure PWML is set to 0 for non-inverted mode

	TIMER1_TBPR_R =  0x04;
	TIMER1_TBILR_R = 0xE200; //TOP value //load timer start value 320000 clk cycles or 20 ms period


	TIMER1_TBPMR_R = 0x04;
	TIMER1_TBMATCHR_R = 0x8C10; // MATCH value 298000 clock cycles or 1.5 ms after the begining of the period 90 degrees



	TIMER1_CTL_R |= (1 << 8); //re-enable TBEN in GPTMCTL

}

void move_servo(double degrees, int direction){
	if(direction == 1  && (TIMER1_TBMATCHR_R + (degrees * 155.5555)) <= ZEROD){
		TIMER1_TBMATCHR_R += (degrees * 155.5555);
	}else if(direction == 0 && (TIMER1_TBMATCHR_R - (degrees * 155.5555)) >= ONEEIGHTYD){
		TIMER1_TBMATCHR_R -= (degrees * 155.5555);
	}
}

/*void servo_goto_degree(double degree){
	TIMER1_TBMATCHR_R =
}*/

void servo_0D(){
	TIMER1_TBMATCHR_R = ZEROD;
}

void servo_90D(){
	TIMER1_TBMATCHR_R = (ONEEIGHTYD + ZEROD) / 2;
}

void servo_180D(){
	TIMER1_TBMATCHR_R = ONEEIGHTYD;
}


