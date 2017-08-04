#include "API.h"
#include "Arduino.h"
#include "Definition.h"
API::API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition, int *state) {
  leftMotor = left;
  rightMotor = right;
  cutter = cut;
  sensor = bwf;
  compass = comp;
  battery = batt;
  definitionDefaults =definition;
  mainState = state;
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
  eeprom_write_byte(0,true);
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
  Response_Invalid_Command();
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
  if(commandIndex==API_COMMAND::INVALID)
  return false;
  return true;
}
bool API::CheckCommand()
{
  #ifdef SERIALCOMMANDDEBUG
  printIndex();
  #endif
  short counter=0;
  for (size_t i = index; i < bufferlenght; i++)
  {
    this->c = &buffer[i];
    #ifdef SERIALCOMMANDDEBUG
    Serial.print("CheckCommand, value that will be checked ");
    Serial.print(*c);
    Serial.print('\n');
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
      if(*c == '\0' || *c== delimit) // Some Get_Commands is syncValue and Commandnumber then \0 since we dont save term in buffer
      {
        #ifdef SERIALCOMMANDDEBUG
        Serial.print("Will set us done\n");
        #endif
        index = index + counter;
        return setCommand();
      }
      else
      {
        Serial.println("Hit borde man inte komma.." + *c);
        // response Invalid command.
        return false;
      }
    } // if not digit or alpha (else)
  } // for loop

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
  clearTemp();
  //Next char (index + 1) will hold first api_args, these must be digits.
  short counter=0;
  for (size_t i = index + 1; i < bufferlenght; i++)
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
      else if (*c == '\r' || *c == '\0')
      {// if value is carrier return or 0 terminator we are considerd done.. store arg and continue
          argument[argscounter] = atoi(temp);
          #ifdef SERIALCOMMANDDEBUG
          Serial.print("End of command found\n");
          Serial.print("and this value was just added\n");
          Serial.print(temp);
          Serial.print('\n');
          #endif

      }
      else
      {
        #ifdef SERIALCOMMANDDEBUG
          Serial.print("This value will fail... line 114");
          Serial.println(*c);
          Serial.print("That one..\n");
          #endif
          return false;
        }
} // For loop
  #ifdef SERIALCOMMANDDEBUG
  Serial.println("Args == ");
  for (int i = 0; i < argslength; i++) {
    if(argument[i]=='\0')
    break;
    Serial.print(argument[i], DEC);
    Serial.print(" : ");
  }
  Serial.print('\n');
  #endif
  WeAreDone();
  return true;
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
    Serial.print(" : ");
    Serial.print(argument[i],DEC);
  }
  Serial.print('\n');
  #endif
}
void API::Response_Invalid_Command()
{
  #ifdef SERIALCOMMANDDEBUG
  Serial.println(API_COMMAND_PRINT_NAME(API_COMMAND::INVALID));
  #else
  sprintf (buffer, ";%i:%s",commandIndex," IS INVALID");
  Serial.println(buffer);
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
} // clearBuffer

void API::RespondGetSetUpDebug()
{
  //Serial.print("IN RespondGetSetUpDebug\n");
  sprintf (buffer, ";%i:%i", commandIndex, definitionDefaults->get_SETUP_AND_DEBUG());
  Serial.println(buffer);
}
bool API::ACT_SetUpDebug()
{
  if(CheckArgNumber(1)) /* Check if therer are more arguments 1 and .....
  if so, set this command invalid. */
  {
    Response_Invalid_Command();
    leave();
    return false;
  }
  bool temp;

  if(argument[0]==1)
  temp = true;
  else
  temp = false;

  definitionDefaults->set_SETUP_AND_DEBUG(temp);
  RespondGetSetUpDebug();
  return true;
}
void API::Response_GetBattery()
{
  /* Return sync, kommand, TYPE, MIN, MAX, GOHOME SOC */
  sprintf (buffer,
    ";%i:%i:%i:%i:%i:%i",
    commandIndex,
    battery->getBatteryType(),
    battery->getDepletedLevel(),
    battery->getFullyChargedLevel(),
    battery->getGoHomeLevel(),
    battery->getSOC()
  );
  Serial.println(buffer);
}
void API::ACT_SetBattery()
{
  this->battery->setBatterType((BATTERY::BATTERY_TYPE)argument[0]);
  this->battery->setDepletedLevel(argument[1]);
  this->battery->setFullyChargedLevel(argument[2]);
  this->battery->setGoHomeLevel(argument[3]);

  this->definitionDefaults->setBatteryType((BATTERY::BATTERY_TYPE)argument[0]);
  this->definitionDefaults->setBatteryFullLevel(argument[2]);
  this->definitionDefaults->setBatteryEmptyLevel(argument[1]);
  this->definitionDefaults->setBatteryGoHomeLevel(argument[3]);
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
Serial.print(";");
Serial.print(commandIndex);
Serial.print(':');
Serial.print(argument[0]);
for (int i = 0; i < 10; i++)
{
Serial.print(':');
  Serial.print(sensor->getSignal(i));

  //delay(86); /* signal is every 86 ms, so we need at least this delay for BWF signal to be registered */
}
Serial.println("");
}

void API::ActRespond()
{
  bool valueChanged=false;
  #ifdef SERIALCOMMANDDEBUG
  Serial.print("IN ActRespond\n");
  #endif
  switch (commandIndex) {
    case API_COMMAND::SetState:
    ACT_SetState();
    valueChanged = true;
    break;

    case API_COMMAND::GetState:
    Response_GetState();
    break;

    case API_COMMAND::GetSetUpDebug:
    RespondGetSetUpDebug();
    break;

    case API_COMMAND::SetSetUpDebug:
    valueChanged=ACT_SetUpDebug();
    break;

    case API_COMMAND::GetBattery:
    Response_GetBattery();
    break;

    case API_COMMAND::SetBattery:
    ACT_SetBattery();
    valueChanged=true;
    Response_GetBattery();
    break;

    case API_COMMAND::GetSensor:
    ACT_GetSensor();
    break;

    case API_COMMAND::SetFirstByteToFalse:
    SetFirstByteFalse();
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
    Serial.print("Detta tog: ");
    long nu=millis();
    EEPROM_WRITE();
    Serial.print(millis()-nu);
    Serial.println(" millisekunder");
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
  commandIndex=API_COMMAND::INVALID;
  inputComplete=false;
}

void API::clearTemp()
{
  for (size_t i = 0; i < templenght; i++) {
    temp[i]='\0';
  }
}
void API::SetFirstByteFalse()
{
  Serial.println("Sätter byte till false");
  eeprom_write_byte(0, false);
}
void API::Response_GetState()
{
  Serial.print("state == ");
  Serial.print(*mainState);
}
void API::ACT_SetState()
{
if(argument[0]>=0 && argument[0]<=1)
  *mainState=argument[0];
}
