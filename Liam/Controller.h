// This is the library for a Controller
//
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/
#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "MMA_7455.h"
#include "Definition.h"

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

class CONTROLLER {
    public:
        CONTROLLER(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, DEFINITION *def);

        //
        int turn(int degrees);
        int turnToReleaseLeft(int degrees);
        int turnToReleaseRight(int degrees);
        int waitWhileChecking(int duration);
        int waitWhileInside(int duration);
	      int GoBackwardUntilInside(BWFSENSOR *Sensor);
        void runForward(int speed);
        void runBackward(int speed);
        void stop();

        boolean allSensorsAreOutside();

        void startCutter();
        void stopCutter();

        void setDefaultDirectionForward(bool fwd);

        void adjustMotorSpeeds();
        void adjustMotorSpeeds(short percent); // innerwheel speed is percent of FULLSPEED
        int compensateSpeedToCutterLoad();
        int compensateSpeedToCompassHeading();

        boolean wheelsAreOverloaded(int delaytime);
        boolean cutterIsOverloaded();
        boolean hasReachedAStop();

        boolean hasBumped();
        boolean hasTilted();
        boolean hasFlipped();
        boolean isLifted();

        void resetBalance();
        int getBalance();

        void updateBalance();
        void storeState();
        void restoreState();

        int turnLeft(int degrees);
        int turnRight(int degrees);
        
    private:
      WHEELMOTOR* leftMotor;
      WHEELMOTOR* rightMotor;
    	CUTTERMOTOR* cutter;
    	BWFSENSOR* sensor;
    	MOTIONSENSOR* compass;
      DEFINITION *definitiondefault;
    	const static int turnDelay = TURNDELAY;
    	const static int mowerTimeout = TIMEOUT;

		int default_dir_fwd;
		int balance;

		int leftMotorSpeed;
		int rightMotorSpeed;
		int cutterSpeed;
    short counter=0;


    bool lastloopsensorwasoutside = false;
    unsigned long dockingLeftSensorInside=millis();
    unsigned long overloadTime;
    unsigned long overloadInterval;

};

#endif /* _CONTROLLER_H_ */
