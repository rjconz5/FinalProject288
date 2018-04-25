/**
*       @file movement.h
*       @brief this is the header file which will contain all required definitions
*       and basic functions that the movement class will use
*
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @date 4/24/2018
*
*/
#include "open_interface.h"


int move_forward(oi_t* sensor, int centimeters);
int moveBackward(oi_t* sensor, int centimeters);

void turn_clockwise(oi_t *sensor, int degrees);
void turn_counterClock(oi_t *sensor, int degrees);
oi_t* setup();

void cleanup(oi_t* sensor);
int absVal(int num);
