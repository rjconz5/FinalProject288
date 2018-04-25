/**
*       @file servo.c
*       @brief this is the header file which contatins all functions
*				that we deemed important to have while operating the servo on
*				on this robot
*
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/

#include "timer.h"

#define ZEROD 0xC2C0
#define ONEEIGHTYD 0x5560

/**
*       This method will be to move the robot forwards by the given distance
*       checking the cliff and bumper sensors along the way
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param sensor The sensor to check to see how far the robot has moved forwards
*
*       @param centimeters The distance we would like the robot to travel
*
*       @return the distance the robot actually traveled
*
*       @date 4/24/2018
*
*/
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

/**
*       This method will be used to move the robot's servo in designated direction by the designated number of degrees
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param degrees The number of degees you would like it to move in that direction
*
*       @param direction The direction you would like the servo to move, 0 = left, 1=right
*
*       @date 4/24/2018
*
*/
void move_servo(double degrees, int direction){
	//checks if it is right and moves the number of degrees
	if(direction == 1  && (TIMER1_TBMATCHR_R + (degrees * 155.5555)) <= ZEROD){
		TIMER1_TBMATCHR_R += (degrees * 155.5555);
	//otherwise it checks that it is left and moves the number of the degrees
	}else if(direction == 0 && (TIMER1_TBMATCHR_R - (degrees * 155.5555)) >= ONEEIGHTYD){
		TIMER1_TBMATCHR_R -= (degrees * 155.5555);
	}
}

/**
*       This method will be used to move the robot's sonar to the 0 degree mark
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
void servo_0D(){
	TIMER1_TBMATCHR_R = ZEROD;
}

/**
*       This method will be used to move the robot's sonar to the 90 degree mark
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
void servo_90D(){
	TIMER1_TBMATCHR_R = (ONEEIGHTYD + ZEROD) / 2;
}

/**
*       This method will be used to move the robot's sonar to the 180 degree mark
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
void servo_180D(){
	TIMER1_TBMATCHR_R = ONEEIGHTYD;
}
