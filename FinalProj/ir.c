/*
 * ir.c
 *
 *  Created on: Nov 3, 2016
 *      Author: gemullen
 */

#include "timer.h"
#include "math.h"

void ir_init(){
	//enable ADC 0 module on port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	//enable clock for ADC
	SYSCTL_RCGCADC_R |= 0x1;
	//enable port D pin 0 to work as alternate functions
	GPIO_PORTB_AFSEL_R |= 0x10;
	//set pin to input - 0
	GPIO_PORTB_DEN_R &= 0b11101111;
	//disable analog isolation for the pin
	GPIO_PORTB_AMSEL_R |= 0x10;
	//initialize the port trigger source as processor (default)
	GPIO_PORTB_ADCCTL_R = 0x00;
	//disable SS0 sample sequencer to configure it
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN0;
	//initialize the ADC trigger source as processor (default)
	ADC0_EMUX_R = ADC_EMUX_EM0_PROCESSOR;
	//set 1st sample to use the AIN10 ADC pin
	ADC0_SSMUX0_R |= 0x000A;
	//enable raw interrupt
	ADC0_SSCTL0_R |= (ADC_SSCTL0_IE0 | ADC_SSCTL0_END0);
	//enable oversampling to average
	ADC0_SAC_R |= ADC_SAC_AVG_64X;
	//re-enable ADC0 SS0
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN0;
}

int IRVoltstoCM(int volts){

	if(volts >= 3200){
		return 9;
	}else if(volts < 3200 && volts > 420){

		return 11247/(pow((double)volts, 0.895)); //y=70998.0/(x^1.116) bot 10
	}else{
		return 80;
	}
}



