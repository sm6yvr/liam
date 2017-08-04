/*
   Welcome to the Liam5_1 program
   This program will control your mower and relies on a two coil
   configuration (0 and 1) with an optional (2).

   The mower is assumed to be rear wheel driven and have three
   boundary wire reciever coils in the following configuration

   	wheel
    ----------------
   |			(0)	|
   |(2)         	|  ----> Mowing direction
   |			(1)	|
    ----------------
	wheel

	Most of the default values for your mower can be set in the
	Definition.h file. Use this to configure your mower for best
	performance.

	(c) Jonas Forssell & team
	Free to use for all.

        Changelog:

        5.2-SNAPSHOT
          - 2017-06-16 Roberth Andersson:
                 - Added SetupDebug class
                 - Removed SetupDebug code from Liam.ino
          - 2017-06-08 Roberth Andersson :
                 - New structure in definition.h so the user-specific parameters
                   are in the beginning.
                 - Changed how to enable/disable the different parts in the code.
                 - Moved user-config from Liam.ino to definition.h
                 - Merged Liam5_Setup_and_Debug into Liam and added a new SetupDebug mode.
          - 2017-06-07 Roberth Andersson:
                 - Added gyro sensor ADXL345 support
          - 2017-06-05 Jonas Forsell:
                 - Added function Obstacle when docking
	        - 2017-06-02 Ola Palm:
                 - Added function GO_BACKWARD_UNTIL_INSIDE
        5.1
          - Removed OzOLED Support for Arduino101 Compatibility

  - -----------------------------------------------------------
	- Ultrasound sensor bumper support						(Planned)
	- More robust shutdown of mower if wheel overload		(Planned)
	- Revised Error messages								(Planned)
	- Support for OLED Display								(Planned)
	- Signal sensitivity factor in Definition.h				(Planned)
	- Slower mowing if cutter motor is using much current	(Planned)
	---------------------------------------------------------------
*/

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <I2Cdev.h>
#include "RTClib.h"
#include "HMC5883L.h"
#include "MPU9150.h"
#include "Battery.h"
#include "Wheelmotor.h"
#include "CutterMotor.h"
#include "BWFSensor.h"
#include "Controller.h"
#include "myLcd.h"
#include "Clock.h"
#include "Error.h"
#include "MotionSensor.h"
#include "Sens5883L.h"
#include "Sens9150.h"
#include "Definition.h"
#include "SensAdxl345.h"
#include "MMA_7455.h"
#include "SetupDebug.h"
#include "API.h"

// Global variables
int state = TESTING;
long time_at_turning = millis();
int turn_direction = 1;
int LCDi = 0;


// Set up all the defaults (check the Definition.h file for all default values)
DEFINITION Defaults;

CUTTERMOTOR CutterMotor(MY_CUTTERMOTOR, CUTTER_PWM_PIN, CUTTER_CURRENT_PIN);

// Wheelmotors
WHEELMOTOR rightMotor(WHEEL_MOTOR_A_PWM_PIN, WHEEL_MOTOR_A_DIRECTION_PIN, WHEEL_MOTOR_A_CURRENT_PIN, WHEELMOTOR_SMOOTHNESS);
WHEELMOTOR leftMotor(WHEEL_MOTOR_B_PWM_PIN, WHEEL_MOTOR_B_DIRECTION_PIN, WHEEL_MOTOR_B_CURRENT_PIN, WHEELMOTOR_SMOOTHNESS);

// Battery
BATTERY Battery(Defaults.get_MY_BATTERY_TYPE(), SOC_PIN, DOCK_PIN);



// BWF Sensors
BWFSENSOR Sensor(BWF_SELECT_B_PIN, BWF_SELECT_A_PIN);

// Compass
#if __MS5883L__
MS5883L Compass;
#elif __MS9150__
MS9150 Compass;
#elif __ADXL345__
SENSADXL345 Compass;
#elif __MMA7455__
MMA_7455 Compass;
#else
MOTIONSENSOR Compass;
#endif

// Controller (pass adresses to the motors and sensors for the controller to operate on)
CONTROLLER Mower(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass);

// Display
#if __LCD__
myLCD Display(&Battery, &leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &state);
#else
MYDISPLAY Display(&Battery, &leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &state);
#endif

// RTC klocka
#if __RTC_CLOCK__
CLOCK myClock;
#endif

// Error handler
ERROR Error(&Display, LED_PIN, &Mower);


SETUPDEBUG SetupDebug(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Battery);


//API
API api(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Battery, &Defaults, &state); // add state to be able to set state from api.


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void APIEvent() {
  // /// as for now only one command is stored and handled, might be a bigger buffer that could hold Multiple commands later on, but not for now.
  // if(api.inputComplete)
  // {
  //   Serial.println("Har redan ett kommando");
  //   return;
  // }
    while (Serial.available()) {
    // get the new byte:
    char c = Serial.read();
    api.inputComplete = api.addByteToBuffer(c);
    // leave for now and let loop continue; Might be call to CheckSyncValue from here later.
    }

} // serialEvent


// This function calls the sensor object every time there is a new signal pulse on pin2

void updateBWF() {
  Sensor.readSensor();
}

// ****************** Setup **************************************
void setup()
{

  Serial.begin(115200); 						// Fast communication on the serial port
  if(api.IsWrittenToEEPROM())
  {
    api.EEPROM_READ();
    Serial.println("Läste från eeprom");
}
else
  Serial.println("Läste inte från eeprom");
    /*API har en pekare mot Defaults, så Defaults kommer få nya värden om raden ovan körs. */

  Defaults.definePinsInputOutput();			// Configure all the pins for input or output
  Defaults.setDefaultLevels(&Battery, &leftMotor, &rightMotor, &CutterMotor); // Set default levels (defined in Definition.h) for your mower

  Display.initialize();							// Start up the display
  CutterMotor.initialize();
  Battery.setGoHomeLevel(Defaults.getBattyGoHomeLevel());
  Battery.setFullyChargedLevel(Defaults.getBattyFullLevel());
  Battery.setDepletedLevel(Defaults.getBattyEmptyLevel());

  Battery.resetSOC();// Set the SOC to current value
  Compass.initialize();

#if __RTC_CLOCK__
  myClock.initialize();
  myClock.setGoOutTime(GO_OUT_TIME);
  myClock.setGoHomeTime(GO_HOME_TIME);
#endif

  attachInterrupt(0, updateBWF, RISING);		// Run the updateBWF function every time there is a pulse on digital pin2
  Sensor.select(0);

// #if __SETUP_AND_DEBUG_MODE__
//   Serial.println("LIAM is running in setup debug mode!!!!");
//   SetupDebug.initialize(&Serial);
if(!state==TESTING)
{
  if (Battery.isBeingCharged())	{			// If Liam is in docking station then
    state = CHARGING;						// continue charging
    Mower.stopCutter();
  } else {										// otherwise
    state = MOWING;
    Mower.startCutter();					// Start up the cutter motor
    Mower.runForward(FULLSPEED);
  }
}
}

// ***************** Main loop ***********************************
void loop() {

  if(api.inputComplete)
    api.ValidateCommand();
  else
    APIEvent();

  /* MAIN PROGRAM */
  boolean in_contact;
  boolean mower_is_outside;
  int err = 0;
if(state!=TESTING)
{
  LCDi++;  //Loops 0-10
  if (LCDi % 25 == 0 ) {
    Display.update();
  }
}
  // Security check Mower is flipped/lifted.
#if __MS9150__ || __MS5883L__ || __ADXL345__ || __MMA7455__
  if (Mower.hasFlipped()) {
    Serial.print("Mower has flipped ");
    Mower.stopCutter();
    Mower.stop();
    Error.flag(9);
  }
#endif

#if __Lift_Sensor__
  if (Mower.isLifted())
  {
    Serial.println("Mower is lifted");
    Mower.stopCutter();
    Mower.stop();
    delay(500);
    Mower.runBackward(FULLSPEED);
    delay(2000);
    if (Mower.isLifted())
      Error.flag(4);
    Mower.turnRight(90);
    //Mover.startCutter();
    Mower.runForward(FULLSPEED);
  }
#endif
if(!state==TESTING)
{
  // Check if stuck and trigger action
  Mower.updateBalance();

  if (abs(Mower.getBalance()) > BALANCE_TRIGGER_LEVEL) {
    Mower.storeState();
    Mower.runBackward(FULLSPEED);
    delay(1000);
    Mower.stop();
    Mower.restoreState();
    Mower.resetBalance();
  }
}
  switch (state) {
    case TESTING:
LCDi++;
    if(LCDi <=1)
    {
      Serial.print("TESTMODE\n");

      }

    delay(500);
      break;
    //------------------------- MOWING ---------------------------
    case MOWING:
      Battery.updateSOC();

      for(int i = 0; i < NUMBER_OF_SENSORS; i++) {
      Sensor.select(i);

      if (BWF_DETECTION_ALWAYS)
        mower_is_outside = !Sensor.isInside();
      else
        mower_is_outside = Sensor.isOutside();

      // Check left sensor (i) and turn right if needed
      if (mower_is_outside) {
        if(i==0)
          Serial.println("Left outside");
        else if(i==1)
          Serial.println("Right outside");

        Serial.println(Battery.getSOC());
        Mower.stop();
        if (Battery.mustCharge()) {
          Mower.stopCutter();
          /*
          There might be reasons, but why should the mower run fullspeed forward when it finds out that it has to charge battery??

          Ola Palm.
          */
          Mower.runForward(FULLSPEED);
          delay(1000);
          Mower.stop();
          // change this value to 1 if you would like the mower to go home counter clock wise.
          Sensor.select(0);
          state = DOCKING;
          break;
        }

        #if GO_BACKWARD_UNTIL_INSIDE
          Mower.GoBackwardUntilInside (&Sensor);
        #endif

        if(i==0)
        {
        // Tries to turn, but if timeout then reverse and try again
          if ((err = Mower.turnToReleaseRight(30) > 0)) {
            Mower.runBackward(FULLSPEED);
            delay(1000);
            Mower.stop();
            if ((err = Mower.turnToReleaseRight(30) > 0))
              Error.flag(err);
          }
        }
      else
      {
        if ((err = Mower.turnToReleaseLeft(30) > 0)) {
        Mower.runBackward(FULLSPEED);
        delay(1000);
        Mower.stop();
        if ((err = Mower.turnToReleaseLeft(30) > 0))
          Error.flag(err);
        }
      }

        Compass.setNewTargetHeading();

        if (Mower.allSensorsAreOutside()) {
          Mower.runBackward(FULLSPEED);
          delay(1000);
          Mower.stop();
          if (Mower.allSensorsAreOutside())
            Error.flag(4);
        }
      }
    }

      Mower.runForward(FULLSPEED);

      // Adjust the speed of the mower to the grass thickness
      Mower.compensateSpeedToCutterLoad();

      // Adjust the speed of the mower to the compass heading
      Compass.updateHeading();
      Mower.compensateSpeedToCompassHeading();


      // Check if mower has hit something
      if (Mower.wheelsAreOverloaded())
      {
        Serial.print("Wheel overload ");
        Mower.runBackward(FULLSPEED);
        if (Mower.waitWhileInside(2000) == 0);
        Mower.turnRight(90);
        Compass.setNewTargetHeading();
        Mower.runForward(FULLSPEED);
      }

      // Check if bumper has triggered (providing you have one enabled)
#if  __Bumper__
      if (Mower.hasBumped())
      {
        Serial.print("Mower has bumped ");
        Mower.runBackward(FULLSPEED);
        delay(2000);
        Mower.turnRight(90);
        Mower.runForward(FULLSPEED);
      }
#endif

#if  __Lift_Sensor__
      if (Mower.isLifted())
      {
        Serial.println("Mower is lifted");
        Mower.stopCutter();
        Mower.runBackward(FULLSPEED);
        delay(2000);
        if (Mower.isLifted())
          Error.flag(4);
        Mower.turnRight(90);
        Mower.startCutter();
        Mower.runForward(FULLSPEED);
      }
#endif

      // Check if mower has tilted (providing you have one enabled)
#if  __MS9150__ ||  __MS5883L__ ||  __ADXL345__ || __MMA7455__
      if (Mower.hasFlipped()) {
        Serial.print("Mower has flipped ");
        Mower.stopCutter();
        Mower.stop();
        Error.flag(9);
      } else if (Mower.hasTilted()) {
        Serial.print("Mower has tilted ");
        Mower.runBackward(FULLSPEED);
        delay(2000);
        Mower.turnRight(90);
        Mower.runForward(FULLSPEED);
        delay(200);
      }
#endif

      break;


    //----------------------- LAUNCHING ---------------------------
    case LAUNCHING:

      Mower.runBackward(FULLSPEED);

      delay(7000);
      Mower.stop();

      // Turn right in random degree
      Mower.turnRight(random(30, 60));
      Mower.startCutter();
      Mower.waitWhileChecking(5000);

      Compass.setNewTargetHeading();

      Mower.runForward(FULLSPEED);

      state = MOWING;

      // Reset the running average
      Battery.resetSOC();

      break;

    //----------------------- DOCKING -----------------------------
    case DOCKING:
      Battery.updateSOC();

      //Make the wheel motors extra responsive
      leftMotor.setSmoothness(10);
      rightMotor.setSmoothness(10);

      // If the mower hits something, reverse and try again
      if (Mower.wheelsAreOverloaded()) {
        Mower.runBackward(FULLSPEED);
        delay(1000);
      }

      // See if mower has repeated overload
      // If so, turn away from the BWF and try to hook on somewhere else
/*
      if (Mower.hasReachedAStop()) {
        Mower.runBackward(FULLSPEED);
        delay(1000);
        Mower.turnRight(90);
        Mower.runForward(FULLSPEED);
        delay(1000);
        Mower.startCutter();
        state = MOWING;
        break;
      }
//NOTE : This needs a setting in DEFINITION;
*/

      // Track the BWF by compensating the wheel motor speeds
      Mower.adjustMotorSpeeds();

      // Clear signal to allow the mower to track the wire closely
      Sensor.clearSignal();

      // Wait a little to avoid current spikes
      delay(100);

      // Stop the mower as soon as the charge plates come in contact
      if (Battery.isBeingCharged()) {
        // Stop
        Mower.stop();
        Mower.resetBalance();
        state = CHARGING;
        break;
      }


      break;

    //----------------------- CHARGING ----------------------------
    case CHARGING:
      Battery.updateSOC();
          // restore wheelmotor smoothness
      leftMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);
      rightMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);

      // Just remain in this state until battery is full
#if  __RTC_CLOCK__
      if (Battery.isFullyCharged() && myClock.timeToCut())
        state = LAUNCHING;
#else
      if (Battery.isFullyCharged())
        state = LAUNCHING;
#endif

      in_contact = false;

      // Spend 20 seconds collecting status if being charged
      for (int i = 0; i < 20; i++) {
        if (Battery.isBeingCharged())
          in_contact = true;
        delay(1000);
      }

      // If the mower is not being charged, jiggle it a bit
      if (!in_contact) {
        Mower.runBackward(20); 	// Back away slow speed
        delay(500);
        Mower.runForward(20);	// Dock again at slow speed
        delay(1000);
        Mower.stop();
      }
break;

  }

}//VOID loop
