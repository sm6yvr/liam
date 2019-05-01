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
    if (!sensor->isOutOfBounds(i))
      return false;
  }

  return true;
}

int CONTROLLER::getFirstSensorOutOfBounds() {
	for (int i = 0; i<NUMBER_OF_SENSORS; i++) {
		if (sensor->isOutOfBounds(i))
			return i;
	}

	return -1;
}

int CONTROLLER::turnToReleaseLeft(int angle) {
  //return 0; //Hack to get it running until we figure out what the heck is going on

  for (int i=0; i<180; i++) {
    if (!sensor->isOutOfBounds(0) && !sensor->isOutOfBounds(1))
    {
      turnLeft(angle);
      return 0;       // OK
    }

    if (wheelsAreOverloaded())
      return ERROR_OVERLOAD;         // Overloaded

    turnLeft(10);
  }

  return ERROR_OUTSIDE;             // Timed Out
}

int CONTROLLER::turnToReleaseRight(int angle) {

  //return 0; //Hack to get it running until we figure out what the heck is going on

  for (int i=0; i<(180) / 10; i++) {

    if (!sensor->isOutOfBounds(0) && !sensor->isOutOfBounds(1))
    {
        turnRight(angle);
        return 0;       // OK
    }

    if (wheelsAreOverloaded())
      return ERROR_OVERLOAD;         // Overloaded

    turnRight(10);
  }

  return ERROR_OUTSIDE;             // Timed Out
}

int CONTROLLER::turn(int angle, bool toLeft) {
  unsigned long start = millis();
  int outcome = 0;
  int leftFactor = toLeft ? -1 :  1;
  bool done = false;
  while (!done){
    int l = leftMotor->setSpeedOverTime(leftFactor * default_dir_fwd * 100, 30);
    int r = rightMotor->setSpeedOverTime(-leftFactor * default_dir_fwd * 100, 30);
    done = l == 0 && r == 0;
    delay(1);
  }
  delay(angle*TURNDELAY + start - millis());
  stop();
  return outcome;
}

int CONTROLLER::turnRight(int angle) {
  return turn(angle, false);
}

int CONTROLLER::turnLeft(int angle) {
  return turn(angle, true);
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
      if(sensor->isOutOfBounds(i))
        return 2;
    }

  // Successful delay
  return 0;
}

int CONTROLLER::GoBackwardUntilInside (int sensorNumber) {
#ifdef GO_BACKWARD_UNTIL_INSIDE
  int counter=MAX_GO_BACKWARD_TIME;
  //Mover has just stoped. Let it pause for a second.
  stop();
  long now = millis();
  long timeout = now + (MAX_GO_BACKWARD_TIME * 1000);
  // while(!sensor->isOutOfBounds()==false)
  while(sensor->isOutOfBounds(sensorNumber))
  {
    runBackwardOverTime(SLOWSPEED, FULLSPEED, ACCELERATION_DURATION);
    if(timeout - millis() < 0)
      return ERROR_OUTSIDE;
  }
#endif
  return 0;
}
void CONTROLLER::startCutter() {
  cutter->setSpeedOverTime(CUTTERSPEED, CUTTER_SPINUP_TIME);
}

void CONTROLLER::stopCutter() {
  cutter->setSpeedOverTime(0, 0);
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


void CONTROLLER::runBackwardOverTime(int minSpeed, int targetSpeed, int time) {
  if (leftMotor->getSpeed() > -minSpeed) leftMotor->setSpeed(-minSpeed);
  if (rightMotor->getSpeed() > -minSpeed) rightMotor->setSpeed(-minSpeed);
  leftMotor->setSpeedOverTime(default_dir_fwd*-targetSpeed, time);
  rightMotor->setSpeedOverTime(default_dir_fwd*-targetSpeed, time);
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

void CONTROLLER::adjustMotorSpeeds(bool isOutOfBounds) {
  int  lms;
  int  rms;
  int ltime;
  int rtime;
  int lowSpeed = DOCKING_WHEEL_LOW_SPEED;
  int highSpeed = DOCKING_WHEEL_HIGH_SPEED;
  int shortTime = DOCKING_TIME_TO_HIGH_SPEED;
  int longTime = DOCKING_TIME_TO_SLOW_SPEED;

  if (isOutOfBounds) {
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
  if (!leftMotor->isAtTargetSpeed() || !rightMotor->isAtTargetSpeed()) return;

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
    l_load_limit = WHEELMOTOR_OVERLOAD;// *max(60, abs(leftMotor->getSpeed())) / MOWING_SPEED;

		r_load = rightMotor->isAtTargetSpeed() ? rightMotor->getLoad() : 0;
    r_load_limit = WHEELMOTOR_OVERLOAD;// *max(60, abs(rightMotor->getSpeed())) / MOWING_SPEED;
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

    //runForward(MOWING_SPEED);
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
