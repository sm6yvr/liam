#ifndef _API_H
#define _API_H
#include <Arduino.h>
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "MotionSensor.h"
#include "Definition.h"
#include "EEPROM.h"
#include "Controller.h"


#define SERIALCOMMANDDEBUG
#undef SERIALCOMMANDDEBUG
class API
{
public:

  /* API response mellan 300 och --> */
enum API_RESPONSE {
  OK=300,
  INVALID=301,
  HEARTBEAT=302,
  NOTIFY=303,
  SUBSCRIBE=304,
  DEBUG=305,
  ONLINE=306
};
/* get commands are even, set are odd */
enum API_COMMAND {
  GetCommands=0,
  GetCutterState=2,
  GetState=10,
  GetSetUpDebug=100,
  GetBattery=102,
  GetSensor=104,
  GetMotor=106, // ;106:MOTOR[0=left,1=Right,2=cutter]#
  GetSlowWheelWhenDocking=108,
  GetCutterStatus=110,

  SetState=11,
  SetSetUpDebug=101,
  SetBattery=103,
  SetMotor=105, //;105:MOTOR[0=left,1=Right,2=cutter]:SPEED# speed -100 --> 100
  SetSlowWheelWhenDocking=109,

  SetFirstByteToFalse=998
};
/*Vet inte om man skulle kunna använda denna enum för att ge rätt kommandon till den som lyssnar på seriesnöret */
  const char* API_COMMAND_PRINT_NAME(API_COMMAND command)
  {
    switch (command) {
      case GetCommands:
      return "GET Commands";
      case SetState:
      return "Set State";
      case GetState:
      return "Get State";
      case GetSetUpDebug:
      return "Get setup and debug";
      case SetSetUpDebug:
      return "Set setup and debug";
      case GetBattery:
      return "Get Battery";
      case SetBattery:
      return "Set Battery";
      case SetMotor:
      return "Set Motor";
      case GetSensor:
      return "Get sensor";
      case GetCutterStatus:
      return "Cutter informartion";
      case GetSlowWheelWhenDocking:
      return "Get slow wheel when docking";
      case SetSlowWheelWhenDocking:
      return "Set slow wheel when docking (%)";
      default:
      return "INVALID";
    }
  }

  API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition, int *state, CONTROLLER *controller);

  void SetFirstByteFalse();
  void ValidateCommand(); // entrypoint
  bool addByteToBuffer(char c);

  bool inputComplete = false;  // whether the string is complete
  void EEPROM_READ();
  void EEPROM_WRITE();
  bool IsWrittenToEEPROM();
  void updatetwo(int nr,int looptime);
  void update(int looptime);
  void Debug(char *value);
  bool get_StateHasBeenChanged();
  void ResetStateHasBeenChanged();
  bool get_ApiDebug();
  void set_ApiDebug(bool value);

  void get_CutterStates();
  void sendHeartBeat();

private:

  WHEELMOTOR* leftMotor;
  WHEELMOTOR* rightMotor;
  CUTTERMOTOR* cutter;
  BWFSENSOR* sensor;
  MOTIONSENSOR* compass;
  BATTERY* battery;
  DEFINITION *definitionDefaults;
  CONTROLLER *ApiController;

  static const short bufferlenght = 64;
  static const short argslength = 4;
  static const short templenght = 4;
  char buffer[bufferlenght]; // Char to hold incoming data
  char term = '\r'; // every command ends with carrier return
  char syncValue = ';'; // every command must start with syncValue
  char endcommand = '#'; // Every Command Must end with this char.
  char delimit = ':';   // string must be delimited by this value.

  char temp[templenght];
  int argument[argslength]={-1}; // init args to -1
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

  int getArgument(int number);

  void ActRespond(); // commando tolkning klar..

  void RespondGetSetUpDebug();
  bool ACT_SetUpDebug();

  void Response_GetBattery();
  void ACT_SetBattery();

  void Response_GetState();
  void ACT_SetState();

  void ACT_GetSensor();
  bool RespondSetSetUpDebug(bool &value);

  void ACT_SetSlowWheelWhenDocking();
  void Respond_GetSlowWheelWhenDocking();

  void Response_get_MotorStatus();
  void ACT_set_Motor();

  void sendEndCommand();
  void sendOkResponse();
  int SearchForChar(char *c);
  short bufPos =0;
  API_COMMAND commandIndex=API::API_COMMAND::SetMotor;

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
