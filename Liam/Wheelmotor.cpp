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
		unsigned long _now = millis();
		if (targetSpeed != ot_currentTargetValue) {
			ot_currentTargetValue = targetSpeed;
			ot_startingValue = ot_currentValue;
			ot_setTime = _now;
		}

    int newValue;

		if (targetSpeed == ot_currentValue) {
      //Serial.print("Speed is already set: ");
      //Serial.print(targetSpeed);
      _atTargetSpeed = true;
      newValue = targetSpeed;
    }
    else {
      if (actionTime == 0) {
        //Serial.print("Actiontime zero");

        newValue = targetSpeed;
      }
      else {
        if (ot_setTime + actionTime < _now) {
          //Serial.println("Overdue");
          newValue = targetSpeed;
        }
        else {

          newValue = map(_now, ot_setTime, ot_setTime + actionTime, ot_startingValue, targetSpeed);
          //Serial.print("Mapping");
          //Serial.print(" ot_startingValue: ");
          //Serial.print(ot_startingValue);
          //Serial.print(" targetSpeed: ");
          //Serial.print(targetSpeed);
        }
      }
    }
    //Serial.print(" newValue: ");
    //Serial.println(newValue);


		analogWrite(pwmpin, 2.55*abs(newValue));
		digitalWrite(dirpin, (newValue > 0));

		ot_currentValue = newValue;
    bool r = targetSpeed - newValue;
    _atTargetSpeed = r == 0;
    return r;
}

bool WHEELMOTOR::isAtTargetSpeed() {
  return _atTargetSpeed;;
}

void WHEELMOTOR::setSpeed(int setspeed) {
  /*Serial.print("setspeed");
  Serial.println(setspeed);*/

	//ot_startingValue = setspeed;
 // ot_currentValue = setspeed;
	if (setspeed > 100) setspeed = 100;
	if (setspeed < -100) setspeed = -100;

  //// Increase or decrease speed?
  //int diff = (setspeed < speed)? -1 : 1;


  while (setSpeedOverTime(setspeed, smoothness_delay * setspeed / 1000) != 0) {
    delay(1);
  }
  //Serial.println("SetSpeed done");
  //// Ramp up/down motor smoothly by changing speed by one %-unit at a time.
  //while(speed != setspeed)
  //{
		//speed += diff;

  //  setSpeedOverTime(speed, 0);
  ////  analogWrite(pwmpin, 255*abs(speed)/100);
		////digitalWrite(dirpin, (speed > 0));

  //  delayMicroseconds(smoothness_delay);
  //}
}


int WHEELMOTOR::getSpeed() {
  return ot_currentValue;
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
  return (getLoad() < overload_level);
}

void WHEELMOTOR::setOverloadLevel(int level) {
  overload_level = level;
}

void WHEELMOTOR::setSmoothness(int level) {
  smoothness_delay = level;
}
