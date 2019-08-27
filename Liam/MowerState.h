#include <Arduino.h>
#include <ArduinoJson.h>
#include "Battery.h"
#include "ModeManager.h"
#include "BWFSensor.h"
#include "Error.h"

#define CONFIG_JSON_SIZE 100

class MOWERSTATE {
  public:
    MOWERSTATE();
    MOWERSTATE(BATTERY* battery_, MODEMANAGER* modeManager_, BWFSENSOR* bwfSensor_, WHEELMOTOR* leftMotor_, WHEELMOTOR* rightMotor, ERROR* error);

    int getMode();
    void setLooptime(float looptime);
    JsonDocument toJson();
    void fromJson(char * jsonInput);

  private:
    float looptime;
    StaticJsonDocument<CONFIG_JSON_SIZE> jsonDoc;
    BATTERY* battery;
    MODEMANAGER* modeManager;
    BWFSENSOR* bwfSensor;
    WHEELMOTOR* leftMotor;
    WHEELMOTOR* rightMotor;
    ERROR* error;
};