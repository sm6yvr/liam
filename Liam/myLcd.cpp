/*
 Liam - DIY Robot Lawn Mower

 LCD Display Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "myLcd.h"
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>
//LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

  myLCD::myLCD(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state) :
  MYDISPLAY(batt, left, right, cut, bwf, comp, state),
  //MYDISPLAY(batt, left, right, cut, bwf, comp),
  // You may need to modify this line to work with your LCD controller
  //lcd(LCD_I2C_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)
  lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)
  //lcd(LCD_I2C_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, 0)
{
}

boolean myLCD::initialize()
{
  current_row = current_col = 0;
  Serial.println("-----Init LCD-----");
  lcd.begin(max_cols, max_rows);

  // Hide cursor and turn on backlight
  lcd.noCursor();
  lcd.backlight();
  lcd.print("Hello");
  return MYDISPLAY::initialize();
}


size_t myLCD::write(uint8_t s)
{
  //current_col++;

  if(s == '\n' || current_col >= max_cols)
  {
    current_row++;
    current_col = 0;
    if(current_row >= max_rows)
      current_row = 0;
    lcd.setCursor(current_col, current_row);
  }

  lcd.write(s);

  // Uncomment to write to serial port also
  /* MYDISPLAY::write(s); */
}

void myLCD::setCursor(int col, int row)
{
  current_row = row;
  current_col = col;
  lcd.setCursor(col, row);
}

void myLCD::clear()
{
  lcd.clear();
  setCursor(0,0);
}

void myLCD::blink()
{
  // Flash backlight
  lcd.backlight();
  delay(100);
  lcd.noBacklight();
  delay(100);
  lcd.backlight();
}
