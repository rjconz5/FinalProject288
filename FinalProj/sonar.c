/**
*         @file sonar.c
*         @breif This class is what is used to control the robot's PING
*         sensor. It provides the user with all valueable functions to use
*					this sensor correctly.
*
*         @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*         @date 4/24/2018
*
*/

#include "timer.h"
#include "lcd.h"
#include "math.h"

/**
*       This method will be used to setup and initialize the PING sensor for use by the
*				coder.
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
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

/**
*       This method will be used to convert the value from the PING sensor into a value more
*				recongizeable to the coder, centimeters.
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*				@param clockCycles This is the value that the PING sensor will give back
*
*				@return The centimeter representation of the Clock Cycles reading
*
*       @date 4/24/2018
*
*/
int ClkCyclesToCM(int clockCycles){ //distance away from object
	return 0.5 * (clockCycles /16000000.0) * 34000.0;
}
/**
*       This method will start up the ping sensor and will send out a "sonar" that
*				it will wait to "hear" back on to do the measurement
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
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

/**
*       This method will perform a full measurement with the PING sensor
*				Then it will record the number of catch the edges to give the number of
*				Clock Cycles back.
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*				@return The clock cycles that were recorded off the PING sensor measurement
*
*       @date 4/24/2018
*
*/
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
