/**
*       @file sonar.h
*       @brief this is the header file which will contain all required definitions
*       and basic functions that the sonar class will use
*
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
#ifndef SONAR_H_
#define SONAR_H_

void ping_init();

int ClkCyclesToCM(int clockCycles);

void send_pulse();

unsigned ping_read();

#endif /* SONAR_H_ */
