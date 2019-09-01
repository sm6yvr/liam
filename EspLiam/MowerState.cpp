#include "MowerState.h"
#include <ArduinoJson.h>

MowerState::MowerState() {
  isValid = false;
}

MowerState::MowerState(int mode, int batteryVoltage, bool isLeftOutside) {
  this->mode = mode;
  this->batteryVoltage= batteryVoltage;
  this->isLeftOutside = isLeftOutside;
  isValid = true;
}

int MowerState::getMode() {
  return this->mode;
}

char * MowerState::toJson() {
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;
  jsonDoc["mode"] = this->mode;
  jsonDoc["batteryVoltage"] = this->batteryVoltage;
  jsonDoc["isLeftOutside"] = this->isLeftOutside;

  char output[300];

  serializeJson(jsonDoc, output, 300);

  char * buf = (char *) malloc (300);
  strcpy (buf, output);
  return buf;
  
}

void MowerState::fromJson(char * input) {

  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;
  DeserializationError err = deserializeJson(jsonDoc, input);

  if (err) {
    isValid = false;
  } else {
    this->mode = jsonDoc["mode"];
    this->batteryVoltage = jsonDoc["batteryVoltage"];
    this->isLeftOutside = jsonDoc["isLeftOutside"];
    isValid = true;
  }

}
