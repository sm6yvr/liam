/// This is the library for an LCD display
//
// Changelog:
//     2014-12-12 - Initial version by Timo

/* ============================================
Placed under the GNU license

===============================================
*/

#include "myLcd.h"


myLCD::myLCD(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state): MYDISPLAY(batt, left, right, cut, bwf, comp, state), lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)
{
	Battery = batt;
	leftMotor = left;
    rightMotor = right;
    cutter = cut;
    sensor = bwf;
	compass = comp;
	moverstate = state;
}


boolean myLCD::initialize()
{
	char buffer [9]; //Format 09.00.00

	lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
	lcd.noBacklight();
	// ------- Quick 3 blinks of backlight  -------------
	for(int i = 0; i < 3; i++)
	{
		lcd.backlight();
		delay(100);
		lcd.noBacklight();
		delay(100);
	}
	lcd.backlight(); // finish with backlight on

	lcd.setCursor(0, 0); 
	lcd.print("SW version:");
	lcd.setCursor(0, 1); 
  	sprintf (buffer, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION_1, MINOR_VERSION_2);
  	lcd.print(buffer);
	lcd.setCursor(0, 2);
	lcd.print(__DATE__);
	lcd.setCursor(0, 3);
	lcd.print(__TIME__);
	delay(3000);
	lcd.clear();

}

void myLCD::setCursor(int col, int row) {
    	lcd.setCursor(col, row);
    }
    
size_t myLCD::write(uint8_t s) {
    	lcd.write(s);
    }

void myLCD::clear() {
    	lcd.clear();
    }
