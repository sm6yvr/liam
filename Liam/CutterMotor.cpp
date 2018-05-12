/* This is the library for a Cutter Motor

The motor can be either brushed or brushless

Motor speed is defined as percentage of full speed.
A speed of 100 means full speed and 0 is stop.
Speed can also be negative if reverse direction is requested.

Current draw of the motor can be read using the getLoad() method

// Changelog:
//     2014-12-13 - Initial version by Jonas

============================================
Placed under the GNU license

===============================================
*/

#include "CutterMotor.h"

/** Specific constructor.
 */
CUTTERMOTOR::CUTTERMOTOR(int type_, int pwmpin_, int loadpin_) {
    type = type_;
    pwmpin = pwmpin_;
    loadpin = loadpin_;

}

void CUTTERMOTOR::initialize() {
	// Initialize if brushless with ESC
	if (type == 0) {
		cutter.attach(pwmpin);
		cutter.writeMicroseconds(2000);
		delay(400);
		cutter.writeMicroseconds(1000);
		delay(2000);
	}	
}


void CUTTERMOTOR::setSpeed(int setspeed) {
   speed = setspeed;
   if (speed > 100)	speed = 100;
   if (speed < 0) speed = 0;
   
   if (type == 0)			// brushless
   	pwm = abs(10*speed)+1000;
   else if (type == 1)		// brushed
   	pwm = abs(2.55*speed);
   else if (type == 2)		// Nidec
    pwm = 255 - abs(2.55*speed);
    
   braking = false;
   
   if (type == 0)
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



void CUTTERMOTOR::brake() {
	setSpeed(0);
	braking = true;
}


bool CUTTERMOTOR::isBraking() {
	return braking;
}

bool CUTTERMOTOR::isOverloaded() {
	return (getLoad() > overload_level);
}

void CUTTERMOTOR::setOverloadLevel(int level) {
	overload_level = level;
}
