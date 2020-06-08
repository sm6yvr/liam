/*
 Liam - DIY Robot Lawn Mower

 Error Handler Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include <Arduino.h>
#include <Wire.h>  // For LCD
#include "MyDisplay.h"
#include "Controller.h"

#ifndef _ERROR_H_
#define _ERROR_H_

#define ERROR_OUTSIDE 1
#define ERROR_OVERLOAD 2
#define ERROR_TILT 3
#define ERROR_LIFT 4
#define ERROR_BUMPERSTUCK 5


class ERROR {
  public:
    ERROR(MYDISPLAY* display_, int led_pin_, CONTROLLER* Mower_);

    String errorMessage(int error_number);
    void flag(int error_number);

  private:
    MYDISPLAY* display;
    CONTROLLER* Mower;
    int led_pin;

};

#endif /* _ERROR_H_ */
