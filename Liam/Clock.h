/*
 Liam - DIY Robot Lawn Mower

 Clock Library

 ======================
  Licensed under GPLv3
 ======================
*/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"


class CLOCK {
  public:
    CLOCK(uint8_t outHour, uint8_t outMinute, uint8_t inHour, uint8_t inMinute);

    void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);
    void setGoOutTime(uint8_t Hour, uint8_t Minutes);
    void setGoHomeTime(uint8_t Hour, uint8_t Minutes);

    bool timeToCut();

    void printTime();

  private:
    RTC_DS1307 RTC;
    uint8_t outTimeHour;
    uint8_t outTimeMinutes;
    uint8_t inTimeHour;
    uint8_t inTimeMinutes;
};

#endif /* _CLOCK_H_ */
