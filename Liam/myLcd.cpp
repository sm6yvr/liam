

/// This is the library for an LCD display
//
// Changelog:
//     2014-12-12 - Initial version by Timo

/* ============================================
Placed under the GNU license

===============================================
*/

#include "myLcd.h"
#include <LCD03.h>
#include "Sens9150.h"

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
  MS9150 Compass;
  //lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
  lcd2.begin(20,4);
  //lcd.noBacklight();
  lcd2.noBacklight();
  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i < 3; i++)
  {
    //lcd.backlight();
    lcd2.backlight();
    delay(100);
    //lcd.noBacklight();
    lcd2.noBacklight();
    delay(100);
  }
  lcd.backlight(); // finish with backlight on
  lcd2.backlight();

  lcd2.setCursor(0, 0); 
  lcd2.print("SW version:");
  lcd2.setCursor(0, 1); 
    sprintf (buffer, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION_1, MINOR_VERSION_2);
    lcd2.print(buffer);
  lcd2.setCursor(0, 2);
  lcd2.print(__DATE__);
  lcd2.setCursor(0, 3);
  lcd2.print(__TIME__);
  delay(3000);
  lcd2.clear();

}

void myLCD::setCursor(int col, int row) {
      lcd2.setCursor(col, row);
    }
    
size_t myLCD::write(uint8_t s) {
      lcd2.print(char(s));
    }

void myLCD::clear() {
      lcd2.clear();
    }
