#ifndef _API_H
#define _API_H
#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "EEPROM.h"


#define SERIALCOMMANDDEBUG
#undef SERIALCOMMANDDEBUG
class API
{
public:

  enum API_COMMAND {
    OK=0,
    GetSetUpDebug=100,
    SetSetUpDebug=101,
    GetBattery=102,
    SetBattery=103,
    GetLeftSensor=104,
    GetRightSensor=105,
    SetFirstByteToFalse=998,
    INVALID = 999
  };

  const char* API_COMMAND_PRINT_NAME(API_COMMAND command)
  {
    switch (command) {
      case OK:
      return "OK";
      case GetSetUpDebug:
      return "Get setup and debug";
      case SetSetUpDebug:
      return "Set setup and debug";
      case GetBattery:
      return "Get Battery";
      case SetBattery:
      return "Set Battery";
      case INVALID:
      return "INVALID";
      default:
      return "INVALID";
    }
  }

  API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition);

  void SetFirstByteFalse();
  void ValidateCommand(); // entrypoint
  bool addByteToBuffer(char c);

  bool inputComplete = false;  // whether the string is complete
  void EEPROM_READ();
  void EEPROM_WRITE();
  bool IsWrittenToEEPROM();
private:

  WHEELMOTOR* leftMotor;
  WHEELMOTOR* rightMotor;
  CUTTERMOTOR* cutter;
  BWFSENSOR* sensor;
  MOTIONSENSOR* compass;
  BATTERY* battery;
  DEFINITION *definitionDefaults;

  static const short bufferlenght = 40;
  static const short argslength = 4;
  static const short templenght = 4;
  char buffer[bufferlenght];         // Char to hold incoming data
  char term = '\r'; // every command ends with carrier return
  char syncValue = ';'; // every command must start with syncValue
  char delimit = ':';   // string must be delimited by this value.

  char temp[templenght];
  bool CheckSyncValue();
  bool CheckArgs();
  bool CheckCommand();
  bool setCommand();
  bool CheckArgNumber(const short &Number);
  void WeAreDone();
  void clearBuffer();
  void clearTemp();
  void clearBufferUpToIndex();
  void clear_args();
  void Response_Invalid_Command();
  void printIndex();

  void leave();
  /* Response for commands */

  void ActRespond();

  void RespondGetSetUpDebug();
  bool ACT_SetUpDebug();

  void Response_GetBattery();
  void ACT_SetBattery();

  void ACT_GetSensor();
  bool RespondSetSetUpDebug(bool &value);

  int SearchForChar(char *c);
  short bufPos =0;
  API_COMMAND commandIndex=INVALID;
  int argument[argslength];
  short argscounter=0;

  int index;
  char *c;
};
#endif
// Skapa en enum Response ok, INVALID, Badly formatted
//Commad type == Command (mot annan) == Vi behöver informartion. Response, subscribe, notify
//
