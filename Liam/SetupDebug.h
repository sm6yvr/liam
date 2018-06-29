/*
 Liam - DIY Robot Lawn Mower

 Debug Mode Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "Controller.h"

#ifndef _SETUPDEBUG_H_
#define _SETUPDEBUG_H_

class SETUPDEBUG {
  public:
    SETUPDEBUG(CONTROLLER* mower, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt);
    int tryEnterSetupDebugMode(int currentState);
    void printHelpHelp();
  private:
    CONTROLLER* mower;
    WHEELMOTOR* leftMotor;
    WHEELMOTOR* rightMotor;
    CUTTERMOTOR* cutter;
    BWFSENSOR* sensor;
    MOTIONSENSOR* compass;
    BATTERY* battery;

    void printHelp();
    void toggleLed();
    void RampUpMotor(WHEELMOTOR* motor);
    void RampDownMotor(WHEELMOTOR* motor);
    void toggleWheelLeft();
    void togglewheelRight();
    void getBwfSignals();
    void toggleCutterMotor();
    void testRun();
    void cutterSpeedUp();
    void cutterSpeedDown();
    void printStatus();
    void SETUPDEBUG::turnRight();
    void getMotionValues();
    void trimpotAdjust();

    boolean led_is_on;

    boolean cutter_motor_is_on;
    boolean left_wheel_motor_is_on;
    boolean right_wheel_motor_is_on;
    int cutterspeed;
};

#endif /* _SETUPDEBUG_H_ */
