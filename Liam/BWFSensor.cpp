/*
 Liam - DIY Robot Lawn Mower

 BWF Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/
/*
  The BWF sensor works by measuring the time between rising edges

  Example:
    Signal in BWF

    I
    ^
    |      _         _
    |     | |       | |
    |     | |       | |
    |...__| |  _____| |  ___...
    |       | |       | |
    |       | |       | |
    |       |_|       |_|
    |
    +----------------------> t
           1 1   5   1 1    5...

    Outside of the fence, the sensor coil and amplifier circuit will sense the
    rising edges in the signal. Inside the fence, the signal is inverted, and
    the circuit will sense the falling edges of the signal instead.

    In this example, the time between rising edges is 2 units, followed by 5,
    2, 5, and so on. The time between falling edges is 7 units.

  When a rising edge is detected on the currently selected sensor, the function
  readSensor() is run. By keeping track of the last time it was run, it can
  calculate the time between pulses and check if it matches what was expected
  for being inside or outside of the fence.
*/

#include "BWFSensor.h"

int BWFSENSOR::outside_code[] = {OUTSIDE_BWF, INSIDE_BWF-OUTSIDE_BWF};
int BWFSENSOR::inside_code[] = {INSIDE_BWF};

int currentSensor = 0;

BWFSENSOR::BWFSENSOR(int selA, int selB) {
  selpin_A = selA;
  selpin_B = selB;
}


// Select active sensor
void BWFSENSOR::select(int sensornumber) {
  if (currentSensor == sensornumber) {

    return;
  }
  currentSensor = sensornumber;

  digitalWrite(selpin_A, (sensornumber & 1) > 0 ? HIGH : LOW);
  digitalWrite(selpin_B, (sensornumber & 2) > 0 ? HIGH : LOW);
  clearSignal();
  long time = millis();
  while (signal_status == NOSIGNAL 
    && millis() - time < BWF_COLLECT_SIGNAL_TIME) // max time of 200ms
  {
    delay(1);
  }

  // delay(200);
}


void BWFSENSOR::clearSignal() {
  for (int i = 0; i < arr_length; i++)
    arr[i] = 0;
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

bool BWFSENSOR::isOutOfBounds() {
	if (BWF_DETECTION_ALWAYS)
		return !isInside();
	else
		return isOutside();
}

bool BWFSENSOR::isTimedOut() {
  return (last_match + TIMEOUT_DELAY < millis());
}

bool BWFSENSOR::hasNoSignal() {
  return (last_match + NO_SIGNAL_DELAY < millis());
}


// This function is run each time the BWF pin gets a pulse
// For accuracy, this function should be kept as fast as possible
void BWFSENSOR::readSensor() {
  long now = micros();

  // Calculate the time since last pulse
  int time_since_pulse = int(now - last_pulse);
  last_pulse = now;

  // Convert to pulse units (rounding up)
  int pulse_length = (time_since_pulse+(pulse_unit_length/2)) / pulse_unit_length;

  // Check if the latest pulse fits the code for inside
  if (abs(pulse_length-inside_code[pulse_count_inside]) < 2) {
    pulse_count_inside++;

    // Check if the entire pulse train has been batched
    if (pulse_count_inside >= sizeof(inside_code)/sizeof(inside_code[0])) {
      signal_status = INSIDE;
      last_match = millis();
      pulse_count_inside=0;
    }
  } else {
    pulse_count_inside=0;
  }

  // Check if the latest pulse fits the code for outside
  if (abs(pulse_length-outside_code[pulse_count_outside]) < 2) {
    pulse_count_outside++;
    if (pulse_count_outside >= sizeof(outside_code)/sizeof(outside_code[0])) {
      signal_status = OUTSIDE;
      last_match = millis();
      pulse_count_outside=0;
    }
  } else {
    pulse_count_outside=0;
  }


  // Store the received code for debug output
  arr[arr_count++] = pulse_length;
  if (arr_count>arr_length) arr_count=0;
}

void BWFSENSOR::printSignal() {
  for (int i = 0; i < arr_length; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}
bool BWFSENSOR::gotSignal()
{
  return arr_count >= BWF_NUMBER_OF_PULSES ? true : false;
}