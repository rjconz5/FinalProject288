#include "movement.h"
#include <stdlib.h>

void cleanup(oi_t* sensor){
    oi_setWheels(0,0);
    oi_free(sensor);
}
 oi_t* setup(){
     oi_t *sensor_data = oi_alloc();
     oi_init(sensor_data);
     return sensor_data;
 }
 int move_forward(oi_t* sensor, int centimeters){
     int sum = 0;
     int modifier = 1;
     modifier = centimeters < 0 ? -1 : 1;
     oi_setWheels(200 * modifier,200 * modifier);
     centimeters = centimeters * modifier;
     while(sum < centimeters){
         oi_update(sensor);
         if(sensor->bumpRight || sensor->bumpLeft){
             return sum;
         }
         sum += sensor->distance * modifier;
     }
     oi_setWheels(0,0);
     return sum;
 }
 void turn_clockwise(oi_t* sensor, int degrees){
     int direction = degrees < 0 ? -1:1;
     if(direction == -1)
         oi_setWheels(-200, 200);
     else
         oi_setWheels(200,-200);
     int difference = 0;
     while(difference < abs(degrees)){
         oi_update(sensor);
         difference += abs(sensor->angle);
     }
     oi_setWheels(0,0);
 }
 int absVal(int num){
     return num<0 ? num*-1:num;
 }

