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

//#include "driverlib/interrupt.h"
/*LAB8*/

void main()
{

    lcd_init();
    Timer1_init(); //servo initialized to 90 degrees (default)
    ir_init();
    ping_init(); //sonar
    uart_init();

    int irDistance[91];
    int sonarDistance[91];
    int objectIR[91]; //either 0 or 1
    int objectSonar[91]; //either 0 or 1

    int objectStartDegree[8] = { -1, -1, -1, -1, -1, -1, -1, -1 }; //Max 8 objects
    int objectEndDegree[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    double objectWidthCM[8] = { -1, -1, -1, -1, -1, -1, -1, -1 }; //Max 8 objects
    char input = '\0';
    extern char isr_char_buffer[];
    extern int SENSOR_TRIP;
    extern int BUG;
    int togo;
    int wantToScan = 0;
    int dist;

    //WiFi_start("password123");

    while (1)
    {
        oi_t* sensor = setup();
        oi_setWheels(0, 0);

        if (wantToScan == 1)
        {

//	    uart_receive();
            servo_0D(); //move servo to 0 Degrees
            timer_waitMillis(500);
            //uart_sendStr("\n\rDegrees, IRDist, sonarDist, objectIR \n\r\n\r");

            int degrees = 0;
            char s[100];
            for (degrees = 0; degrees <= 180; degrees += 2)
            {
//READIR
                //initiate SS1 conversion
                ADC0_PSSI_R = ADC_PSSI_SS0;
                //wait for ADC conversion to be complete
                while ((ADC0_RIS_R & ADC_RIS_INR0) == 0)
                {
                    //wait
                }
                //grab result
                timer_waitMillis(100);
                int irDist = IRVoltstoCM(ADC0_SSFIFO0_R);
//endREADIR
//READSONAR
                unsigned int sonarDist = ClkCyclesToCM(ping_read());
//endREADSONAR
                irDistance[degrees / 2] = irDist;
                sonarDistance[degrees / 2] = (int) sonarDist;

                //is this part of an object with 80 cm
//                if (sonarDist < 15)
//                { //don't trust IR at this distance
//                    if (sonarDist < 80)
//                    {
//                        objectSonar[degrees / 2] = 1; //yes this is part of an object
//                        objectIR[degrees / 2] = 1;
//                    }
//                    else
//                    {
//                        objectSonar[degrees / 2] = 0; // no this is part of an object
//                        objectIR[degrees / 2] = 0;
//                    }
//                }
//                else
//                { // normal case
//                    if (irDist < 80)
//                    {
//                        objectIR[degrees / 2] = 1; // yes this is part of an object
//                    }
//                    else
//                    {
//                        objectIR[degrees / 2] = 0; // no this is part of an object
//                    }
//                    if (sonarDist < 80)
//                    {
//                        objectSonar[degrees / 2] = 1; //yes this is part of an object
//                    }
//                    else
//                    {
//                        objectSonar[degrees / 2] = 0; // no this is part of an object
//                    }
//                }
                if (sonarDist < 100)
                {
                    objectSonar[degrees / 2] = 1;
                }
                else
                {
                    objectSonar[degrees / 2] = 0;
                }
                if ((irDist>15) && (irDist < 80))
                {
                    objectIR[degrees / 2] = 1;
                }
                else
                {
                    objectIR[degrees / 2] = 0;
                }

                lcd_printf("Degrees, IR, sonar, objectIR\n%d, %d, %d, %d",
                           degrees, irDist, sonarDist, objectIR[degrees / 2]);

                move_servo(2, 0); //move 2 degrees to the left
                timer_waitMillis(50);
            }

//DATA ANALYSIS

            //TRY TO GET CONSISTANT DATA
            //get consistant data (get rid of single jumping out objects)(then and them together into objectIR)
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
            for (degrees = 2; degrees <= 178; degrees += 2)
            { //hopefully fixes glitches of decting to many objects
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

            for (degrees = 0; degrees <= 180; degrees += 2)
            {
                if (objectIR[degrees / 2] == 1 && objectSonar[degrees / 2] == 1)
                {
                    objectIR[degrees / 2] = 1;
                }
                else
                {
                    objectIR[degrees / 2] = 0;
                }
            }
            //endTRY TO GET CONSISTANT DATA

            int object = 0;
            degrees = 0;
            while (objectIR[degrees / 2] == 1)
            { //don't count first object if we haven't had a distance value over 80 CM otherwise we won't have a start degree
                degrees += 2;
            }
            if (degrees == 0)
            {
                degrees += 2;
            }

            for (; degrees <= 180; degrees += 2)
            {

                if (objectIR[(degrees - 2) / 2] == 0
                        && objectIR[(degrees / 2)] == 1)
                { //start of object
                    objectStartDegree[object] = degrees - 1; //somewhere between first and last
                }
                else if (objectIR[(degrees - 2) / 2] == 1
                        && objectIR[(degrees / 2)] == 0)
                { //end of object
                    objectEndDegree[object] = degrees - 1;
                    object++;
                }

                if (object > 7)
                {
                    break;
                }
            }
            if (object <= 7)
            { //don't detect objects that are partcially out of range of detection
                if (objectEndDegree[object] == -1)
                {
                    objectStartDegree[object] = -1;
                }
            }

            int objectsNum = 0;
            for (object = 0; object < 8; object++)
            {
                if (objectEndDegree[object] != -1)
                {
                    objectsNum++;
                }
            }
            lcd_printf("Objects %d\n", objectsNum);
            timer_waitMillis(1000); // wait one second to display num objects detected on screen

            //calculate width
            for (object = 0; object < 8; object++)
            {
                if (objectStartDegree[object] != -1)
                {
                    int sonarIndex = (int) (((((objectEndDegree[object]
                            - objectStartDegree[object]) / 2.0)
                            + objectStartDegree[object])) / 2.0);

                    double distanceObject = ((double) sonarDistance[sonarIndex]);
                    int hypotInd = (int) ((objectStartDegree[object] + 1.0)
                            / 2.0);
                    int hypote = ((int) sonarDistance[hypotInd]);
                    //double distanceObject = ((double)irDistance[sonarIndex]+(double) sonarDistance[sonarIndex])/2;
//                            objectWidthCM[object] = 2.0
//                                    * (distanceObject
//                                            * tan((((objectEndDegree[object]
//                                                    - objectStartDegree[object]
//                                                    + 4) / 2.0) * 3.14159)
//                                                    / 180.0));
                    objectWidthCM[object] = 2.0
                            * (hypote
                                    * cos(((((objectEndDegree[object]
                                            - objectStartDegree[object]) / 2.0)
                                            - 90.0) * 3.14159) / 180.0));

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

            int smallestObjectIndex = -1;

            /*if(objectsNum > 1){ //find smallest using degrees
             smallestObjectIndex = 0;
             for(object = 1;  object < 8 && objectStartDegree[object] != -1 ; object++){
             if((objectEndDegree[smallestObjectIndex] - objectStartDegree[smallestObjectIndex]) > (objectEndDegree[object] - objectStartDegree[object])){
             smallestObjectIndex = object;
             }
             }
             }else if(objectsNum == 1){
             smallestObjectIndex = 0;
             }*/

            if (objectsNum > 1)
            { //find smallest using calculated width
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

                //sprintf(s, "Object: %d\n\rWidth: %d cm \n\rDist: %d\n\rDeg: %d\n\r", smallestObjectIndex + 1, ((int)((objectWidthCM[smallestObjectIndex]) / 2.0)), ((int)distanceObject),(int)(((objectEndDegree[smallestObjectIndex] - objectStartDegree[smallestObjectIndex])/2.0) + objectStartDegree[smallestObjectIndex]));
                sprintf(s,
                        "object %d %d %d\n",
                        ((int) ((objectWidthCM[smallestObjectIndex]) / 2.0)),
                        ((int) distanceObject),
                        (int) (((objectEndDegree[smallestObjectIndex]
                                - objectStartDegree[smallestObjectIndex]) / 2.0)
                                + objectStartDegree[smallestObjectIndex]));
//                uart_sendStr(s);
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
                move_servo(
                        (((objectEndDegree[smallestObjectIndex]
                                - objectStartDegree[smallestObjectIndex]) / 2.0)
                                + objectStartDegree[smallestObjectIndex]),
                        0);
            }
            wantToScan = 0;
            for (object = 0; object < 8; object++)
            {
                objectStartDegree[object] = -1;
                objectEndDegree[object] = -1;
                objectWidthCM[object] = -1; //Max 8 objects
            }
        }
        UART1_Clear();

        uart_sendStr("done\n");
        char strIn[2];
        input = isr_char_buffer[0];

        strIn[0] = isr_char_buffer[1];
        strIn[1] = isr_char_buffer[2];

        while (strIn[1] == '\0')
        {
            input = isr_char_buffer[0];
            strIn[0] = isr_char_buffer[1];
            strIn[1] = isr_char_buffer[2];
        }
        int distance;
        int modifier;
        distance = atoi(strIn);

        togo = distance * 10;
        int needToMove;
        switch (input)
        {
        case 'w':
            needToMove = 1;
            modifier = 1;
            break;
        case 's':
            needToMove = 1;
            modifier = -1;
            break;
        case 'a':
            needToMove = 0;
            turn_counterClock(sensor, distance);
//            if (BUG == 2)
//            {
//                uart_sendStr(
//                        "Robot tried to exceed 45, robot only turned 45\n");
//                cleanup(sensor);
//                sensor = setup();
//            }
            break;
        case 'd':
            needToMove = 0;
            turn_clockwise(sensor, distance);
//            if (BUG == 1)
//            {
//                uart_sendStr(
//                        "Robot tried to exceed 45, robot only turned 45\n");
//                cleanup(sensor);
//                sensor = setup();
//            }
            break;
        case 't':
            needToMove = 0;
            wantToScan = 1;
            break;
        case 'p':
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
        while (togo > 0 && needToMove == 1)
        {
            if(modifier == 1){
                dist = abs(move_forward(sensor, togo));
            }
            else if(modifier ==-1){
                dist = abs(moveBackward(sensor, togo));
            }
            switch (SENSOR_TRIP)
            {
            case 1:
                uart_sendStr("HIT OBJECT RIGHT\n");
                togo = 0;
                SENSOR_TRIP = 0;
                break;
            case 2:
                uart_sendStr("HIT OBJECT LEFT\n");
                SENSOR_TRIP = 0;
                togo = 0;
                break;
            case 3:
                togo = 0;
                uart_sendStr("HIT WALL LEFT\n");
                SENSOR_TRIP = 0;
                break;
            case 4:
                togo = 0;
                uart_sendStr("HIT WALL RIGHT\n");
                SENSOR_TRIP = 0;
                break;

            case 5:
                togo = 0;
                uart_sendStr("HIT HOLE RIGHT\n");
                SENSOR_TRIP = 0;
                break;
            case 6:
                togo = 0;
                uart_sendStr("HIT HOLE LEFT\n");
                SENSOR_TRIP = 0;
                break;

            default:
                togo -= dist;
            }
            cleanup(sensor);
            togo = 0;
        }
    }
}
