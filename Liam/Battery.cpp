/*
 Liam - DIY Robot Lawn Mower

 Battery Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Battery.h"
#include "Definition.h"

BATTERY::BATTERY(int type, int sensepin, int dockpin) {
  batType = type;
  batSensePin = sensepin;
  batDockPin = dockpin;

  // Battery types are defined in Definition.h
  // LIION
  // NIMH
  // LEAD_ACID

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


int BATTERY::getBatteryType() {
  return batType;
}


// Set the voltage at which battery is considered fully charged (mV)
void BATTERY::setFullyChargedLevel(int level) {
  fullyChargedLevel = level;
}

int BATTERY::getFullyChargedLevel() {
  return fullyChargedLevel;
}


// Set the voltage at which battery is considered depleted (mV)
void BATTERY::setDepletedLevel(int level) {
  depletedLevel = level;
}

int BATTERY::getDepletedLevel() {
  return depletedLevel;
}


bool BATTERY::mustCharge() {
  return (averageVoltage < depletedLevel);
}

bool BATTERY::isBeingCharged() {
  return digitalRead(batDockPin);
}

bool BATTERY::isFullyCharged() {
  return (readBatteryAndCalcValue() > fullyChargedLevel);
}


// Get battery voltage in mV (filtered through running average)
int BATTERY::getVoltage() {
  return averageVoltage/100;
}

void BATTERY::resetVoltage() {
  averageVoltage = readBatteryAndCalcValue();
}

// Take a battery reading and recalculate running average
void BATTERY::updateVoltage() {
  averageVoltage -= averageVoltage / FILTER;
  averageVoltage += readBatteryAndCalcValue() / FILTER;
}

// Measure battery voltage in mV
word BATTERY::readBatteryAndCalcValue(){
  unsigned long reading = analogRead(batSensePin);

  // Convert from ADC units to uV
  reading = reading * 4880;
  // Adjust for voltage divider circuit
  reading = (reading * VOLTDIVATOR) / 10;
  // Convert to mV
  reading = reading / 1000;

  return word(reading);
}
