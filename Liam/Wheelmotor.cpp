/*
 Liam - DIY Robot Lawn Mower

 Wheel Motor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Wheelmotor.h"


WHEELMOTOR::WHEELMOTOR(int pwmpin_, int dirpin_, int loadpin_, int smoothness) {
  pwmpin = pwmpin_;
  dirpin = dirpin_;
  loadpin = loadpin_;
  smoothness_delay = smoothness;
}




int WHEELMOTOR::setSpeedOverTime(int targetSpeed, int actionTime) {
		int _now = millis();
		if (targetSpeed != ot_currentTargetValue) {
			ot_currentTargetValue = targetSpeed;
			ot_startingValue = ot_currentValue;
			ot_setTime = _now;
		}

		if (targetSpeed == ot_currentValue) {
      //Serial.print("Speed is already set: ");
      //Serial.println(targetSpeed);
      return 0;
		}


		int newValue;
		if (actionTime == 0) {

			newValue = targetSpeed;
    }
    else {
      if (ot_setTime + actionTime < _now) {
        newValue = targetSpeed;
      }
      else {
        if (ot_startingValue > targetSpeed) {
          newValue = map(_now, ot_setTime, ot_setTime + actionTime, targetSpeed, ot_startingValue);

        }
        else {
          newValue = map(_now, ot_setTime, ot_setTime + actionTime, ot_startingValue, targetSpeed);
        }
      }
    }

		analogWrite(pwmpin, 2.55*abs(newValue));
		digitalWrite(dirpin, (newValue > 0));

		ot_currentValue = newValue;
    return targetSpeed - newValue;
}

bool WHEELMOTOR::isAtTargetSpeed() {
  return ot_currentTargetValue == ot_currentValue;
}

void WHEELMOTOR::setSpeed(int setspeed) {
	//ot_startingValue = setspeed;
 // ot_currentValue = setspeed;
	if (setspeed > 100) setspeed = 100;
	if (setspeed < -100) setspeed = -100;

  // Increase or decrease speed?
  int diff = (setspeed < speed)? -1 : 1;

  // Ramp up/down motor smoothly by changing speed by one %-unit at a time.
  while(speed != setspeed)
  {
		speed += diff;

    setSpeedOverTime(speed, 0);
  //  analogWrite(pwmpin, 255*abs(speed)/100);
		//digitalWrite(dirpin, (speed > 0));

    delayMicroseconds(smoothness_delay);
  }
}


int WHEELMOTOR::getSpeed() {
  return speed;
}


int WHEELMOTOR::getLoad() {
  int load = 0;

  for (int i = 0; i < MOTOR_LOAD_READINGS; i++) {
    load += analogRead(loadpin);
    delay(1);
  }

  return load/MOTOR_LOAD_READINGS;
}


bool WHEELMOTOR::isOverloaded() {
  return (getLoad() > overload_level);
}

void WHEELMOTOR::setOverloadLevel(int level) {
  overload_level = level;
}

void WHEELMOTOR::setSmoothness(int level) {
  smoothness_delay = level;
}
