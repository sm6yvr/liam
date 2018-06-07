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




void WHEELMOTOR::setSpeedOverTime(int speed, int actionTime) {
	//void setPWM(int pwmPin, int directionPin, int targetValue, int totalActionMs) {


		int _now = millis();
		if (speed != ot_currentTargetValue) {
			ot_currentTargetValue = speed;
			ot_startingValue = ot_currentValue;
			ot_setTime = _now;
		}

		if (speed == ot_currentValue) {
			return;
		}


		int newValue;
		if (actionTime == 0) {
			newValue = speed;
		} if (ot_setTime + actionTime < _now) {
			newValue = speed;
		}
		else {
			newValue = map(_now, ot_setTime, ot_setTime + actionTime, ot_startingValue, speed);
		}

		analogWrite(pwmpin, 2.55*abs(newValue));
		digitalWrite(dirpin, (newValue > 0));

		//Serial.println(newValue);
		speed = newValue;
		ot_currentValue = newValue;
	
}

void WHEELMOTOR::setSpeed(int setspeed) {
	ot_startingValue = setspeed;
	if (setspeed > 100) setspeed = 100;
	if (setspeed < -100) setspeed = -100;

  // Increase or decrease speed?
  int diff = (setspeed < speed)? -1 : 1;

  // Ramp up/down motor smoothly by changing speed by one %-unit at a time.
  while(speed != setspeed)
  {
		speed += diff;

    analogWrite(pwmpin, 255*abs(speed)/100);
		digitalWrite(dirpin, (speed > 0));

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
