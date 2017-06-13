/// This is the library for a Battery
//
/* Changelog:
    2014-12-12 - Initial version by Jonas
    
	2015-06-10 - Morgan M
		- new function , readBatteryAndCalcValue
		- update for resetSOC
		- update for updateSOC
		- update for isFullyCharged
		
		
============================================
Placed under the GNU license

===============================================
*/

#include "Battery.h"

/** Specific constructor.
 */
BATTERY::BATTERY(int type, int socpin, int dockpin) {
    batType = type;
    batSocpin = socpin;
    batDockpin = dockpin;
    
	if (batType == LIION) {
		fullyChargedLevel = LIIONFULL;
		depletedLevel = LIIONEMPTY;
	}

	if (batType == NIMH) {
		fullyChargedLevel = NIMHFULL;
		depletedLevel = NIMHEMPTY;
	}

	if (batType == LEAD_ACID) {
		fullyChargedLevel = LEADACIDFULL;
		depletedLevel = LEADACIDEMPTY;
	}
    
}


// set the level when battery is concidered fully charged
void BATTERY::setFullyChargedLevel(int level) {
   fullyChargedLevel = level;
   }

int BATTERY::getBatteryType() {
	return batType;
}


int BATTERY::getFullyChargedLevel() {
	return fullyChargedLevel;
}


// set the level when battery is concidered depleted
void BATTERY::setDepletedLevel(int level) {
   depletedLevel = level;
   }

int BATTERY::getDepletedLevel() {
	return depletedLevel;
}

int BATTERY::getSOC() {
	return averageSOC;
}

void BATTERY::resetSOC() {
	averageSOC = readBatteryAndCalcValue();
}

bool BATTERY::mustCharge() {
	return (averageSOC < depletedLevel);
}


void BATTERY::updateSOC() {
	averageSOC = averageSOC - (averageSOC / FILTER) + (readBatteryAndCalcValue() / FILTER);
}


word BATTERY::readBatteryAndCalcValue(){
    unsigned long newReading = analogRead(batSocpin);	
    newReading = newReading * 488  * VOLTDIVATOR;
    newReading /= 10000;
    return newReading;
  	//return word(newReading);  
}

bool BATTERY::isBeingCharged() {
	return digitalRead(batDockpin);
}

bool BATTERY::isFullyCharged() {
	return (readBatteryAndCalcValue() > fullyChargedLevel);
}

