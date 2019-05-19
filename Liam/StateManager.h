#include <Arduino.h>
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "Controller.h"
#include "SetupDebug.h"

#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

class STATEMANAGER {
  public:
    STATEMANAGER(CONTROLLER* controller, BWFSENSOR* bwf, BATTERY* batt, SETUPDEBUG* setup) ;
    void tryChangeOperationState();
    void setOperationState(OperationStates opState);
    OperationStates getCurrentOperationalState();
  private:
    CONTROLLER* mower;
    BWFSENSOR* sensor;
    BATTERY* battery;
    SETUPDEBUG* setupDebug;
    OperationStates currentOpState;
    int state;
};

#endif
