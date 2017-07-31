#include "API.h"
#include "Arduino.h"
#include "Definition.h"
API::API(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt, DEFINITION *definition) {
    leftMotor = left;
    rightMotor = right;
    cutter = cut;
    sensor = bwf;
    compass = comp;
    battery = batt;
    definitionDefaults =definition;
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
  inputComplete=CheckSyncValue(); // start with check if buger contains syncValue
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
  for (size_t i = 0; i < argslength; i++) {
    argument[i] ='\0';
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
    }
    else if (isdigit(*c))
    {
      temp[counter]=buffer[i]; // add bufforposition to out temp.. might come more digits;
      counter++;
      #ifdef SERIALCOMMANDDEBUG
      Serial.print("Temp is now == ");
      Serial.print(temp);
      Serial.print('\n');
      #endif
    }
    else
    {
      if(*c == this->delimit)
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
      else
      {
        if(*c == '\r' || *c == '\0') // if value is carrier return or 0 terminator we are considerd done.. store arg and continue
        {
        argument[argscounter] = atoi(temp);
          #ifdef SERIALCOMMANDDEBUG
          Serial.print("End of command found\n");
          Serial.print("and this value was just added\n");
          Serial.print(temp);
          Serial.print('\n');
          #endif
          return true;
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
      }
    }
  }
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
  inputComplete = false;
  clearBuffer();
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
  if(i)
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
void API::ACT_SetUpDebug()
{
//  Serial.print("IN ACT_SetUpDebug\n");

if(CheckArgNumber(1))
{
Response_Invalid_Command();
clearBuffer();
return;
}
bool temp;

  if(argument[0]==1)
    temp = true;
    else
    temp = false;

    definitionDefaults->set_SETUP_AND_DEBUG(temp);
    RespondGetSetUpDebug();
}
void API::Response_GetBattery()
{
  /// TYPE MIN MAX GOHOME
  sprintf (buffer,
     ";%i:%i:%i:%i:%i",
      commandIndex,
      battery->getBatteryType(),
      battery->getDepletedLevel(),
      battery->getFullyChargedLevel(),
      battery->getSOC());


  Serial.println(buffer);
}
void API::ACT_SetBattery()
{
  this->battery->setBatterType(argument[0]);
  this->battery->setDepletedLevel(argument[1]);
  this->battery->setFullyChargedLevel(argument[2]);
  this->battery->setGoHomeLevel(argument[3]);
}
void API::ACT_GetLSensor()
{
  
}
void API::ACT_GetRSensor()
{

}
void API::ActRespond()
{
  Serial.print("IN ActRespond\n");
  switch (commandIndex) {
    case API_COMMAND::GetSetUpDebug:
      RespondGetSetUpDebug();
      break;
      case API_COMMAND::SetSetUpDebug:
      ACT_SetUpDebug();
      break;
      case API_COMMAND::GetBattery:
      Response_GetBattery();
      break;
      case API_COMMAND::SetBattery:
      ACT_SetBattery();
      break;
      default:
      Serial.print("\nDefault value from AtcReponse\n");
      Response_Invalid_Command();
      break;
    }
//Serial.print("Done\n");

}

void API::leave()
{
  Serial.println("Clearing");
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
