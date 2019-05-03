/*
 Liam - DIY Robot Lawn Mower

 BWF Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include <Arduino.h>
#include "Definition.h"

#ifndef _BWFSENSOR_H_
#define _BWFSENSOR_H_

#define INSIDE    1
#define NOSIGNAL  0
#define OUTSIDE   -1

// BWF Code for timout and no signal (in milliseconds)
#define TIMEOUT_DELAY    20000
#define NO_SIGNAL_DELAY  4000


class BWFSENSOR {
  public:
    BWFSENSOR(int selA, int selB);
    void setup();

    void selectNext();
    void select(int sensornumber);
    void SetManualSensorSelect(bool useManualMode);


    bool isInside(int sensornumber);
    bool isOutside(int sensornumber);
	  bool isOutOfBounds(int sensornumber);
    bool isTimedOut();
    bool hasNoSignal();
    bool gotSignal();

    void readSensor();


    void printSignal();

  private:
    String getSignalStatusName(int signalStatus);
    void assignIfNeeded(int sensor, int signalStatus);

    int sensorValue[NUMBER_OF_SENSORS];
    int getCurrentSensor();
    void clearSignal();
    // BWF Code for inside and outside the fence
    static int inside_code[];
    static int outside_code[];
    int _currentSensor = 0;
    const static int pulse_unit_length = 100;

    int pulse_count_inside;
    int pulse_count_outside;

    int selpin_A;
    int selpin_B;
    volatile bool _switching; //volatile since it's share between interrupt and loop
    int signal_status;
    unsigned long lastSwitch;
    long last_match;
    long last_pulse;

    // Array for debug printing
    const static int arr_length=10;
    int arr[arr_length];
    int arr_count;
    bool _manualSensorSelect = false;
};

#endif /* _BWFSENSOR_H_ */
