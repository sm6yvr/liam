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
BATTERY::BATTERY(BATTERY_TYPE type, int socpin, int dockpin) {
    batType = type;
    batSocpin = socpin;
    batDockpin = dockpin;

}


// set the level when battery is concidered fully charged
void BATTERY::setFullyChargedLevel(int level) {
   fullyChargedLevel = level;
   }

void BATTERY::setBatterType(BATTERY::BATTERY_TYPE type)
{
  this->batType = type;
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
  averageSOC=readBatteryAndCalcValue();
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
    return word(newReading);
}

bool BATTERY::isBeingCharged() {
	return digitalRead(batDockpin);
}

bool BATTERY::isFullyCharged() {
bool value=false;
value = (readBatteryAndCalcValue() > fullyChargedLevel);
if(value)
{
numbersOfFullyRead++;
Serial.print(";501:");
Serial.print(readBatteryAndCalcValue(),DEC);
Serial.print(":");
Serial.println(numbersOfFullyRead);

if(numbersOfFullyRead > 10)
{
  value = true;
}
// 300 times needs to be read fullyCharged before leaveing.
}
else
numbersOfFullyRead = 0;

return value;
}
void BATTERY::setGoHomeLevel(int level)
{
  this->gohomeLevel=level;
}
int BATTERY::getGoHomeLevel()
{
  return this->gohomeLevel;
}
