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
  /* API response/Status */
enum API_STATUS_CODE {
  OK=1,
  ERROR=2,
  ARGUMENT_FAILED=3
};

enum API_COMMAND {
  /* Notify 0-->99 */
  HEARTBEAT=4,
  NOTIFY=10,
  SUBSCRIBE=11,
  DEBUG=90,
  ONLINE=99,
  /* GetCommands 100-->199 */
  GetCommands=100,
  GetMowerStatus=101,
  GetState=102,
  GetBattery=103,
  GetSensor=104,
  GetMotor=105, // ;105:MOTOR[0=left,1=Right,2=cutter]#
  GetSlowWheelWhenDocking=106,
  GetWheelOverloadLevel = 107,

/* Set commands 200-->299 */
  SetState=202,   /* state */
  SetBattery=203, /* battype,min,max,gohome */
  SetMotor=205, //;205:MOTOR[0=left,1=Right,2=cutter]:SPEED# speed -100 --> 100
  SetSlowWheelWhenDocking=206, /* value */
  SetWheelOverloadLevel = 207, /* value */

  SetFirstByteToFalse=998 /* <;998# > */
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
      case GetBattery:
      return "Get Battery";
      case SetBattery:
      return "Set Battery";
      case SetMotor:
      return "Set Motor";
      case GetSensor:
      return "Get sensor";
      case GetSlowWheelWhenDocking:
      return "Get slow wheel when docking";
      case SetSlowWheelWhenDocking:
      return "Set slow wheel when docking (%)";
      case SetWheelOverloadLevel:
      return "Set Wheel overload level";
      case GetWheelOverloadLevel:
      return "Get Wheel overload level";
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
  void Debug(char *value);
  bool get_StateHasBeenChanged();
  void ResetStateHasBeenChanged();
  bool get_ApiDebug();
  void set_ApiDebug(bool value);
  int *errorNumber;
  void get_CutterStates();
  void sendHeartBeat();
  int looptime;

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
  void Response_Invalid_Argument();
  void Response_Command_OK();
  void Init_Response();
  void Response_add_value(int value);

  void printIndex();

  void leave();
  /* Response for commands */

  int getArgument(int number);

  void ActRespond(); // commando tolkning klar..

  void Response_GetBattery();
  void ACT_SetBattery();

  void Response_GetState();
  void ACT_SetState();

  void ACT_GetSensor();

  void ACT_SetSlowWheelWhenDocking();
  void Respond_GetSlowWheelWhenDocking();

  void Response_get_MotorStatus();
  void ACT_set_Motor();

  void sendEndCommand();

  void Respond_get_MowerStatus();

  void Response_get_WheelOverloadlevel();
  void ACT_set_WheelOverloadlevel();
  int SearchForChar(char *c);
  short bufPos =0;
  API_COMMAND commandIndex=API::API_COMMAND::SetMotor;


  int index;
  char *c;
  int *mainState;
  bool stateChanged=false;
  bool apidebug=true;
  bool endCommandFound=false;
};
#endif
// Skapa en enum Response ok, INVALID, Badly formatted
//Commad type == Command (mot annan) == Vi behöver informartion. Response, subscribe, notify
//
