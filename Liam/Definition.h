// This is the definition for all the defaults
//

/* ============================================
Placed under the GNU license

===============================================
*/
#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "Battery.h"

#ifndef _DEFINITION_H_
#define _DEFINITION_H_

const int NUMBER_OF_SENSORS = 3;	// Number of BWF sensors can be 1-4 depending on shield

// Pin setup following Morgan 1.5 shield and up
#define WHEEL_MOTOR_A_CURRENT_PIN		0
#define WHEEL_MOTOR_B_CURRENT_PIN		1
#define SOC_PIN							2
#define CUTTER_CURRENT_PIN				3
#define I2C_SDA_PIN						4
#define I2C_SDL_PIN						5
// Digital pins
#define RX_PIN							0
#define TX_PIN							1
#define BWF_SENSOR_INPUT_PIN			2
#define WHEEL_MOTOR_A_PWM_PIN			3
#define BWF_SELECT_A_PIN				4
#define DOCK_PIN						5
#define CUTTER_PWM_PIN					6
#define BWF_SELECT_B_PIN				7
#define BUMPER							8
#define LIFT_SENSOR_PIN					9
#define LED_PIN							13
#define WHEEL_MOTOR_B_PWM_PIN			11
#define WHEEL_MOTOR_A_DIRECTION_PIN		12
#define WHEEL_MOTOR_B_DIRECTION_PIN		13

// Cutter motor types
#define BRUSHLESS						0
#define BRUSHED							1
#define NIDEC							2

// Battery
#define LEADACID	0
#define NIMH		1
#define LIION		2

// Wheel motor
#define WHEELMOTOR_OVERLOAD		130
#define WHEELMOTOR_SMOOTHNESS	300

// If the mower has repeated overload within the interval below (in milliseconds), it will flag as having reached a bump.
// It will then do some action as stated in the Controller.cpp file.
#define OVERLOAD_INTERVAL        3000

// CUTTER
#define CUTTER_OVERLOAD			100

// Cutter states
const int MOWING = 0;
const int LAUNCHING = 1;
const int DOCKING = 2;
const int CHARGING = 3;

// Turning details
#define TURN_INTERVAL					15000
#define REVERSE_DELAY					2
#define TURNDELAY						20 //Reduce for smaller turning angle

// BWF Detection method (true = always, false = only at wire)
#define BWF_DETECTION_ALWAYS			true
#define TIMEOUT							6000 //Time without signal before error

// Trigger value for the mower to leave the BWF when going home
// The higher value the more turns (in the same direction) the mower can make before leaving
#define BALANCE_TRIGGER_LEVEL			10000

// Code for inside and outside
//#define INSIDE_BWF						103,4,103
//#define OUTSIDE_BWF						103,107,103

// Version 2 of the BWF transmitter
#define INSIDE_BWF					5
#define OUTSIDE_BWF					86

#define MAJOR_VERSION				4
#define MINOR_VERSION_1				9
#define MINOR_VERSION_2				1

// If you have a bumper connected to pin8, uncomment the line below. Remember to cut the brake connection on your motor shield
//#define __Bumper__

// If you have a lift sensor connection to front wheel (connected to pin9), uncomment this line
//#define __Lift_Sensor__

// Do you have a Sensor? If so, uncomment one of these lines
//#define __MS5883L__
//#define __MS9150__
//#define __ADXL345__

// Do you have a Display? If so, uncomment one of these lines
//#define __OLED__
//#define __LCD__

// Do you have a clock? If so, uncomment the line below
//#define __RTC_CLOCK__
#define GO_OUT_TIME					16, 00
#define GO_HOME_TIME				22, 00

// Tiltangle
#define TILTANGLE						45

// Flipangle (turn off cutter and stop everything)
#define FLIPANGLE						75

// Motor Speeds
#define FULLSPEED						100
#define SLOWSPEED						30
#define CUTTERSPEED						100

/* Settings for ADXL345, what angle values the sensor reports when the mover is standing flat.
*  IMPORTANT! You must calibrate those values for your setup, see the wiki:https://github.com/sm6yvr/liam/wiki/12.-Gyro-Accelerometer */
#define Z_ANGLE_NORMAL 195
#define Y_ANGLE_NORMAL 180

// Enable this if you need the mower to go backward until it's inside and then turn.
// Default behavior is to turn directly when mower is outside BWF, if definition below is enabled this might help mower not to get stuck in slopes.
// If mower is not inside within x seconds mower will stop.
//#define GO_BACKWARD_UNTIL_INSIDE
//#define MAX_GO_BACKWARD_TIME  5 // try to get inside for max x seconds, then stop and error.

class DEFINITION {
    public:
        void definePinsInputOutput();
        void setDefaultLevels(BATTERY* battery, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cutter);

    private:
};

#endif /* _DEFINITION_H_ */
