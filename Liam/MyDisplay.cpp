/*
 Liam - DIY Robot Lawn Mower

 Display Library

 ======================
  Licensed under GPLv3
 ======================
*/
/*
  Base class for displays.
  Default behavior is to print to serial port.

  Subclasses should implement the following functions:
  - size_t write(uint8_t)
  - void setCursor(int col, int row)
  - void clear()
  - void blink();

  If necesary, also override
  - boolean initialize()
  but make sure to also call MYDISPLAY::initialize()
*/

#include "MyDisplay.h"

MYDISPLAY::MYDISPLAY(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state)
{
  Battery = batt;
  leftMotor = left;
  rightMotor = right;
  cutter = cut;
  sensor = bwf;
  compass = comp;
  moverstate = state;
}

boolean MYDISPLAY::initialize()
{
    Serial.println("MYDISPLAY::initialize");

    Wire.begin();
  Wire.beginTransmission(0x27);
  int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("LCD found.");

  } else {
    Serial.println("LCD not found.");
  } 
  for (int i=0; i<3; i++)
    blink();

  clear();
  return true;
}

// Do NOT override. Implement basic commands here
void MYDISPLAY::update()
{
  // Row 1: Sensor status
  setCursor(0,0);
/*
#if __MS9150__ || __MS5883L__ || __ADXL345__ || __MMA7455__
  print(F("Comp: "));
  print(compass->getHeading());
#else
  print(F("Sens: "));
  print(F("Disabled"));
#endif

  print(F("InOut: "));
  print(!sensor->isOutOfBounds(0));
  print(!sensor->isOutOfBounds(1));

  print("\n");
  // Row 2: Motor load
  print(F("LM:"));
  print(leftMotor->getLoad());
  print(F("RM:"));
  print(rightMotor->getLoad());
*/
  //print("\n");
  // Row 3: Battery
  print(F("B:"));
  print(Battery->getVoltage());
  print(F(" IO:"));
  print(!sensor->isOutOfBounds(0));
  print(!sensor->isOutOfBounds(1));
  
  //print("\n");
  // Row 4: State and Error data
  //print(F("State: "));
  setCursor(0,1);

  switch (*moverstate)
  {
    case MOWING:
      print(F("MOWING"));
      break;
    case LAUNCHING:
      print(F("LAUNCHING"));
      break;
    case DOCKING:
      print(F("DOCKING"));
      break;
    case CHARGING:
      print(F("CHARGING"));
      break;
    case LOOKING_FOR_BWF:
      print(F("LOOKING"));
      break;
      case IDLE:
      print(F("IDLE"));
      break;
  }
  print(F("L:"));
  print(leftMotor->getLoad());
  print(F("R:"));
  print(rightMotor->getLoad());
}


// DEVICE SPECIFIC FUNCTIONS

// Override this for each type of display
size_t MYDISPLAY::write(uint8_t s)
{
  // By default just write to serial port
  return Serial.write(s);
}

// Override this for each type of display
void MYDISPLAY::setCursor(int col, int row)
{
  // For a serial port, do nothing
}

// Override this for each type of display
void MYDISPLAY::clear()
{
  // For a serial port, do very little
  println();
}

// Override this for each type of display
void MYDISPLAY::blink()
{
  // For a serial port, do very little
  println("*");
}
