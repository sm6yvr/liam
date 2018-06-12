/*
 Liam - DIY Robot Lawn Mower

 Controller Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Controller.h"
#include "Definition.h"
#include "Error.h"

extern long time_at_turning;

/** Specific constructor.
*/
CONTROLLER::CONTROLLER(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp) {
  leftMotor = left;
  rightMotor = right;
  cutter = cut;
  sensor = bwf;
  compass = comp;
  default_dir_fwd = 1;
  balance = 0;
}


boolean CONTROLLER::allSensorsAreOutside() {

  for(int i=0; i<NUMBER_OF_SENSORS; i++) {
    sensor->select(i);
    if (!sensor->isOutOfBounds())
      return false;
  }

  return true;
}

int CONTROLLER::getFirstSensorOutOfBounds() {
	for (int i = 0; i<NUMBER_OF_SENSORS; i++) {
		sensor->select(i);
		if (sensor->isOutOfBounds())
			return i;
	}

	return -1;
}

int CONTROLLER::turnToReleaseLeft(int angle) {
  turnLeft(angle);

  for (int i=0; i<20; i++) {
    sensor->select(1);

    if (!sensor->isOutOfBounds()) {
      sensor->select(0);
      if (!sensor->isOutOfBounds())
        return 0;       // OK
    }

    if (wheelsAreOverloaded())
      return ERROR_OVERLOAD;         // Overloaded

    turnLeft(10);
  }

  return ERROR_OUTSIDE;             // Timed Out
}

int CONTROLLER::turnToReleaseRight(int angle) {
  turnRight(angle);

  for (int i=0; i<20; i++) {
    sensor->select(0);

    if (!sensor->isOutOfBounds()) {
      sensor->select(1);
      if (!sensor->isOutOfBounds())
        return 0;       // OK
    }

    if (wheelsAreOverloaded())
      return ERROR_OVERLOAD;         // Overloaded

    turnRight(10);
  }

  return ERROR_OUTSIDE;             // Timed Out
}

int CONTROLLER::turnRight(int angle) {
  int outcome = 0;

  leftMotor->setSpeed(default_dir_fwd*100);
  rightMotor->setSpeed(default_dir_fwd*-100);

  delay(angle*TURNDELAY);

  return outcome;
}

int CONTROLLER::turnLeft(int angle) {
  int outcome = 0;

  leftMotor->setSpeed(default_dir_fwd*-100);
  rightMotor->setSpeed(default_dir_fwd*100);

  delay(angle*TURNDELAY);

  return outcome;
}



int CONTROLLER::waitWhileChecking(int duration) {

  delay(200);   // Let any current spikes settle

  for (int i=0; i<duration/30; i++) {
    // check for problems
    if(leftMotor->isOverloaded())
      return 2;
    if(rightMotor->isOverloaded())
      return 2;
    if (sensor->isTimedOut())
      return 3;

    delay(turnDelay);
  }

  // Successful delay
  return 0;
}



int CONTROLLER::waitWhileInside(int duration) {

  for (int k=0; k<duration/(NUMBER_OF_SENSORS*200); k++)
    for (int i=0; i<NUMBER_OF_SENSORS; i++) {
      sensor->select(i);
      if(sensor->isOutOfBounds())
        return 2;
    }

  // Successful delay
  return 0;
}

int CONTROLLER::GoBackwardUntilInside (BWFSENSOR *Sensor) {
#ifdef GO_BACKWARD_UNTIL_INSIDE
  int counter=MAX_GO_BACKWARD_TIME;
  //Mover has just stoped. Let it pause for a second.
  delay(1000);
  while(!sensor->isOutOfBounds()==false)
  {
    runBackward(FULLSPEED);
    delay(1000);
    counter--;
    if(counter<=0)
      return ERROR_OUTSIDE;
  }
#endif
  return 0;
}
void CONTROLLER::startCutter() {
  cutter->setSpeedOverTime(CUTTERSPEED, CUTTER_SPINUP_TIME);
}

void CONTROLLER::stopCutter() {
  cutter->setSpeed(0);
}

void CONTROLLER::storeState() {
  leftMotorSpeed = leftMotor->getSpeed();
  rightMotorSpeed = rightMotor->getSpeed();
  cutterSpeed = cutter->getSpeed();
}

void CONTROLLER::restoreState() {
  leftMotor->setSpeed(leftMotorSpeed);
  rightMotor->setSpeed(rightMotorSpeed);
  cutter->setSpeed(cutterSpeed);
}


void CONTROLLER::runForwardOverTime(int minSpeed, int targetSpeed, int time) {
  if (leftMotor->getSpeed() < minSpeed) leftMotor->setSpeed(minSpeed);
  if (rightMotor->getSpeed() < minSpeed) rightMotor->setSpeed(minSpeed);
  leftMotor->setSpeedOverTime(default_dir_fwd*targetSpeed, time);
  rightMotor->setSpeedOverTime(default_dir_fwd*targetSpeed, time);
}


void CONTROLLER::runForward(int speed) {
  leftMotor->setSpeed(default_dir_fwd*speed);
  rightMotor->setSpeed(default_dir_fwd*speed);
}

void CONTROLLER::runBackward(int speed) {
  leftMotor->setSpeed(default_dir_fwd*-speed);
  rightMotor->setSpeed(default_dir_fwd*-speed);
}

void CONTROLLER::setDefaultDirectionForward(bool fwd) {
  if (fwd == true)
    default_dir_fwd = 1;
  else
    default_dir_fwd = -1;
};

void CONTROLLER::adjustMotorSpeeds() {
  int  lms;
  int  rms;
  int ltime;
  int rtime;
  int lowSpeed = 40;
  int highSpeed = FULLSPEED;
  int shortTime = 10;
  int longTime = 500;

  if (sensor->isOutOfBounds()) {
	  //Serial.println("Adjust to out of bounds");
    lms = highSpeed;
	ltime = shortTime;
    rms = lowSpeed;
	rtime = longTime;
  }
  else
  {
	  //Serial.println("Adjust to inside bounds");
	  lms = lowSpeed;
	  ltime = longTime;
	  rms = highSpeed;
	  rtime = shortTime;
  }


  leftMotor->setSpeedOverTime(default_dir_fwd*lms, ltime);
  rightMotor->setSpeedOverTime(default_dir_fwd*rms, rtime);
}

void CONTROLLER::updateBalance() {
  balance = balance + leftMotor->getSpeed() - rightMotor->getSpeed();

  if(balance > 0)
    balance-=10;
  else
    balance+=10;
}



void CONTROLLER::stop() {
  leftMotor->setSpeed(0);
  rightMotor->setSpeed(0);

}

int CONTROLLER::compensateSpeedToCutterLoad() {

}

int CONTROLLER::compensateSpeedToCompassHeading() {
  int  lms = abs(leftMotor->getSpeed());
  int  rms = abs(rightMotor->getSpeed());

  if (compass->headingVsTarget() < 0) {
    rms = 0.9*rms;
  }
  else if (compass->headingVsTarget() > 0) {
    lms = 0.9*lms;
  }

  leftMotor->setSpeed(default_dir_fwd*lms);
  rightMotor->setSpeed(default_dir_fwd*rms);
}

boolean CONTROLLER::wheelsAreOverloaded() {
	long now = millis();
	int l_load = 0;
	int r_load = 0;
	int l_load_limit = 0;
	int r_load_limit = 0;
	int counter = 0;
	while (millis() - now <= 200)
	{
    l_load = leftMotor->isAtTargetSpeed() ? leftMotor->getLoad() : 0;
    l_load_limit = WHEELMOTOR_OVERLOAD;// *max(60, abs(leftMotor->getSpeed())) / FULLSPEED;

		r_load = rightMotor->isAtTargetSpeed() ? rightMotor->getLoad() : 0;
    r_load_limit = WHEELMOTOR_OVERLOAD;// *max(60, abs(rightMotor->getSpeed())) / FULLSPEED;
		/*counter++;*/
		delay(1);
		if (l_load  < l_load_limit && r_load < r_load_limit)
		{
			return false;
    }
  }

	return true;
}

void CONTROLLER::turnIfObstacle() {
  // Check if bumper has triggered (providing you have one enabled)
  if (
#if defined __Bumper__
    hasBumped() ||
#endif
#if defined __MS9150__ || defined __MS5883L__ || defined __ADXL345__
    hasTilted() ||
#endif
    wheelsAreOverloaded()) {
    int angle = random(90, 160);
    runBackward(FULLSPEED);
    delay(1200);

    if (random(0, 100) % 2 == 0) {
      turnRight(angle);
    }
    else {
      turnLeft(angle);
    }
    stop();
    time_at_turning = millis();
    compass->setNewTargetHeading();

    //runForward(FULLSPEED);
  }
}
boolean CONTROLLER::hasBumped() {
  return !digitalRead(BUMPER);
}

boolean CONTROLLER::hasTilted() {
  return (compass->getTiltAngle() > TILTANGLE);
}

boolean CONTROLLER::hasFlipped() {
  return (compass->getTiltAngle() > FLIPANGLE);
}

boolean CONTROLLER::isLifted() {
  return !digitalRead(LIFT_SENSOR_PIN);
}

void CONTROLLER::resetBalance() {
  balance = 0;
}

int CONTROLLER::getBalance() {
  return balance;
}
