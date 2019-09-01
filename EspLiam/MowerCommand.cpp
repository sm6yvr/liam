#include "MowerCommand.h"
#include <ArduinoJson.h>

MowerCommand::MowerCommand() {
  isValid = false;
}

MowerCommand::MowerCommand(int newMode) {
  this->newMode = newMode;
  isValid = true;
}

int MowerCommand::getNewMode() {
  return this->newMode;
}

char * MowerCommand::toJson() {
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;
  jsonDoc["newMode"] = this->newMode;

  char output[300];

  serializeJson(jsonDoc, output, 300);

  char * buf = (char *) malloc (300);
  strcpy (buf, output);
  return buf;
  
}

void MowerCommand::fromJson(char * input) {

  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;
  DeserializationError err = deserializeJson(jsonDoc, input);

  if (err) {
    isValid = false;
  } else {
    this->newMode = jsonDoc["newMode"];
    isValid = true;
  }

}
