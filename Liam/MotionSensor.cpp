/// This is the library for any type of motion sensor
//
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#include "MotionSensor.h"

boolean MOTIONSENSOR::initialize() {
	return true;						// true = connection OK
}


void MOTIONSENSOR::updateHeading() {
    current_heading = getHeading();
}

void MOTIONSENSOR::setNewTargetHeading() {
	for (int i=0; i<10; i++)			// take 10 readings to get a stable value
		target_heading = getHeading();
}

int MOTIONSENSOR::headingVsTarget() {				// Will return angle difference between heading and target
	return 	copysign(1.0,current_heading - target_heading) *
			copysign(1.0,abs(current_heading-target_heading)-180) *
			(180-abs(abs(current_heading-target_heading)-180));
}

int MOTIONSENSOR::getHeading() {
  return 0;
}

int MOTIONSENSOR::getTiltAngle() {
	return 0;
}

