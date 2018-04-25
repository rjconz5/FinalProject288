/**
*         @file movement.c
*         @breif This class is what is used to control the movement of
*         the robot it will allow us to move it forawrd or backward and rotate
*        clockwise or counter-clockwise
*
*         @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*         @date 4/24/2018
*
*/
#include "movement.h"
#include "timer.h"
#include <stdlib.h>

volatile int SENSOR_TRIP = 0;
/**
*       This method will be cleanup a sensor for reuse or to be put away
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param sensor This is the sensor that we would like to cleanup for
*       reuse or put away
*
*       @date 4/24/2018
*
*/
void cleanup(oi_t* sensor)
{
  //stops wheels
    oi_setWheels(0, 0);
    //clears sensor
    oi_free(sensor);
}
/**
*       This method will be used to setup and initialize a sensor for use by the
*				coder.
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @return The sensor that is all setup and ready to be used.
*
*       @date 4/24/2018
*
*/
oi_t* setup()
{
  //allocates memory for sensor
    oi_t *sensor_data = oi_alloc();
    //initializes sensor and returns it
    oi_init(sensor_data);
    return sensor_data;
}
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
int move_forward(oi_t* sensor, int centimeters)
{
  //initalizes variables for use
    int sum = 0;
    centimeters = centimeters;
    //runs commands until destination distance has been met
    while (sum < centimeters)
    {
      //turns on wheels
        oi_setWheels(150, 150);
        //updates sensor
        oi_update(sensor);
        //throws that into the sum
        sum += sensor->distance;
        timer_waitMillis(1);
        //checks the sensors
        if (SENSOR_TRIP == 0)
        {
          //will trigger if bumper sensor trigger was hit backs it up 15 cm then stops it
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
            //will trigger if cliff sensor detects a boundary was hit backs it up 15 cm then stops it
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
            //will trigger if cliff sensor detects a hole was hit backs it up 15 cm then stops it
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
    //stops wheels, clears sensor, and returns distance traveled
    oi_setWheels(0, 0);
    oi_free(sensor);
    return sum;
}
/**
*       This method will be used to move the robot backwards by the given distnace
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param sensor The sensor to check to see how far the robot has moved backwards
*
*       @param centimeters The distance we would like the robot to travel
*
*       @return the distance the robot actually traveled
*
*       @date 4/24/2018
*
*/
int moveBackward(oi_t* sensor, int centimeters){
    timer_waitMillis(10);

    int sum = 0 ;
    //moves backward until at destination
    while (sum < centimeters)
    {
      //starts movement and updates sensor
        oi_setWheels(-150, -150); //move backward; half speed
        oi_update(sensor);
      //throws distance into sum
        sum -= sensor->distance;
        timer_waitMillis(1);

    }

    oi_setWheels(0,0); //stop wheels
    oi_free(sensor); //clears sensor
    return sum; //reurns distance travled
}
/**
*       This method will be used to turn the robot clockwise by the number of degrees given
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param sensor The sensor to check to see how far it has turned
*
*       @param degrees The number of degrees it would like the robot to turn
*
*       @date 4/24/2018
*
*/
void turn_clockwise(oi_t* sensor, int degrees)
{
    //sets it to turn clockwise
    oi_setWheels(-200, 200);
    //waits for degree rotation to complete
    timer_waitMillis(degrees * 11);
    oi_setWheels(0, 0); //stop wheels
    oi_free(sensor);//clears sensor
}
/**
*       This method will be used to turn the robot counter clockwise the number of degrees given
*
*       @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*       @param sensor The sensor to check to see how far it has turned
*
*       @param degrees The number of degrees it would like the robot to turn
*
*       @date 4/24/2018
*
*/
void turn_counterClock(oi_t* sensor, int degrees)
{
    //starts wheels to turn counter clockwise
    oi_setWheels(200, -200);
    //waits until it has reached the degree we are looking for
    timer_waitMillis(degrees * 11);
    oi_setWheels(0, 0); //stop wheels
    oi_free(sensor);//clears sensor
}
