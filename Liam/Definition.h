/*
  Definition.h is part of the project: liam - DIY Robot Lawn mover

  Description: This file holds all configuration parameters, user specific,
               common and software specific parameters.

  2017-06-06 Roberth Andersson:
             - Restructured the file and made differens sections of parameters.
             - Added setup debug mode.
  2014-12-12 Jonas Forsell:
            - Initial version.

  Copyright (c) 2014-2017 Jonas Forsell (and team)

  liam - DIY Robot LAwn mower is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "Battery.h"

#ifndef _DEFINITION_H_
#define _DEFINITION_H_

/* SETUP AND DEBUG MODE .
   First time you start your mover the code is configured to run in debug mode.
   In that mode you can send different commands to the mover to test different functions.
   You can also see some values reported by the sensors. You need probably to tweek some
   of the parameters in this file and when you are done remove or comment out this line to
   run your mover in real mode. */
#define __SETUP_AND_DEBUG_MODE__ false

/******************************************************************
  User specific settings depends on how you have built your mover.
*******************************************************************/


/* Configure which Cutter motor you have.
   Types available:
                    BRUSHLESS  0     (for all hobbyking motors with external ESC)
                    BRUSHED:   1     (for all brushed motors, A3620 and NIDEC 22)
                    NIDEC      2     (for NIDEC 24 or NIDEC 22 connected to morgan shield without any modifications) */
const int MY_CUTTERMOTOR = 2;

/* Configure which type of battery you have.
   Types availible:
                    LEADACID  0
                    NIMH      1
                    LIION	    2
*/
const int MY_BATTERY = 2;

/* Number of BWF sensors can be 1-4 depending on shield */
const int NUMBER_OF_SENSORS = 2;

/* BWF transmitter signals */
#define INSIDE_BWF 86
#define OUTSIDE_BWF 6

/* If you have a bumper connected to pin8, set it to true. Remember to cut the brake connection on your motor shield */
#define __Bumper__ false

/* If you have a lift sensor connection to front wheel (connected to pin9), set it to true */
#define __Lift_Sensor__ false

/* Do you have a Sensor? If so, set one of these lines to true. */
#define __MS5883L__ false
#define __MS9150__ false
#define __ADXL345__ false
#define __MMA7455__ false

/* Tiltangle */
#define TILTANGLE 45

/* Flipangle (turn off cutter and stop everything) */
#define FLIPANGLE 75

/* Do you have a Display? If so, set one of these lines to true. */
#define __OLED__ false
#define __LCD__ true

/* Do you have a clock? If so, set it to true. */
#define __RTC_CLOCK__ true
#define GO_OUT_TIME 01, 00
#define GO_HOME_TIME 23, 00

/* Motor Speeds */
#define FULLSPEED 100
#define SLOWSPEED 30
#define CUTTERSPEED 100

/* Settings for ADXL345 and MMA_7455, what angle values the sensor reports when the mover is standing flat.
  IMPORTANT! You must calibrate those values for your setup.
  See the wiki:https://github.com/sm6yvr/liam/wiki/12.-Gyro-Accelerometer */
  /* Try to get inside for max x seconds, then stop and error. */

  /*IMPORTANT II !!
   * If you are using MMA_7455 you will get these values automatic from running setup-debug and type CAPITAL G, you still have to
  come back here and type them in though*/

#define Z_ANGLE_NORMAL 276 //false
#define Y_ANGLE_NORMAL 30 // false
#define X_ANGLE_NORMAL -20 // false

/* Enable this if you need the mower to go backward until it's inside and then turn.
   Default behavior is to turn directly when mower is outside BWF, if definition below is enabled this might help mower not to get stuck in slopes.
   If mower is not inside within x seconds mower will stop. */
#define GO_BACKWARD_UNTIL_INSIDE true
#define MAX_GO_BACKWARD_TIME 5
// The amount of times to check and sum angle, sum will be divided by this value to get avrage angle of slopereadings number of time read.
#define SLOPEREADINGS 1
/* Slopeangle. Mower will not go backwards if slope is less then slopeangle.
if you have no angle-sensor and still want mower to go backwards until it's inside, then set SLOPEANGLE to -1
*/
#define SLOPEANGLE -1

/******************************************************************
  Common settings that should be same for the most of us.
*******************************************************************/
/* Pin setup following Morgan 1.5 shield and up */
/* Analog pins */
#define WHEEL_MOTOR_A_CURRENT_PIN 0
#define WHEEL_MOTOR_B_CURRENT_PIN 1
#define SOC_PIN 2
#define CUTTER_CURRENT_PIN 3
#define I2C_SDA_PIN 4
#define I2C_SDL_PIN 5
/* Digital pins */
#define RX_PIN 0
#define TX_PIN 1
#define BWF_SENSOR_INPUT_PIN 2
#define WHEEL_MOTOR_A_PWM_PIN 3
#define BWF_SELECT_A_PIN 4
#define DOCK_PIN 5
#define CUTTER_PWM_PIN 6
#define BWF_SELECT_B_PIN 7
#define BUMPER 8
#define LIFT_SENSOR_PIN 9
#define LED_PIN 10
#define WHEEL_MOTOR_B_PWM_PIN 11
#define WHEEL_MOTOR_A_DIRECTION_PIN 12
#define WHEEL_MOTOR_B_DIRECTION_PIN 13

/******************************************************************
  Program settings that shold not be changed.
*******************************************************************/

/* Cutter motor types */
#define BRUSHLESS 0
#define BRUSHED 1
#define NIDEC 2

/* Battery */
#define LEADACID 0
#define NIMH 1
#define LIION	2

/* Wheel motor */
#define WHEELMOTOR_OVERLOAD		130
#define WHEELMOTOR_SMOOTHNESS	300

/* If the mower has repeated overload within the interval below (in milliseconds),
   it will flag as having reached a bump. It will then do some action as stated
   in the Controller.cpp file. */
#define OVERLOAD_INTERVAL 3000

/* CUTTER */
#define CUTTER_OVERLOAD 100

/* Cutter states */
const int MOWING = 0;
const int LAUNCHING = 1;
const int DOCKING = 2;
const int CHARGING = 3;

/* Turning details */
#define TURN_INTERVAL 15000
#define REVERSE_DELAY 2
#define TURNDELAY 20 //Reduce for smaller turning angle

/* BWF Detection method (true = always, false = only at wire) */
#define BWF_DETECTION_ALWAYS true
/* Time without signal before error */
#define TIMEOUT 6000

/* Trigger value for the mower to leave the BWF when going home- The higher value
   the more turns (in the same direction) the mower can make before leaving */
#define BALANCE_TRIGGER_LEVEL 10000

/* Software version */
#define MAJOR_VERSION 5
#define MINOR_VERSION_1	2
#define MINOR_VERSION_2	0

/* Dicks tillägg */
#define OUTSIDE_READINGS 2

class DEFINITION {
    public:
        void definePinsInputOutput();
        void setDefaultLevels(BATTERY* battery, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cutter);
    private:
};

#endif /* _DEFINITION_H_ */
