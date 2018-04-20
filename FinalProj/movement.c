#include "movement.h"
#include "timer.h"
#include <stdlib.h>

volatile int SENSOR_TRIP = 0;
volatile int BUG = 0;

void cleanup(oi_t* sensor)
{
    oi_setWheels(0, 0);
    oi_free(sensor);
}
oi_t* setup()
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    return sensor_data;
}
int move_forward(oi_t* sensor, int centimeters)
{
    int sum = 0;
    centimeters = centimeters;
    while (sum < centimeters)
    {
        oi_setWheels(150, 150);
        oi_update(sensor);
        sum += sensor->distance;
        timer_waitMillis(1);
        if (SENSOR_TRIP == 0)
        {
            if (sensor->bumpRight)
            {
                SENSOR_TRIP = 1;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }
            else if (sensor->bumpLeft)
            {
                SENSOR_TRIP = 2;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }
            else if ((sensor->cliffLeftSignal > 2700)
                    || (sensor->cliffFrontLeftSignal > 2700))
            {
                SENSOR_TRIP = 3;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }

            else if ((sensor->cliffFrontRightSignal > 2700)
                    || (sensor->cliffRightSignal > 2700))
            {
                SENSOR_TRIP = 4;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }
            else if ((sensor->cliffLeftSignal < 50)
                    || (sensor->cliffFrontLeftSignal < 50))
            {
                SENSOR_TRIP = 6;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }

            else if ((sensor->cliffFrontRightSignal < 50)
                    || (sensor->cliffRightSignal < 50))
            {
                SENSOR_TRIP = 5;
                moveBackward(sensor, 150);
                oi_free(sensor);
                return sum;
            }
        }
    }
    oi_setWheels(0, 0);
    oi_free(sensor);
    return sum;
}
int moveBackward(oi_t* sensor, int centimeters){
    timer_waitMillis(10);

    int sum = 0 ;

    while (sum < centimeters)
    {
        oi_setWheels(-150, -150); //move backward; half speed
        oi_update(sensor);

        sum -= sensor->distance;
        timer_waitMillis(1);

    }

    oi_setWheels(0,0); //stop wheels
    oi_free(sensor);
    return sum;
}
void turn_clockwise(oi_t* sensor, int degrees)
{
//    int direction = degrees < 0 ? -1 : 1;
//    int count = 0;
//    BUG = 0;
////    cleanup(sensor);
//    if (direction == -1)
//        oi_setWheels(-200, 200);
//    else
//        oi_setWheels(200, -200);
//    int difference = 0;
//    while (difference < abs(degrees))
//    {
//        if ((direction == -1) && (count == 11))
//        {
//            BUG = 1;
//            break;
//        }
//        if ((direction == 1) && (count == 11))
//        {
//            BUG = 2;
//            break;
//        }
//        oi_update(sensor);
//        difference += abs(sensor->angle);
//        count++;
//    }
//    oi_setWheels(0, 0);
    oi_setWheels(-200, 200);
    timer_waitMillis(degrees * 11);
    oi_setWheels(0, 0); //stop wheels
    oi_free(sensor);
}
void turn_counterClock(oi_t* sensor, int degrees)
{
    oi_setWheels(200, -200);
    timer_waitMillis(degrees * 11);
    oi_setWheels(0, 0); //stop wheels
    oi_free(sensor);
}
int absVal(int num)
{
    return num < 0 ? num * -1 : num;
}

