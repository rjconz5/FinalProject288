/**
*         @file main.c
*         @breif This class is the heart and soul of the project
*          in charge routing commands and performing scans.
*
*         @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*         @date 4/24/2018
*
*/
#include "timer.h"
#include "lcd.h"
#include "math.h"
#include "button.h"
#include "ir.h"
#include "sonar.h"
#include "servo.h"
#include "uart.h"
#include "movement.h"
#include <stdlib.h>


/**
*      This is our main method it will be used to take in the input of the user
*      and send the correct course of action to the robot based off the commands
*      for objects and tell us about the objects, and also moving the robot.
*
*      @author Ryan Connolly, Ian Rostkowski, Jacob Moody, and Jacob Brown
*
*      @date 4/24/2018
*
*/

void main()
{
  //intialize all devices and variables
    lcd_init();
    Timer1_init();
    ir_init();
    ping_init();
    uart_init();

    int irDistance[91];
    int sonarDistance[91];
    //either 0 or 1
    int objectIR[91];
    int objectSonar[91];
    //Max 8 objects
    int objectStartDegree[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    int objectEndDegree[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    double objectWidthCM[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

    char input = '\0';
    extern char isr_char_buffer[];
    extern int SENSOR_TRIP;
    extern int BUG;
    int togo;
    int wantToScan = 0;
    int dist;
//start the program, and don't stop it till told otherwise
    while (1)
    {
      //setup the sensors and the robot
        oi_t* sensor = setup();
        oi_setWheels(0, 0);
      //check to see if a scan was requested
        if (wantToScan == 1)
        {
           //move servo to 0 Degrees
            servo_0D();
            //wait for move to finsih
            timer_waitMillis(500);
            //intialize some values that will be used to record measurements
            int degrees = 0;
            char s[100];
            //record measurements every 2 degrees starting from 0 going to 180
            for (degrees = 0; degrees <= 180; degrees += 2)
            {
                //Read from IR
                //initiate SSI conversion
                ADC0_PSSI_R = ADC_PSSI_SS0;
                //wait for ADC conversion to be complete
                while ((ADC0_RIS_R & ADC_RIS_INR0) == 0)
                {
                    //wait
                }
                //End Read from IR
                //Allow IR Data read to finish
                timer_waitMillis(100);
                //then convert the IR to CM
                int irDist = IRVoltstoCM(ADC0_SSFIFO0_R);
                //then get the sonar value
                unsigned int sonarDist = ClkCyclesToCM(ping_read());
                //store each value in a array at the appropriate spot
                irDistance[degrees / 2] = irDist;
                sonarDistance[degrees / 2] = (int) sonarDist;
                //check to see if it was over an object or notes
                //first check that it within the valid range of a sonar
                if (sonarDist < 100)
                {
                  //then say that it was an object to the sonar
                    objectSonar[degrees / 2] = 1;
                }
                else
                {
                  //otherwise it was not an object to the sonar
                    objectSonar[degrees / 2] = 0;
                }
                //check if it was in the valid IR distance
                if ((irDist>15) && (irDist < 80))
                {
                  //if it was then it is an object to the IR
                    objectIR[degrees / 2] = 1;
                }
                else
                {
                  //otherwise it is not an object to the IR
                    objectIR[degrees / 2] = 0;
                }
                //print on LCD so it is easy to read from bot
                lcd_printf("Degrees, IR, sonar, objectIR\n%d, %d, %d, %d",
                            degrees, irDist, sonarDist, objectIR[degrees / 2]);
                //then move 2 degrees to the left
                move_servo(2, 0);
                timer_waitMillis(50);
            }

//Check if the data is accurate
            //if the edge of the sweep picked up a sweep but the one next to it did not then it is forgotten
            if (objectIR[0] == 1 && objectIR[1] == 0)
            {
                objectIR[0] = 0;
            }
            if (objectSonar[0] == 1 && objectSonar[1] == 0)
            {
                objectSonar[0] = 0;
            }
            if (objectIR[89] == 0 && objectIR[90] == 1)
            {
                objectIR[90] = 0;
            }
            if (objectSonar[89] == 0 && objectSonar[90] == 1)
            {
                objectSonar[90] = 0;
            }
            //gets rid of incorrect object identification
            for (degrees = 2; degrees <= 178; degrees += 2)
            {
              //if there is a tiny object only picked up at one two degree then it is more than likely not an object
                if (objectIR[(degrees - 2) / 2] == 0
                        && objectIR[degrees / 2] == 1
                        && objectIR[(degrees + 2) / 2] == 0)
                {
                    objectIR[degrees / 2] = 0;
                }
                if (objectSonar[(degrees - 2) / 2] == 0
                        && objectSonar[degrees / 2] == 1
                        && objectSonar[(degrees + 2) / 2] == 0)
                {
                    objectSonar[degrees / 2] = 0;
                }
                //if there is a object that was not picked up at one of the two degree marks it can probably be overlooked
                if (objectIR[(degrees - 2) / 2] == 1
                        && objectIR[degrees / 2] == 0
                        && objectIR[(degrees + 2) / 2] == 1)
                {
                    objectIR[degrees / 2] = 1;
                }
                if (objectSonar[(degrees - 2) / 2] == 1
                        && objectSonar[degrees / 2] == 0
                        && objectSonar[(degrees + 2) / 2] == 1)
                {
                    objectSonar[degrees / 2] = 1;
                }
            }
//cleans up the object array
            for (degrees = 0; degrees <= 180; degrees += 2)
            {
                // if both the sonar and IR detected an object, its an object
                if (objectIR[degrees / 2] == 1 && objectSonar[degrees / 2] == 1)
                {
                    objectIR[degrees / 2] = 1;
                }
                else
                {
                  //otherwise it is not
                    objectIR[degrees / 2] = 0;
                }
            }
            //intialize values for counting objects
            int object = 0;
            degrees = 0;
            while (objectIR[degrees / 2] == 1)
            { //look for the first object detection
                degrees += 2;
            }
            //dont count 0 degree so we can have start degree for reference
            if (degrees == 0)
            {
                degrees += 2;
            }
            //once you find it look for its edges
            for (; degrees <= 180; degrees += 2)
            {

                if (objectIR[(degrees - 2) / 2] == 0
                        && objectIR[(degrees / 2)] == 1)
                { //starting edge of the object was detected
                    objectStartDegree[object] = degrees - 1; //average degree of the edge measurements
                }
                else if (objectIR[(degrees - 2) / 2] == 1
                        && objectIR[(degrees / 2)] == 0)
                { //ending edge of the object was detected
                    objectEndDegree[object] = degrees - 1;
                    object++; //so we can add an object
                }

                if (object > 7)
                { //stop if we get 8 objects
                    break;
                }
            }
            if (object <= 7)
            { //get rid of the Start angle if there was no end angle
                if (objectEndDegree[object] == -1)
                {
                    objectStartDegree[object] = -1;
                }
            }

            int objectsNum = 0;
            for (object = 0; object < 8; object++)
            {
              //count strictly objects that have an end degree
                if (objectEndDegree[object] != -1)
                {
                    objectsNum++;
                }
            }
            //print the num objects on bot so it is easy to read
            lcd_printf("Objects %d\n", objectsNum);
            timer_waitMillis(1000); // wait one second to display num objects detected on screen

            //calculate width
            for (object = 0; object < 8; object++)
            {
              //gets rid of all invalid objects
                if (objectStartDegree[object] != -1)
                {
                  //records the angle at the center of the object
                    int sonarIndex = (int) (((((objectEndDegree[object]
                            - objectStartDegree[object]) / 2.0)
                            + objectStartDegree[object])) / 2.0);
                    //finds the distance at that angle
                    double distanceObject = ((double) sonarDistance[sonarIndex]);
                    //finds the hypotenuse degree of this iscoles triangle
                    int hypotInd = (int) ((objectStartDegree[object] + 1.0)
                            / 2.0);
                    //then grabs the sonar value at that angle
                    int hypote = ((int) sonarDistance[hypotInd]);
                    //then uses the angle and the hypotonuse of this iscoles triangle to calculate linear width
                    objectWidthCM[object] = 2.0
                            * (hypote
                                    * cos(((((objectEndDegree[object]
                                            - objectStartDegree[object]) / 2.0)
                                            - 90.0) * 3.14159) / 180.0));
                    //tells putty if it found objects what the width, distance, and angle of the object was.
                    sprintf(s,
                            "object %lf %d %d\n",
                            (objectWidthCM[object]),
                            (int) distanceObject,
                            (int) ((objectEndDegree[object]
                                    - objectStartDegree[object]) / 2.0)
                                    + objectStartDegree[object]);
                    uart_sendStr(s);
                }
            }
            //finds the smallest object using calculated width
            int smallestObjectIndex = -1;
            if (objectsNum > 1)
            {
                smallestObjectIndex = 0;
                for (object = 1; object < 8; object++)
                {
                    if (objectStartDegree[object] != -1)
                    {
                        if ((objectWidthCM[smallestObjectIndex])
                                > (objectWidthCM[object]))
                        {
                            smallestObjectIndex = object;
                        }
                    }
                }
            }
            else if (objectsNum == 1)
            {
                smallestObjectIndex = 0;
                while (objectStartDegree[smallestObjectIndex] == -1)
                    smallestObjectIndex++;
            }

            if (smallestObjectIndex == -1)
            {
                sprintf(s, "No Objects!  \n\r");
                //uart_sendStr(s);
                lcd_printf("No Objects to point to ");
            }
            else
            {
                int sonarIndex = (int) (((((objectEndDegree[object]
                        - objectStartDegree[object]) / 2.0)
                        + objectStartDegree[object]) / 2.0) + 0.5);
                double distanceObject = ((double) sonarDistance[sonarIndex]);
                //prints all the objects on the LCD very briefly
                lcd_printf(
                        "Object: %d\nWidth: %d cm \nDist: %d\nDeg: %d\n Ang: %d\n",
                        smallestObjectIndex + 1,
                        (int) (objectWidthCM[smallestObjectIndex] / 2.0),
                        (int) distanceObject,
                        (int) (((objectEndDegree[smallestObjectIndex]
                                - objectStartDegree[smallestObjectIndex]) / 2.0)
                                + objectStartDegree[smallestObjectIndex]),
                        (int) (objectEndDegree[smallestObjectIndex]
                                - objectStartDegree[smallestObjectIndex]));
                servo_0D();
                //moves servo to smallest object
                move_servo(
                        (((objectEndDegree[smallestObjectIndex]
                                - objectStartDegree[smallestObjectIndex]) / 2.0)
                                + objectStartDegree[smallestObjectIndex]),
                        0);
            }
            //turns off the scan flag variable since it has completed
            wantToScan = 0;
            //reintializies the object arrays for a new scan
            for (object = 0; object < 8; object++)
            {
                objectStartDegree[object] = -1;
                objectEndDegree[object] = -1;
                objectWidthCM[object] = -1; //Max 8 objects
            }
        }
        UART1_Clear();
        //this marks that the scan has been completed
        uart_sendStr("done\n");
        //sets up values to take in the user input3
        char strIn[2];
        input = isr_char_buffer[0];
        strIn[0] = isr_char_buffer[1];
        strIn[1] = isr_char_buffer[2];
        //stores the user input
        while (strIn[1] == '\0')
        {
            input = isr_char_buffer[0];
            strIn[0] = isr_char_buffer[1];
            strIn[1] = isr_char_buffer[2];
        }
        //initialize some values for movement
        int distance;
        int modifier;
        //turn the char dist into an int dist
        distance = atoi(strIn);

        //converts the cm command to mm for movement
        togo = distance * 10;
        int needToMove;
        //checks the input
        switch (input)
        {
        case 'w':
        //this will set it up to move forward
            needToMove = 1;
            modifier = 1;
            break;
        case 's':
        //this will set it up to move backward
            needToMove = 1;
            modifier = -1;
            break;
        case 'a':
        //this will set it up to turn right by the angle the user passed
            needToMove = 0;
            turn_counterClock(sensor, distance);
            break;
        case 'd':
        //this will set it up to turn right by the angle the user passed
            needToMove = 0;
            turn_clockwise(sensor, distance);
            break;
        case 't':
        //this will set it up to scan
            needToMove = 0;
            wantToScan = 1;
            break;
        case 'p':
        //this will set it up to mark the completion of the course, so it will play music and flash its light
            needToMove = 0;
            wantToScan = 0;
            int l;
            for (l = 0; l < 30; l++)
            {
                oi_setLeds(1, 1, 255, 255);
                timer_waitMillis(100);
                oi_setLeds(1, 1, 0, 255);
                timer_waitMillis(100);
            }
            unsigned char notes1[] = { 76, 75, 76, 75, 76, 71, 74, 72, 69, 60,
                                       64, 69, 71, 64, 68, 71 };
            unsigned char noteLen[] = { 16, 16, 16, 16, 16, 16, 16, 16, 48, 16,
                                        16, 16, 48, 16, 16, 16 };
            oi_loadSong(1, 16, notes1, noteLen);
            oi_play_song(1);
            oi_setLeds(1, 1, 255, 127);
            timer_waitMillis(5000);
            unsigned char notes2[] = { 72, 64, 76, 75, 76, 75, 76, 71, 74, 72,
                                       69, 60, 64, 69, 71, 64 };
            unsigned char noteLen1[] = { 48, 16, 16, 16, 16, 16, 16, 16, 16, 16,
                                         48, 16, 16, 16, 48, 16 };
            oi_loadSong(1, 16, notes2, noteLen1);
            oi_play_song(1);
            timer_waitMillis(5000);
            unsigned char notes3[] = { 72, 71, 69, 1, 76, 75, 69, 1, 71, 72, 74,
                                       76, 67, 77, 76, 74 };
            unsigned char noteLen2[] = { 16, 16, 32, 32, 16, 16, 32, 16, 16, 16,
                                         16, 48, 16, 16, 16, 48 };
            oi_loadSong(1, 16, notes3, noteLen2);
            oi_play_song(1);
            timer_waitMillis(5000);
            unsigned char notes4[] = { 65, 76, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                       1, 1, 1 };
            unsigned char noteLen3[] = { 16, 16, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                         1, 1, 1, 1 };
            oi_loadSong(1, 16, notes4, noteLen3);
            oi_play_song(1);

        }
        //moves the robot forward or backward based off the user command
        while (togo > 0 && needToMove == 1)
        {
          //checks for forward movement
            if(modifier == 1){
                dist = abs(move_forward(sensor, togo));
            }
            //checks for backward movement
            else if(modifier ==-1){
                dist = abs(moveBackward(sensor, togo));
            }
            //checks the sensors
            switch (SENSOR_TRIP)
            {
            case 1:
            //this will happen if right bumper was hit
                uart_sendStr("HIT OBJECT RIGHT\n");
                togo = 0;
                SENSOR_TRIP = 0;
                break;
            case 2:
            //this will trip if left bumper was hit
                uart_sendStr("HIT OBJECT LEFT\n");
                SENSOR_TRIP = 0;
                togo = 0;
                break;
            case 3:
            //this will trip if a boundary was hit on left side
                togo = 0;
                uart_sendStr("HIT WALL LEFT\n");
                SENSOR_TRIP = 0;
                break;
            case 4:
            //this will trip if a boundary was hit on right side
                togo = 0;
                uart_sendStr("HIT WALL RIGHT\n");
                SENSOR_TRIP = 0;
                break;

            case 5:
            //this will trip if a hole was hit on right side
                togo = 0;
                uart_sendStr("HIT HOLE RIGHT\n");
                SENSOR_TRIP = 0;
                break;
            case 6:
            //this will trip if a hole was hit on right side
                togo = 0;
                uart_sendStr("HIT HOLE LEFT\n");
                SENSOR_TRIP = 0;
                break;

            default:
            //else it keeps moving till its done
                togo -= dist;
            }
            //clean sensors and set distance to 0
            cleanup(sensor);
            togo = 0;
        }
    }
}
