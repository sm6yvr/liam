/// This is the library for an Error handler
//
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#include "Error.h"

/** Specific constructor.
 */
ERROR::ERROR(MYDISPLAY* display_, int led_pin_, CONTROLLER* Mower_) {
 mylcd = display_;
 led_pin = led_pin_;
 Mower = Mower_;
}

void ERROR::flag(int error_number_) {
	
  Mower->stopCutter();
  Mower->stop();

  mylcd->setCursor(5,0);
  mylcd->print("Error!!");
  switch (error_number_) {
  case 1:
    mylcd->setCursor(0,1);
    mylcd->print("Mower is");
    mylcd->setCursor(0,2);
    mylcd->print("outside");
    mylcd->setCursor(0,3);
    mylcd->print("Boundaries");
    break;
  case 2:
    mylcd->setCursor(0,1);
    mylcd->print("No Signal");
    mylcd->setCursor(0,2);
    mylcd->print("From BFW");
    mylcd->setCursor(0,3);
    mylcd->print("");
    break;
  case 3:
    mylcd->setCursor(0,1);
    mylcd->print("Overloaded right");
    mylcd->setCursor(0,2);
    mylcd->print("motor while docking");
    break;
  case 4:
    mylcd->setCursor(0,1);
    mylcd->print("Overloaded left");
    mylcd->setCursor(0,2);
    mylcd->print("motor while docking");
    break;
  case 5:
    mylcd->setCursor(0,1);
    mylcd->print("Failed to align");
    mylcd->setCursor(0,2);
    mylcd->print("perpend. left to");
    mylcd->setCursor(0,3);
    mylcd->print("wire while tracking");
    break;
  case 6:
    mylcd->setCursor(0,1);
    mylcd->print("Failed to align");
    mylcd->setCursor(0,2);
    mylcd->print("perpend. right to");
    mylcd->setCursor(0,3);
    mylcd->print("wire while tracking");
    break;
  case 7:
    mylcd->setCursor(0,1);
    mylcd->print("Left sensor");
    mylcd->setCursor(0,2);
    mylcd->print("triggered while");
    mylcd->setCursor(0,3);
    mylcd->print("WaitAndCheck");
    break;
  case 8:
    mylcd->setCursor(0,1);
    mylcd->print("Right sensor");
    mylcd->setCursor(0,2);
    mylcd->print("triggered while");
    mylcd->setCursor(0,3);
    mylcd->print("WaitAndCheck");
    break;
  case 9:
    mylcd->setCursor(0,2);
    mylcd->print("Mower tilted");
    break;  
  } 

  // blink LED forever
  while (true) 
	blink_led(error_number_);
}


void ERROR::blink_led(int error_number_){
	for (int i=0; i<error_number_;i++) {
		digitalWrite(led_pin, HIGH);
		delay(500);
		digitalWrite(led_pin, LOW);
		delay(500);
	}
	delay (2000);
}
