/* This is the library for a Cutter Motor
//
// Changelog:
//     2014-12-13 - Initial version by Jonas

============================================
Placed under the GNU license

===============================================
*/

#ifndef _CUTTERMOTOR_H_
#define _CUTTERMOTOR_H_

#include <Servo.h>
#include <Arduino.h>

class CUTTERMOTOR {
    public:
        CUTTERMOTOR(int type_, int pwmpin_, int loadpin_);
        
        // 
        void initialize();
        void setSpeed(int setspeed);
        int getSpeed();
        
        int getLoad(); 
        
        void brake();
        bool isBraking();

		bool isBrushless();
		bool isOverloaded();
		void setOverloadLevel(int level);

    private:
    	int pwmpin;
    	int pwm;
    	int type;
    	int brakepin;
    	bool braking;
    	int loadpin;
    	int load;
    	int speed;
    	int overload_level;
    	Servo cutter;
};

#endif /* _CUTTERMOTOR_H_ */
