/// This is the library for a BWF Sensor
//
// Changelog:
//     2014-12-14 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#include "BWFSensor.h"

int BWFSENSOR::outside_code[] = {OUTSIDE_BWF};
int BWFSENSOR::inside_code[] = {INSIDE_BWF};	

/** Specific constructor.
 */
BWFSENSOR::BWFSENSOR(int selA, int selB) {
	selpin_A = selA;
	selpin_B = selB;
}


// select this sensor to be active
void BWFSENSOR::select(int sensornumber) {
   digitalWrite(selpin_A, (sensornumber & 1) > 0 ? HIGH : LOW);
   digitalWrite(selpin_B, (sensornumber & 2) > 0 ? HIGH : LOW);
   clearSignal();
   delay(200);			// Wait a little to collect signal
   }


void BWFSENSOR::clearSignal() {
	for (int i=0; i<arr_length; i++)
		arr[i]=0;
	signal_status = NOSIGNAL;
	pulse_count_inside = 0;
	pulse_count_outside = 0;
	arr_count = 0;
}


bool BWFSENSOR::isInside() {
	return (signal_status == INSIDE);
}

bool BWFSENSOR::isOutside() {
	return (signal_status == OUTSIDE);
}

bool BWFSENSOR::isTimedOut() {
	return (signal_status_checked + TIMEOUT_DELAY < millis());
}

bool BWFSENSOR::hasNoSignal() {
	return (signal_status_checked + NO_SIGNAL_DELAY < millis());
}

// This routine is run at every timer interrupt and updates the sensor status
void BWFSENSOR::readSensor() {
  volatile int pulse_unit = 0;
  
  // Calculate the time since last pulse
  pulse_length = int(micros() - pulse_time);
  pulse_time = micros(); 
  pulse_unit = (pulse_length+half_unit_length) / pulse_unit_length;


  // Store the numbers for debug printout
  arr[arr_count++] = pulse_unit;
  	if (arr_count>arr_length) arr_count=0;

  // Check if the latest pulse fits the code for inside
  if (abs(pulse_unit-inside_code[pulse_count_inside]) < 2) {
    pulse_count_inside++;
    // If the whole code sequence has been OK, then set signal status to 1
    if (pulse_count_inside >= sizeof(inside_code)/sizeof(inside_code[0])) {
  		signal_status = INSIDE;
  		signal_status_checked = millis();
      	pulse_count_inside=0;
    }
  }
  else
    pulse_count_inside=0;

  // Check if the latest pulse fits the code for outside
  if (abs(pulse_unit-outside_code[pulse_count_outside]) < OUTSIDE_READINGS) {
    pulse_count_outside++;
    if (pulse_count_outside >= sizeof(outside_code)/sizeof(outside_code[0])) {
 		signal_status = OUTSIDE;
  		signal_status_checked = millis();
      	pulse_count_outside=0;
    }
  }
  else
    pulse_count_outside=0;

}

void BWFSENSOR::printSignal() {
	
	for (int i=0; i<arr_length; i++) {
		Serial.print(arr[i]);
		Serial.print(" ");
	}

}
