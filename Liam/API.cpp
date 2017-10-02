#include "API.h"
#include "Arduino.h"
#include "Definition.h"
API::API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition, int *state, CONTROLLER *controller) {
  leftMotor = left;
  rightMotor = right;
  cutter = cut;
  sensor = bwf;
  compass = comp;
  battery = batt;
  definitionDefaults =definition;
  mainState = state;
  ApiController = controller;
}
void API::Init_Response()
{
  Serial.print(syncValue);
  Serial.print(commandIndex);

}
void API::Response_add_value(int value)
{
  Serial.print(delimit);
  Serial.print(value);
}
bool API::IsWrittenToEEPROM()
{
  bool written = eeprom_read_byte(0);
  return written;
}
void API::EEPROM_READ()
{
  eeprom_read_block((void*)definitionDefaults, (void*)1, sizeof(*definitionDefaults));
}
void API::EEPROM_WRITE()
{
  eeprom_write_block((const void*)definitionDefaults, (void*)1, sizeof(*definitionDefaults));
  eeprom_write_byte(0,true); // add true to first byte.. used for to identify if eepom is used in application or not. */
}

int API::SearchForChar(char *c)
{
  int pos=0;
  const char *ptr = strchr(buffer, *c);
  if(ptr) {
    pos = ptr - buffer;
    return pos;
  }
  return -1;

}

void API::ValidateCommand()
{
  endCommandFound=false;
  #ifdef SERIALCOMMANDDEBUG
  Serial.print(buffer);
  Serial.println(" is incomming buffer");
  #endif
  inputComplete=CheckSyncValue(); // start with check if buffer contains syncValue
  if(inputComplete) // if syncValue
  {
    inputComplete=CheckCommand();
  }
  else // if no syncvalue this will be run.
  {
    Serial.print("NO SYNC");
    Response_Invalid_Command();
    leave();
    return;
  }
  if(inputComplete) // if command
  {
    inputComplete = CheckArgs();
  }
  else // if invalid command
  {
    Serial.print("NO command");
    Response_Invalid_Command();
    leave();
    return;
  }
  if(inputComplete) // if args
  {
    WeAreDone();
    ActRespond();
  }
  else // if invalid args
  {
    Response_Invalid_Command();
  }
  leave();
}
void API::printIndex()
{
  #ifdef SERIALCOMMANDDEBUG
  Serial.print(" INDEX IS NOW ");
  Serial.print(index, DEC);
  Serial.print('\n');
  #endif
}
bool API::CheckSyncValue()
{
  // we now have a buffer that is terminated with '\r'
  //Check for syncValue;
  index = SearchForChar(&syncValue);
  if(index!=-1)
  {
    #ifdef SERIALCOMMANDDEBUG
    Serial.print("sync found!\n");
    #endif
    //index found, add 1 to index to read next char.
    index++;
    return true;
  }
  return false;
}
bool API::setCommand()
{
  commandIndex=(API::API_COMMAND)atoi(temp);
  if(commandIndex== -1)
  return false;
  return true;
}
bool API::CheckCommand()
{

  #ifdef SERIALCOMMANDDEBUG
  printIndex();
  #endif
  short counter=0;
  for (int i = index; i < bufferlenght; i++)
  {
    this->c = &buffer[i];
    #ifdef SERIALCOMMANDDEBUG
    Serial.print("CheckCommand, value that will be checked ");
    Serial.println(*c);
    #endif

    if (isalpha(*c))
    {
      #ifdef SERIALCOMMANDDEBUG
      Serial.print("LETTER FOUND IN COMMAND");
      #endif
      return false;
      // char is a letter.. not acceptable as command;
    }
    else if (isdigit(*c))
    {
      temp[counter]=buffer[i]; // add bufforposition to out temp.. might come more digits;
      counter++;
      continue;
    }
    else
    {
      if(*c == endcommand || *c== delimit) /* Command must have either endcommand <#> or delimit <:> as end. */
      {
        if(*c == endcommand)
        {
          endCommandFound = true; // no args...
        }
        #ifdef SERIALCOMMANDDEBUG
        Serial.println("Command done");
        #endif
        index += counter;
        return setCommand();
      }
      else
      {
        Serial.print("Hit borde man inte komma..");
        Serial.print(*c);
        Serial.println("Kolla tecken ovan");
        // response Invalid command.
        return false;
      }
    } // if not digit or alpha (else)
  } // for loop
return false;
} // CheckCommand

void API::clear_args()
{
  for (short i = 0; i < argslength; i++) {
    argument[i] =-1;
  }
}

bool API::CheckArgs()
{
  #ifdef SERIALCOMMANDDEBUG
  printIndex();
  #endif
  if(endCommandFound)
  return true;
  clearTemp();
  //Next char (index + 1) will hold first api_args, these must be digits.
  short counter=0;
  for (int i = index + 1; i < bufferlenght; i++)
  {
    this->c = &buffer[i];
    #ifdef SERIALCOMMANDDEBUG
    Serial.print("args, value to check ");
    Serial.print(*c);
    Serial.print("\n");
    #endif
    if (isalpha(*c))
    {
      #ifdef SERIALCOMMANDDEBUG
      Serial.print("char is a letter");
      #endif
      return false;
    } // if alpha
    else if (isdigit(*c))
    {
      temp[counter]=buffer[i]; // add bufforposition to out temp.. might come more digits;
      counter++;
      #ifdef SERIALCOMMANDDEBUG
      Serial.print("Temp is now == ");
      Serial.print(temp);
      Serial.print('\n');
      #endif
    } // if digit
    else if (*c == this->delimit)
    {
      argument[argscounter] = atoi(temp);
      #ifdef SERIALCOMMANDDEBUG
      Serial.print("Value of temp: ");
      Serial.print(atoi(temp));
      Serial.print('\n');
      Serial.print("Value of arg: ");
      Serial.print(argument[argscounter], DEC);
      Serial.print('\n');
      #endif
      argscounter++;
      clearTemp();
      counter=0;
    }
    else if (*c == this->endcommand)
    {/* all commmands must end with endcommand */
      argument[argscounter] = atoi(temp);

      #ifdef SERIALCOMMANDDEBUG
      Serial.println("End of command found");
      #endif
      return true;
      break;
    }
    else
    {
      // Debbuing purpose
      Serial.print("This value will fail");
      Serial.println(*c);
      return false;
    }
  } // For loop
} // check args...

void API::WeAreDone()
{
  #ifdef SERIALCOMMANDDEBUG
  Serial.print("Commad: ");
  Serial.print(API_COMMAND_PRINT_NAME(commandIndex));
  Serial.print('\n');
  Serial.print("Aguments");
  for (size_t i = 0; i < argslength; i++) {
    if(argument[i] == '\0')
    break;
    Serial.print(delimit);
    Serial.print(argument[i],DEC);
  }
  sendEndCommand();
  #endif
}
void API::Response_Invalid_Argument()
{
  Init_Response();
  Serial.print(delimit);
  Serial.print(API::API_STATUS_CODE::ARGUMENT_FAILED);
  sendEndCommand();

}

void API::Response_Invalid_Command()
{
  #ifdef SERIALCOMMANDDEBUG
  Serial.println("Något gick fel");
  #else
  Init_Response();
Serial.print(delimit);
  Serial.print(API::API_STATUS_CODE::ERROR);
  sendEndCommand();
  #endif
}

bool API::addByteToBuffer(char c)
{
  if (c == '\r' || c == '\n') // Check for terminator (default '\r') = end of command
  return true; // No need to save term to buffer.

  if (isprint(c))   // Only printable characters into the buffer
  {
    if(bufPos>=bufferlenght)
    {
      Serial.println("buffer full.. clearing\n");
      clearBuffer(); // buffer is full clear it.
    }
    buffer[bufPos]=c; // Put character into buffer
    bufPos++; // add 1 to position
  }// if isprint
  return false;
}
bool API::CheckArgNumber(const short &number)
{
  int i = argument[number];
  if(i != -1)
  return true;
  else
  return false;
}
void API::clearBuffer()
{
  for (int i=0; i<=bufferlenght; i++)
  {
    buffer[i]='\0';
  }
  bufPos=0;
  inputComplete=false;
}
void API::Response_GetBattery()
{
  /* Return sync, kommand, TYPE, MIN, MAX, GOHOME SOC */
Init_Response();

Response_add_value(battery->getBatteryType());
Response_add_value(battery->getDepletedLevel());
Response_add_value(battery->getFullyChargedLevel());
Response_add_value(battery->getGoHomeLevel());
Response_add_value(battery->getSOC());
sendEndCommand();
}
void API::ACT_SetBattery()
{
  int battype,min,max,gohome = 0;
  battype = getArgument(0);
  min = getArgument(1);
  max = getArgument(2);
  gohome = getArgument(1);
  this->battery->setBatterType((BATTERY::BATTERY_TYPE)battype);
  this->battery->setDepletedLevel(min);
  this->battery->setFullyChargedLevel(max);
  this->battery->setGoHomeLevel(gohome);

  this->definitionDefaults->setBatteryType((BATTERY::BATTERY_TYPE)battype);
  this->definitionDefaults->setBatteryFullLevel(max);
  this->definitionDefaults->setBatteryEmptyLevel(min);
  this->definitionDefaults->setBatteryGoHomeLevel(gohome);

}
void API::ACT_GetSensor()
{ /* return sync, command, value */

  if(argument[0]==-1)
  {
    Response_Invalid_Command();
    leave();
    return;
  }
  sensor->select(argument[0]);
  Init_Response();
  Response_add_value(argument[0]);
  for (int i = 0; i < sensor->getArrLength(); i++)
  {
    Response_add_value(sensor->getSignal(i));
  }
  sendEndCommand();
}

void API::ActRespond()
{
  bool valueChanged=false;
  #ifdef SERIALCOMMANDDEBUG
  Serial.print("IN ActRespond\n");
  #endif
  switch (commandIndex) {
    case API_COMMAND::GetMowerStatus:
    Respond_get_MowerStatus();
    break;

    case API_COMMAND::SetState:
    ACT_SetState();
    valueChanged = true;
    break;

    case API_COMMAND::GetState:
    Response_GetState();
    break;

    case API_COMMAND::GetBattery:
    Response_GetBattery();
    break;

    case API_COMMAND::SetBattery:
    ACT_SetBattery();
    valueChanged=true;
    break;

    case API_COMMAND::GetSensor:
    ACT_GetSensor();
    break;

    case API_COMMAND::SetFirstByteToFalse:
    SetFirstByteFalse(); // denna är lite special... tror jag.
    break;

    case API_COMMAND::GetSlowWheelWhenDocking:
    Respond_GetSlowWheelWhenDocking();
    break;

    case API_COMMAND::SetSlowWheelWhenDocking:
    ACT_SetSlowWheelWhenDocking();
    valueChanged=true;
    Respond_GetSlowWheelWhenDocking();
    break;

    case API_COMMAND::GetMotor:
    Response_get_MotorStatus();
    break;

    case API_COMMAND::GetWheelOverloadLevel:
    Response_get_WheelOverloadlevel();
    break;

    case API_COMMAND::SetWheelOverloadLevel:
    ACT_set_WheelOverloadlevel();
    valueChanged=true;
    Response_get_WheelOverloadlevel();

    break;

    default:
    #ifdef SERIALCOMMANDDEBUG
    Serial.print("\nDefault value from AtcReponse\n");
    #endif
    Response_Invalid_Command();
    leave();
    break;
  }
  if(valueChanged)
  {
    EEPROM_WRITE();
    Response_Command_OK();
  }
  //Serial.print("Done\n");
  leave();
}

void API::leave()
{
  #ifdef SERIALCOMMANDDEBUG
  Serial.println("Clearing");
  #endif

  clearTemp();
  clear_args();
  clearBuffer();
  commandIndex=-1;
  inputComplete=false;
}

void API::clearTemp()
{
  for (int i = 0; i < templenght; i++) {
    temp[i]='\0';
  }
}
void API::Response_Command_OK()
{
  Init_Response();
  Response_add_value(API_STATUS_CODE::OK);
  sendEndCommand();
}
void API::SetFirstByteFalse()
{
    eeprom_write_byte(0, false);
    Response_Command_OK();
}
void API::Response_GetState()
{
  Init_Response();
  Response_add_value(*mainState);
  sendEndCommand();
}
void API::ACT_SetState()
{
  /*  Mowing och ERror är fösta och sista enum i state */
  if(argument[0]>=DEFINITION::CUTTERSTATES::MOWING && argument[0]<=DEFINITION::CUTTERSTATES::ERROR)
  {
    *mainState=argument[0];
    this->stateChanged=true; // prepare for Lian.ino to detect changes.
  }
  else
  {
    Response_Invalid_Argument();
  }
}

void API::Debug(char *value)
{
  Serial.print(syncValue);
  Serial.print(DEBUG,DEC);
  Serial.print(delimit);
  Serial.print(value);
  sendEndCommand();
}

bool API::get_StateHasBeenChanged()
{
  return this->stateChanged;
}
void API::ResetStateHasBeenChanged()
{
  this->stateChanged = false;
}

bool API::get_ApiDebug()
{
  return this->apidebug;
}
void API::set_ApiDebug(bool value)
{
  this->apidebug = value;
}
void API::ACT_SetSlowWheelWhenDocking()
{
  definitionDefaults->SetSlowWheelWhenDocking(argument[0]);
}
void API::Respond_GetSlowWheelWhenDocking()
{
  Init_Response();
  Response_add_value(definitionDefaults->GetSlowWheelWhenDocking());
  sendEndCommand();
}
void API::sendHeartBeat()
{
  Serial.print(syncValue);
  Serial.print(API_COMMAND::HEARTBEAT);
  Serial.print(delimit);
  Serial.print(millis()/1000);
  sendEndCommand();
}
void API::sendEndCommand()
{
  Serial.print(endcommand);
  Serial.print('\n');
}
void API::Response_get_MotorStatus()
{
  /* check motor nr
  return speed and load */
  int motornr = getArgument(0);
  int speed, load=-1;
  switch (motornr) {
    case 0:
    speed = leftMotor->getSpeed();
    load = leftMotor->getLoad();
    break;
    case 1:
    speed = rightMotor->getSpeed();
    load = rightMotor->getLoad();
    break;
    case 2:
    speed = this->cutter->getSpeed();
    load = this->cutter->getLoad();
    break;
  }

  Init_Response();
  Response_add_value(motornr);
  Response_add_value(speed);
  Response_add_value(load);
  sendEndCommand();

}
int API::getArgument(int number)
{
  if(argument[number]==-1)
  {
    Response_Invalid_Command();
    leave();
    return -1;
  }
  return argument[number];
}

void API::Respond_get_MowerStatus()
{
  Init_Response();
  Response_add_value(compass->getHeading()); // compass
  Response_add_value(sensor->isInside()); // in/out
  Response_add_value(leftMotor->getLoad()); // LMoto
  Response_add_value(rightMotor->getLoad()); // Rmoto
  Response_add_value(*mainState); // state
  Response_add_value(battery->getSOC()); // SOC
  Response_add_value(*errorNumber); // errorcode
  Response_add_value(cutter->getLoad()); // cutterload
  Response_add_value(leftMotor->getSpeed()); // left motorspeeds
  Response_add_value(rightMotor->getSpeed()); // right motorspeeds
  Response_add_value(cutter->getSpeed()); // cutterSpeed
  Response_add_value(this->looptime); // cutterSpeed
  sendEndCommand();
}

void API::Response_get_WheelOverloadlevel()
{
  Init_Response();
  Response_add_value(definitionDefaults->get_WheelOverload());
  sendEndCommand();
}
void API::ACT_set_WheelOverloadlevel()
{
  bool ok=false;
  int value =-1;
  ok = this->CheckArgNumber(0);
  if(ok)
  {
    value = this->getArgument(0);
    this->definitionDefaults->set_WheelOverload(value);
    Response_Command_OK();
  }
  else
  {
    Response_Invalid_Command();
    leave();
  }

}
