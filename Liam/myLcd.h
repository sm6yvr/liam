

// This is the library for an LCD display
//
// Changelog:
//     2014-12-12 - Initial version by Timo

/* ============================================
Placed under the GNU license

===============================================
*/
#include <Arduino.h>
#include <Wire.h>  // For LCD
#include <LiquidCrystal_I2C.h>
#include <LCD03.h>
#include "Definition.h"
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Battery.h"
#include "MyDisplay.h"

#ifndef _MYLCD_H_
#define _MYLCD_H_

class myLCD : public MYDISPLAY {
    public:
        myLCD(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state);
    
        virtual boolean initialize();
        virtual void setCursor(int col, int row);
        virtual size_t write(uint8_t);
        virtual void clear();

    private:
      LiquidCrystal_I2C lcd;
      //LCD03 lcd;
      LCD03 lcd2;
};

#endif /* _MYLCD_H_ */
