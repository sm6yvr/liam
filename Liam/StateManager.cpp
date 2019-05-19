#include "StateManager.h"
#include "Definition.h"

STATEMANAGER::STATEMANAGER(CONTROLLER* controller, BWFSENSOR* bwf, BATTERY* batt, SETUPDEBUG* setup) {
  mower = controller;
  sensor = bwf;
  battery = batt;
  setupDebug = setup;
  currentOpState =  OperationStates::BOOTING;
}

OperationStates STATEMANAGER::getCurrentOperationalState() {
  return currentOpState;
}
void STATEMANAGER::setOperationState(OperationStates opState) {

  if (opState == currentOpState) return;



  currentOpState = opState;
  Serial.print(F("New state: "));
  Serial.println(currentOpState);

  switch (currentOpState) {
    case OperationStates::MOW:
    case OperationStates::MOW_ONCE:
      state = battery->isBeingCharged() ? CHARGING : MOWING;
      sensor->logAllSensorChanges = false;
    return;
    case OperationStates::CHARGE:
      state = battery->isBeingCharged() ? CHARGING : LOOKING_FOR_BWF;
      sensor->logAllSensorChanges = false;
    return;
    case OperationStates::BOOTING:
    return;
    case OperationStates::IDLE:
    return;
  }

  sensor->logAllSensorChanges = true;

  if (opState == BOOTING) return;

  setupDebug->InitializeDebugMode();
  state = SETUP_DEBUG;
}

void STATEMANAGER::tryChangeOperationState() {
  if (currentOpState != IDLE && Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == 'd' || inChar == 'D') {
      setOperationState(OperationStates::IDLE);
      return;
    }

    if (inChar == 'm' || inChar == 'M') {
      setOperationState(OperationStates::MOW);
      return;
    }

    if (inChar == 'o' || inChar == 'O') {
      setOperationState(OperationStates::MOW_ONCE);
      return;
    }

    if (inChar == 'c' || inChar == 'C') {
      setOperationState(OperationStates::CHARGE);
      return;
    }
  }
}
