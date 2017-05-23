// This is the library for an Error
//
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/
#include <Arduino.h>
#include <Wire.h>  // For LCD
#include "myLcd.h"
#include "Controller.h"

#ifndef _ERROR_H_
#define _ERROR_H_

class ERROR {
    public:
        ERROR(MYDISPLAY* display_, int led_pin_, CONTROLLER* Mower_);
        
        void flag(int error_number);

    private:
    MYDISPLAY* mylcd;
    CONTROLLER* Mower;
    int led_pin;
    
    void blink_led(int error_number);
};

#endif /* _ERROR_H_ */
