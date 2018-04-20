#include "open_interface.h"


int move_forward(oi_t* sensor, int centimeters);
int moveBackward(oi_t* sensor, int centimeters);

void turn_clockwise(oi_t *sensor, int degrees);
void turn_counterClock(oi_t *sensor, int degrees);
oi_t* setup();

void cleanup(oi_t* sensor);
int absVal(int num);
