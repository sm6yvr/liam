/*
 Liam - DIY Robot Lawn Mower

 Clock Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Clock.h"
#include "Definition.h"

CLOCK::CLOCK(uint8_t outHour, uint8_t outMinute, uint8_t inHour, uint8_t inMinute) {
  outTimeHour = outHour;
  outTimeMinutes = outMinute;
  inTimeHour = outHour;
  inTimeMinutes = outMinute;

  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
}

void CLOCK::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC.adjust(DateTime(year, month, day, hours, minutes, seconds));
}

void CLOCK::setGoOutTime(uint8_t Hour, uint8_t Minutes) {
  outTimeHour = Hour;
  outTimeMinutes = Minutes;
}

void CLOCK::setGoHomeTime(uint8_t Hour, uint8_t Minutes) {
  inTimeHour = Hour;
  inTimeMinutes = Minutes;
}

bool CLOCK::timeToCut() {
  if ((int)RTC.now().hour() * 60 + (int)RTC.now().minute() > (int)outTimeHour * 60 + (int)outTimeMinutes &&
      (int)RTC.now().hour() * 60 + (int)RTC.now().minute() < (int)inTimeHour * 60 + (int)inTimeMinutes)
    return true;

  return false;
}

void CLOCK::printTime() {
  DateTime now = RTC.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
