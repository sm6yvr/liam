/* This is the library for a WheelMotor

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

#include "Wheelmotor.h"


/** Specific constructor.
 */
WHEELMOTOR::WHEELMOTOR(int pwmpin_, int dirpin_, int loadpin_, int smoothness) {
	// Set up PWM frequency for pin 3 and 11 to 3921 Hz for smooth running
	// At default pwm, the frequency of the motors will disturb the BWFsensors
	// This is very evident if using chinese wheelmotors
//  	TCCR2B = TCCR2B & 0b11111000 | 0x02;

    pwmpin = pwmpin_;
    dirpin = dirpin_;
    loadpin = loadpin_;
    smoothness_delay = smoothness;
}


void WHEELMOTOR::setSpeed(int setspeed) {
  	int diff = 1-2*((setspeed-speed) < 0);
   	int stepnr = abs(setspeed-speed);

   	if (setspeed > 100)	setspeed = 100;
   	if (setspeed < -100) setspeed = -100;

   	dir = (setspeed > 0);

   	for (int i=0; i<stepnr; i++){
   		speed += diff;
	   	analogWrite(pwmpin, 2.55*abs(speed));
   		digitalWrite(dirpin, (speed > 0));
   		delayMicroseconds(smoothness_delay);					// Smooth ramping of motors
   	}

  	speed = setspeed;
}


int WHEELMOTOR::getSpeed() {
	return speed;
}


int WHEELMOTOR::getLoad() {
	load = 0;

	for (int i=0; i<10; i++) {
		load += analogRead(loadpin);
		delay(1);
	}
	return load/10;
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
