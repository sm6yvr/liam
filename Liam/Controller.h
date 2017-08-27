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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

class CONTROLLER {
    public:
        CONTROLLER(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp);
        
        // 
        int turn(int degrees);
        int turnToReleaseLeft(int degrees);
        int turnToReleaseRight(int degrees);
        int waitWhileChecking(int duration);
        int runWhileInside(int speed, int distance);
	int GoBackwardUntilInside(BWFSENSOR *Sensor);
        void runForward(int speed);
        void runForward(int speed, int distance);
        void runBackward(int speed);
        void runBackward(int speed, int distance);
        void stop();
        
        boolean allSensorsAreOutside();
        
        void startCutter();
        void stopCutter();
        
        void setDefaultDirectionForward(bool fwd);

        void adjustMotorSpeeds();
        int compensateSpeedToCutterLoad();
        int compensateSpeedToCompassHeading();
        
        boolean wheelsAreOverloaded();
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
    	const static int turnDelay = TURNDELAY;
    	const static int mowerTimeout = TIMEOUT;
    	
		int default_dir_fwd;
		int balance;
		
		int leftMotorSpeed;
		int rightMotorSpeed;
		int cutterSpeed;

    unsigned long overloadTime;
    unsigned long overloadInterval;

    int durationFromDistance (int speed, int distance) {
      long rescaledDistance = WHEELMOTOR_DISTANCE_FACTOR * distance;
      return rescaledDistance / speed;
    }

};

#endif /* _CONTROLLER_H_ */
