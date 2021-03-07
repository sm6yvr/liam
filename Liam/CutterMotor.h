/*
 Liam - DIY Robot Lawn Mower

 Cutter Motor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#ifndef _CUTTERMOTOR_H_
#define _CUTTERMOTOR_H_

#include <Servo.h>
#include <Arduino.h>

class CUTTERMOTOR {
  public:
    CUTTERMOTOR(int type_, int pwmpin_, int loadpin_);

    void initialize();
    int setSpeedOverTime(int setspeed, int time);
    void setSpeed(int setspeed);
    int getSpeed();

    int getLoad();

    bool isBrushless();
    bool isOverloaded();
    void setOverloadLevel(int level);

  private:
    int pwmpin;
    int pwm;
    int type;
    int loadpin;
    int speed;
    int overload_level;
    Servo cutter;


    int ot_setTime = 0;
    int ot_currentTargetValue = 0;
    int ot_startingValue = 0;
    int ot_currentValue = 0;
};

#endif /* _CUTTERMOTOR_H_ */
