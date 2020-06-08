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

int BWFSENSOR::outside_code[] = {OUTSIDE_BWF, INSIDE_BWF - OUTSIDE_BWF, OUTSIDE_BWF, INSIDE_BWF - OUTSIDE_BWF };
int BWFSENSOR::inside_code[] = {INSIDE_BWF, INSIDE_BWF};


BWFSENSOR::BWFSENSOR(int selA, int selB) {
  selpin_A = selA;
  selpin_B = selB;
}


int BWFSENSOR::getCurrentSensor() {
  return _currentSensor;
}

void BWFSENSOR::setup() {

  //select(0);
}

void BWFSENSOR::selectNext() {
  if (_manualSensorSelect) {
    return;
  }
  if (signal_status != NOSIGNAL) {
    //Serial.println("Got signal");
    select((_currentSensor + 1) % NUMBER_OF_SENSORS);
    return;
  }

  if (millis() - lastSwitch > BWF_COLLECT_SIGNAL_TIME) {
    /*Serial.print(millis());
    Serial.print(" Signal timout on sensor ");
    Serial.print(_currentSensor);
    Serial.println(", switching");*/
    assignIfNeeded(_currentSensor, NOSIGNAL);
    select((_currentSensor + 1) % NUMBER_OF_SENSORS);
  }
}

void BWFSENSOR::SetManualSensorSelect(bool useManualMode){
  _manualSensorSelect = useManualMode;
}

// Select active sensor
void BWFSENSOR::select(int sensornumber) {
  //Serial.print("Selecting sensor: ");
  //Serial.println(sensornumber);

   if (_currentSensor == sensornumber) {
    return;
  }
  _switching = true;
  digitalWrite(selpin_A, (sensornumber & 1) > 0 ? HIGH : LOW);
  digitalWrite(selpin_B, (sensornumber & 2) > 0 ? HIGH : LOW);
  _currentSensor = sensornumber;
  clearSignal();
  lastSwitch = millis();
  _switching = false;
  // Serial.println(_currentSensor);
  // clearSignal();

  _switching = false;

   //long time = millis();
   //while (signal_status == NOSIGNAL
   //  && millis() - time < BWF_COLLECT_SIGNAL_TIME) // max time of 200ms
   //{
   //  delay(1);
   //}

  // delay(200);
}


void BWFSENSOR::clearSignal() {
  for (int i = 0; i < arr_length; i++)
    arr[i] = 0;
  signal_status = NOSIGNAL;
  //sensorValue[_currentSensor] = NOSIGNAL;
  pulse_count_inside = 0;
  pulse_count_outside = 0;
  arr_count = 0;
}


bool BWFSENSOR::isInside(int sensornumber) {
  return (sensorValue[sensornumber] == INSIDE);
}

bool BWFSENSOR::isOutside(int sensornumber) {
  return (sensorValue[sensornumber] == OUTSIDE);
}

bool BWFSENSOR::isOutOfBounds(int sensornumber) {
	if (BWF_DETECTION_ALWAYS)
		return !isInside(sensornumber);
	else
		return isOutside(sensornumber);
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
  //bool needOneMoreRun = false;
  long now = micros();
  if (_switching)
  {
    last_pulse = now;
    Serial.println(">> new puls while still processing.");
    return; //Avoid data for undefined state for selection pins
  }
  char buf[40];
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
      //sensorValue[_currentSensor] = INSIDE;
      signal_status = INSIDE;
      assignIfNeeded(_currentSensor, signal_status);
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
      //sensorValue[_currentSensor] = OUTSIDE;
      signal_status = OUTSIDE;
      assignIfNeeded(_currentSensor, signal_status);
      last_match = millis();
      pulse_count_outside=0;
    }
  } else {
    pulse_count_outside=0;

  }



  // Store the received code for debug output
  arr[arr_count++] = pulse_length;
  if (arr_count>arr_length) arr_count=0;
//sensorValue[_currentSensor] = isOutOfBounds();
//select((_currentSensor+1) % NUMBER_OF_SENSORS);
  selectNext();
}

void BWFSENSOR::assignIfNeeded(int sensor, int signalStatus) {
  if (sensorValue[sensor] != signalStatus) {
    sensorValue[_currentSensor] = signalStatus;
    //Signal change here if needed
    Serial.print(F("Sensor "));
    Serial.print(sensor);
    Serial.print(F(" "));
    Serial.println(getSignalStatusName(signalStatus));
  };
}

String BWFSENSOR::getSignalStatusName(int signalStatus) {

  switch (signalStatus)
  {
  case INSIDE:
    return F("Inside");
  case OUTSIDE:
    return F("Outside");
  case NOSIGNAL:
    return F("No signal");
  default:
    return F("UNKNOWN");
  }
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
