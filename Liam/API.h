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
    NOTIFY=1,
    DEBUG=2,
    GetState=10,
    SetState=11,
    GetSetUpDebug=100,
    SetSetUpDebug=101,
    GetBattery=102,
    SetBattery=103,
    GetSensor=104,
    SetMotorOn=105,
    SetMotorOff=106,
    GetWheelMotor=107,
    GetSlowWheelWhenDocking=108,
    SetSlowWheelWhenDocking=109,
    GetCutterStatus=110,
    SetFirstByteToFalse=998,
    INVALID = 999
  };
/*Vet inte om man skulle kunna använda denna enum för att ge rätt kommandon till den som lyssnar på seriesnöret */
  const char* API_COMMAND_PRINT_NAME(API_COMMAND command)
  {
    switch (command) {
      case OK:
      return "OK";
      case SetState:
      return "SetState";
      case GetState:
      return "GetState";
      case GetSetUpDebug:
      return "Get setup and debug";
      case SetSetUpDebug:
      return "Set setup and debug";
      case GetBattery:
      return "Get Battery";
      case SetBattery:
      return "Set Battery";
      case SetMotorOn:
      return "Motor On";
      case SetMotorOff:
      return "Motor off";
      case GetSensor:
      return "Get sensor";
      case GetWheelMotor:
      return "Wheel informartion";
      case GetCutterStatus:
      return "Cutter informartion";
      case GetSlowWheelWhenDocking:
      return "Get slow wheel when docking";
      case SetSlowWheelWhenDocking:
      return "Set slow wheel when docking (%)";
      case INVALID:
      return "INVALID";
      default:
      return "INVALID";
    }
  }

  API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition, int *state);

  void SetFirstByteFalse();
  void ValidateCommand(); // entrypoint
  bool addByteToBuffer(char c);

  bool inputComplete = false;  // whether the string is complete
  void EEPROM_READ();
  void EEPROM_WRITE();
  bool IsWrittenToEEPROM();
  void update(int looptime);
  void Debug(char *value);
  bool get_StateHasBeenChanged();
  void ResetStateHasBeenChanged();
  bool get_ApiDebug();
  void set_ApiDebug(bool value);

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
  char buffer[bufferlenght]; // Char to hold incoming data
  char term = '\r'; // every command ends with carrier return
  char syncValue = ';'; // every command must start with syncValue
  char delimit = ':';   // string must be delimited by this value.

  char temp[templenght];
  int argument[argslength]={-1};
  short argscounter=0;

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

  void ACT_SetMotorOn();
  void ACT_SetMotorOff();

  void Response_GetState();
  void ACT_SetState();

  void ACT_GetSensor();
  bool RespondSetSetUpDebug(bool &value);

  void ACT_SetSlowWheelWhenDocking();
  void Respond_GetSlowWheelWhenDocking();


  int SearchForChar(char *c);
  short bufPos =0;
  API_COMMAND commandIndex=INVALID;

  int index;
  char *c;
  int *mainState;
  bool stateChanged=false;
  bool apidebug=true;
};
#endif
// Skapa en enum Response ok, INVALID, Badly formatted
//Commad type == Command (mot annan) == Vi behöver informartion. Response, subscribe, notify
//
