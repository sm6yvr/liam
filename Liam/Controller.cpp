/// This is the library for a Controller
//
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
  Placed under the GNU license

  ===============================================
*/

#include "Controller.h"
#include "Definition.h"
#include "Arduino.h"

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

  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    sensor->select(i);
    if (!sensor->isOutside())
      return false;
  }

  return true;
}

int CONTROLLER::turnToReleaseLeft(int angle) {
  turnLeft(angle);

  for (int i = 0; i < 20; i++) {
    sensor->select(1);

    if (sensor->isInside()) {
      sensor->select(0);
      if (sensor->isInside())
        return 0;				// OK
    }

    if (wheelsAreOverloaded())
      return 1;					// Overloaded

    turnLeft(10);
  }

  return 2;							// Timed Out
}

int CONTROLLER::turnToReleaseRight(int angle) {
  turnRight(angle);

  for (int i = 0; i < 20; i++) {
    sensor->select(0);

    if (sensor->isInside()) {
      sensor->select(1);
      if (sensor->isInside())
        return 0;				// OK
    }

    if (wheelsAreOverloaded())
      return 1;					// Overloaded

    turnRight(10);
  }

  return 2;							// Timed Out
}

int CONTROLLER::turnRight(int angle) {
  int outcome = 0;

  leftMotor->setSpeed(default_dir_fwd * 100);
  rightMotor->setSpeed(default_dir_fwd * -100);

  delay(angle * TURNDELAY);

  return outcome;
}

int CONTROLLER::turnLeft(int angle) {
  int outcome = 0;

  leftMotor->setSpeed(default_dir_fwd * -100);
  rightMotor->setSpeed(default_dir_fwd * 100);

  delay(angle * TURNDELAY);

  return outcome;
}

int CONTROLLER::waitWhileChecking(int duration) {

  delay(200);   // Let any current spikes settle

  for (int i = 0; i < duration / 30; i++) {
    // check for problems
    if (leftMotor->isOverloaded())
      return 2;
    if (rightMotor->isOverloaded())
      return 2;
    if (sensor->isTimedOut())
      return 3;

    delay(turnDelay);
  }

  // Successful delay
  return 0;
}



int CONTROLLER::waitWhileInside(int duration) {

  for (int k = 0; k < duration / (NUMBER_OF_SENSORS * 200); k++)
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      sensor->select(i);
      if (!sensor->isInside())
        return 2;
    }

  // Successful delay
  return 0;
}

#if GO_BACKWARD_UNTIL_INSIDE
int CONTROLLER::GoBackwardUntilInside (BWFSENSOR *Sensor) {
  int counter = MAX_GO_BACKWARD_TIME;
  // Check if tiltAngle is greater then slopeangle, if not return directly.
  int angle = 0;
  for (int i = 0; i < SLOPEREADINGS; i++)
  {
    angle += compass->getTiltAngle();
    delay(100);
  }
  if (abs(angle / SLOPEREADINGS) <= SLOPEANGLE)
    return 0;
  
  while (Sensor->isInside() == false)
  {
    runBackward(FULLSPEED);
    delay(1000);
    counter--;
    if (counter <= 0)
      return 1;
  }
  return 0;
}
#endif

void CONTROLLER::startCutter() {
  for (int i = 0; i < CUTTERSPEED; i++)
    cutter->setSpeed(i);
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

void CONTROLLER::runForward(int speed) {
  //leftMotor->setSpeed(default_dir_fwd * speed);
  //rightMotor->setSpeed(default_dir_fwd * speed);
  leftMotor->setSpeed(100);
  rightMotor->setSpeed(96);
}

void CONTROLLER::runBackward(int speed) {
  leftMotor->setSpeed(default_dir_fwd * -speed);
  rightMotor->setSpeed(default_dir_fwd * -speed);
}

void CONTROLLER::setDefaultDirectionForward(bool fwd) {
  if (fwd == true)
    default_dir_fwd = 1;
  else
    default_dir_fwd = -1;
};

void CONTROLLER::adjustMotorSpeeds() {
  int  lms = abs(leftMotor->getSpeed());
  int  rms = abs(rightMotor->getSpeed());

  if (!sensor->isInside()) {
    lms = 100;
    rms = 10;
  }
  else if (sensor->isInside())
  {
    lms = 10;
    rms = 100;
  }
  else {
    rms += 80;
    lms += 80;
  }

  if (rms > 100) rms = 100;
  if (lms > 100) lms = 100;
  if (rms < -50) rms = -50;
  if (lms < -50) lms = -50;

  leftMotor->setSpeed(default_dir_fwd * lms);
  rightMotor->setSpeed(default_dir_fwd * rms);

}

void CONTROLLER::updateBalance() {
  balance = balance + leftMotor->getSpeed() - rightMotor->getSpeed();

  if (balance > 0)
    balance -= 10;
  else
    balance += 10;
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
    rms = 0.9 * rms;
  }
  else if (compass->headingVsTarget() > 0) {
    lms = 0.9 * lms;
  }

  leftMotor->setSpeed(default_dir_fwd * lms);
  rightMotor->setSpeed(default_dir_fwd * rms);
  leftMotor->setSpeed(100);
  rightMotor->setSpeed(96);
}

boolean CONTROLLER::wheelsAreOverloaded()
{
  delay(200);       // Settle current spikes
  if (leftMotor->isOverloaded() | rightMotor->isOverloaded()) {
    overloadInterval = millis() - overloadTime;
    overloadTime = millis();
    return true;
  }

  else
    return false;
}


boolean CONTROLLER::hasReachedAStop()
{
  if (overloadInterval > OVERLOAD_INTERVAL)
  {
    overloadInterval = 0;
    return true;
  }
  else
    return false;
}

boolean CONTROLLER::hasBumped() {
  return !digitalRead(BUMPER);
}

boolean CONTROLLER::hasTilted() {
  Serial.println("");
  Serial.println("NU är tiltvärdet :");
  Serial.println(compass->getTiltAngle(), DEC);
  Serial.println(compass->getTiltAngle() > TILTANGLE);
  return (compass->getTiltAngle() > TILTANGLE);
}

boolean CONTROLLER::hasFlipped() {
  Serial.println("");
  Serial.println("NU är Flippvärdet :");
  Serial.println(compass->getTiltAngle(), DEC);
  Serial.println(compass->getTiltAngle() > FLIPANGLE);
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
