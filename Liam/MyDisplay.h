/*
 Liam - DIY Robot Lawn Mower

 Display Library

 ======================
  Licensed under GPLv3
 ======================
*/

/*
  Base class for displays.
  Default behavior is to print to serial port.

  This code extends the Print class to get the print
  capabilities. Basically the print class method calls each
  other until reaching the most basic method which is
  implemented here (write)
*/

#ifndef _MYDISPLAY_H_
#define _MYDISPLAY_H_

#include "Definition.h"
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Battery.h"

class MYDISPLAY : public Print
{
  public:
    MYDISPLAY(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state);
    virtual boolean initialize();
    void update();

    virtual size_t write(uint8_t);
    virtual void setCursor(int col, int row);
    virtual void clear();
    virtual void blink();

  protected:
    BATTERY* Battery;
    WHEELMOTOR* leftMotor;
    WHEELMOTOR* rightMotor;
    CUTTERMOTOR* cutter;
    BWFSENSOR* sensor;
    MOTIONSENSOR* compass;
    int* moverstate;
};

#endif /* _MYDISPLAY_H_ */
