/*
 * servo.h
 *
 *  Created on: Nov 10, 2016
 *      Author: gemullen
 */

#ifndef SERVO_H_
#define SERVO_H_

void Timer1_init(void);

void move_servo(double degrees, int direction);

void servo_0D();

void servo_90D();

void servo_180D();

#endif /* SERVO_H_ */
