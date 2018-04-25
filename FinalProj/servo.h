/**
*       @file servo.h
*       @brief this is the header file which will contain all required definitions
*       and basic functions that the sonar class will use
*
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/

#ifndef SERVO_H_
#define SERVO_H_

void Timer1_init(void);

void move_servo(double degrees, int direction);

void servo_0D();

void servo_90D();

void servo_180D();

#endif /* SERVO_H_ */
