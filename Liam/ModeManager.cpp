#include "ModeManager.h"
#include "Definition.h"

MODEMANAGER::MODEMANAGER(CONTROLLER* controller, BWFSENSOR* bwf, BATTERY* batt, SETUPDEBUG* setup) {
  mower = controller;
  sensor = bwf;
  battery = batt;
  setupDebug = setup;
  currentOpState =  CutterModes::BOOTING;
}

CutterModes MODEMANAGER::getCurrentMode() {
  return currentOpState;
}

bool MODEMANAGER::CurrentModeAllowsLaunch() {
  return currentOpState ==  CutterModes::MOW_REPEAT || currentOpState == CutterModes::MOW_ONCE;
}

bool MODEMANAGER::setMode(CutterModes opState) {

  if (opState == currentOpState) return false;



  currentOpState = opState;
  Serial.print(F("New mode: "));
  Serial.println(getModeText(currentOpState));

  switch (currentOpState) {
    case CutterModes::MOW_REPEAT:
    case CutterModes::MOW_ONCE:
      state = battery->isBeingCharged() ? CHARGING : MOWING;
      sensor->logAllSensorChanges = false;
    return true;
    case CutterModes::CHARGE_ONLY:
      state = battery->isBeingCharged() ? CHARGING : LOOKING_FOR_BWF;
      sensor->logAllSensorChanges = false;
    return true;
    case CutterModes::BOOTING:
    case CutterModes::IDLE:
    case CutterModes::HOLD_FOR_ERROR:
    return true;
  }

  sensor->logAllSensorChanges = true;

  if (opState == BOOTING) return true;

  setupDebug->InitializeDebugMode();
  state = SETUP_DEBUG;
  return true;
}

bool MODEMANAGER::tryChangeMode() {
  if (currentOpState != IDLE && Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == 'd' || inChar == 'D') {
      Serial.println("Setting idle");
      setMode(CutterModes::IDLE);
      return true;
    }

    if (inChar == 'm' || inChar == 'M') {
      setMode(CutterModes::MOW_REPEAT);
      return true;
    }

    if (inChar == 'o' || inChar == 'O') {
      setMode(CutterModes::MOW_ONCE);
      return true;
    }

    if (inChar == 'c' || inChar == 'C') {
      setMode(CutterModes::CHARGE_ONLY);
      return true;
    }
  }
  return false;
}

String MODEMANAGER::getModeText(CutterModes mode){
  switch (mode) {
    case MOW_ONCE:
      return F("MOW ONCE");
    case MOW_REPEAT:
      return F("MOW");
    case CHARGE_ONLY:
      return F("CHARGE");
    case IDLE:
      return F("IDLE");
    case HOLD_FOR_ERROR:
      return F("ERROR");
    default:
      return "UNKNOWN" + mode;
  }
}
