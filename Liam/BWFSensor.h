/* This is the library for a BWFSensor
//
// Changelog:
//     2014-12-14 - Initial version by Jonas

SelA	selection pin for the sensor swith
SelB	selection pin for the sensor swith
in[]	an array of integers for the inside code
out[]	an array of integers for the outside code
todel	the time until time out is signalled
nodel	the time until the sensor has lost a signal

============================================
Placed under the GNU license

===============================================
*/

#include <Arduino.h>
#include "Definition.h"

#ifndef _BWFSENSOR_H_
#define _BWFSENSOR_H_

#define INSIDE		1
#define	NOSIGNAL	0
#define OUTSIDE		-1

// BWF Code for timout and no signal (in milliseconds)
#define TIMEOUT_DELAY					20000
#define NO_SIGNAL_DELAY					4000

class BWFSENSOR {
    public:
        BWFSENSOR(int selA, int selB);

        void select(int sensornumber);

        void attach(int intpin);
    	  void readSensor();
      
        bool isTimedOut();
        bool isInside();
        bool isOutside();
        bool hasNoSignal();

        int getSignal(int value);
        void printSignal();
        void clearSignal();

        int getArrLength();
    private:
    // BWF Code for inside and outside the fence
		static int inside_code[];
		static int outside_code[];
    	int selpin_A;
    	int selpin_B;
    	int counter;
    	int signal_status;
    	long last_interrupt;
    	long signal_status_checked;
    	long pulse_length;
    	long pulse_time;
    	int pulse_count_inside;
    	int pulse_count_outside;
    	int sensor_number;
    	const static int pulse_unit_length = 100;
    	const static int half_unit_length = 50;
    	const static int arr_length=3;
    	int arr[arr_length];
    	int arr_count;



};

#endif /* _BWFSENSOR_H_ */
