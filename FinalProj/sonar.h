/*
 * sonar.h
 *
 *  Created on: Nov 3, 2016
 *      Author: gemullen
 */

#ifndef SONAR_H_
#define SONAR_H_

void ping_init();

int ClkCyclesToCM(int clockCycles);

void send_pulse();

unsigned ping_read();

#endif /* SONAR_H_ */
