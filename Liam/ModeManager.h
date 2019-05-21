#include <Arduino.h>
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "Controller.h"
#include "SetupDebug.h"

#ifndef _MODEMANAGER_H_
#define _MODEMANAGER_H_

class MODEMANAGER {
  public:
    MODEMANAGER(CONTROLLER* controller, BWFSENSOR* bwf, BATTERY* batt, SETUPDEBUG* setup) ;
    bool tryChangeMode();
    bool setMode(CutterModes opState);
    CutterModes getCurrentMode();
    bool CurrentModeAllowsLaunch();
    String getModeText(CutterModes mode);
  private:
    CONTROLLER* mower;
    BWFSENSOR* sensor;
    BATTERY* battery;
    SETUPDEBUG* setupDebug;
    CutterModes currentOpState;
    int state;
};

#endif
