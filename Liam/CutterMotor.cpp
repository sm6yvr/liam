/*
 Liam - DIY Robot Lawn Mower

 Cutter Motor Library

 ======================
  Licensed under GPLv3
 ======================
*/
/*
  The cutter motor can be of type BRUSHLESS, BRUSHED or
  NIDEC

  BRUSHLESS is assumed to be connected via an ESC.
  BRUSHED and NIDEC are controlled via PWM, but the signal
  to NIDEC is inverted.

  Speed can be set between 0 and 100.
*/

#include "CutterMotor.h"
#include "Definition.h"

CUTTERMOTOR::CUTTERMOTOR(int type_, int pwmpin_, int loadpin_) {
  type = type_;
  pwmpin = pwmpin_;
  loadpin = loadpin_;
}

void CUTTERMOTOR::initialize() {
  // Initialize if brushless with ESC
  if (type == BRUSHLESS) {
    cutter.attach(pwmpin);
    cutter.writeMicroseconds(2000);
    delay(400);
    cutter.writeMicroseconds(1000);
    delay(2000);
  }
  setSpeed(0);
}


void CUTTERMOTOR::setSpeed(int setspeed) {
  speed = setspeed;
  if (speed > 100) speed = 100;
  if (speed < 0) speed = 0;

  int pwm;
  if (type == BRUSHLESS)
    pwm = 1000 + 1000 * speed/100;
  else if (type == BRUSHED)
    pwm = 255 * speed/100;
  else if (type == NIDEC)
    pwm = 255 - 255 * speed/100;

  if (type == BRUSHLESS)
    cutter.writeMicroseconds(pwm);
  else
    analogWrite(pwmpin, pwm);
}


int CUTTERMOTOR::getSpeed() {
  return speed;
}

int CUTTERMOTOR::getLoad() {
  return analogRead(loadpin);
}

bool CUTTERMOTOR::isOverloaded() {
  return (getLoad() > overload_level);
}

void CUTTERMOTOR::setOverloadLevel(int level) {
  overload_level = level;
}
