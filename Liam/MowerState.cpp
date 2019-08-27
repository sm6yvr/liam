#include "MowerState.h"
#include <ArduinoJson.h>
#include "ModeManager.h"
#include "Wheelmotor.h"
#include "Error.h"

MOWERSTATE::MOWERSTATE() {};
MOWERSTATE::MOWERSTATE(BATTERY* battery_, MODEMANAGER* modeManager_, BWFSENSOR* bwfSensor_, WHEELMOTOR* leftMotor_, WHEELMOTOR* rightMotor_, ERROR* error_) {
  battery = battery_;
  modeManager = modeManager_;
  bwfSensor = bwfSensor_;
  leftMotor = leftMotor_;
  rightMotor = rightMotor_;
  error = error_;
}

void MOWERSTATE::setLooptime(float looptime_) {
  looptime = looptime_;
}

JsonDocument MOWERSTATE::toJson() {
     jsonDoc["mode"] = (int) modeManager->getCurrentMode();
     jsonDoc["battery"] = battery->getVoltage();
     //onDoc["isLeftOutside"] = !bwfSensor->isOutOfBounds(0);
     //sonDoc["isRightOutside"] = !bwfSensor->isOutOfBounds(1);
     //jsonDoc["leftMotorLoad"] = leftMotor->getLoad();
     //jsonDoc["rightMotorLoad"] = rightMotor->getLoad();
     jsonDoc["looptime"] = looptime;
     jsonDoc["errorMsg"] = error->getErrorMsg();
     
     return jsonDoc;
}

void MOWERSTATE::fromJson(char * input) {

  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;
  deserializeJson(jsonDoc, input);

}