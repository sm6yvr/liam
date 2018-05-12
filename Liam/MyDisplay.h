// This is the library for a Display
// It will by default write to serial port, but can be replaced by subclasses for
// a built in LCD or OLED for example
//
// This code extends the Print class to get the print capabilities
// Basically the print class method calls each other until reaching the most basic method which is
// implemented here (write)
// Changelog:
//     2015-08-05 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#ifndef _MYDISPLAY_H_
#define _MYDISPLAY_H_

#include "Definition.h"
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Battery.h"

class MYDISPLAY : public Print {
    public:
        MYDISPLAY(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state);
        virtual boolean initialize();
        virtual void update();
        virtual void setCursor(int col, int row);
        virtual size_t write(uint8_t);
        virtual void clear();
		virtual void blink();

    protected:
    	int current_row;
    	int current_col;
   		BATTERY* Battery;
   		WHEELMOTOR* leftMotor;
    	WHEELMOTOR* rightMotor;
    	CUTTERMOTOR* cutter;
    	BWFSENSOR* sensor;
    	MOTIONSENSOR* compass;
    	int* moverstate;


};

#endif /* _MYDISPLAY_H_ */
