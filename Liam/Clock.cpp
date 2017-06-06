/// This is the library for a Clock
//
// Changelog:

/* ============================================
Placed under the GNU license

===============================================
*/

#include "Clock.h"
#include "Definition.h"

void CLOCK::initialize() {
	
	Wire.begin();
    RTC.begin();
    
	if (! RTC.isrunning()) {
    	Serial.println("RTC is NOT running!");
    	// following line sets the RTC to the date & time this sketch was compiled
    	RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
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



