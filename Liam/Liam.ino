/*
 Liam - DIY Robot Lawn Mower

 ======================
  Licensed under GPLv3
 ======================
*/

/*
   Welcome to the Liam5_1 program
   This program will control your mower and relies on a two coil
   configuration (0 and 1) with an optional (2).

   The mower is assumed to be rear wheel driven and have three
   boundary wire reciever coils in the following configuration

    wheel
    ----------------
   |      (0) |
   |(2)           |  ----> Mowing direction
   |      (1) |
    ----------------
  wheel

  Most of the default values for your mower can be set in the
  Definition.h file. Use this to configure your mower for best
  performance.

  (c) Jonas Forssell & team
  Free to use for all.

  Changes in this version
  - Removed OzOLED Support for Arduino101 Compatibility
  - -----------------------------------------------------------
  - Ultrasound sensor bumper support            (Planned)
  - More robust shutdown of mower if wheel overload   (Planned)
  - Revised Error messages                (Planned)
  - Support for OLED Display                (Planned)
  - Signal sensitivity factor in Definition.h       (Planned)
  - Slower mowing if cutter motor is using much current (Planned)
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

#ifdef DEBUG_ENABLED
  #include "SetupDebug.h"
#endif

// Global variables
int state;
long time_at_turning = millis();
int turn_direction = 1;
int LCDi = 0;
int dockCollisionCount = 0;
long lastCollision = 0;
long dockingInsideSince = 0;
long lastDockingAllOutsideCheck = 0;

bool debug_mode = false;


// Set up all the defaults (check the Definition.h file for all default values)
DEFINITION Defaults;

// Please select which type of cutter motor you have
CUTTERMOTOR CutterMotor(CUTTER_MOTOR_TYPE, CUTTER_PWM_PIN, CUTTER_CURRENT_PIN);

// Wheelmotors
WHEELMOTOR rightMotor(WHEEL_MOTOR_A_PWM_PIN, WHEEL_MOTOR_A_DIRECTION_PIN, WHEEL_MOTOR_A_CURRENT_PIN, WHEELMOTOR_SMOOTHNESS);
WHEELMOTOR leftMotor(WHEEL_MOTOR_B_PWM_PIN, WHEEL_MOTOR_B_DIRECTION_PIN, WHEEL_MOTOR_B_CURRENT_PIN, WHEELMOTOR_SMOOTHNESS);

// Battery
BATTERY Battery(BATTERY_TYPE, BAT_PIN, DOCK_PIN);

// BWF Sensors
BWFSENSOR Sensor(BWF_SELECT_B_PIN, BWF_SELECT_A_PIN);

// Compass
#if defined __MS5883L__
MS5883L Compass;
#elif defined __MS9150__
MS9150 Compass;
#else
MOTIONSENSOR Compass;
#endif

// Controller (pass adresses to the motors and sensors for the controller to operate on)
CONTROLLER Mower(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass);

#ifdef DEBUG_ENABLED
SETUPDEBUG SetupAndDebug(&Mower, &leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Battery);
#endif

// Display
#if defined __LCD__
myLCD Display(&Battery, &leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &state);
#else
MYDISPLAY Display(&Battery, &leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &state);
#endif

// RTC clock
#if defined __RTC_CLOCK__
CLOCK Clock(GO_OUT_TIME, GO_HOME_TIME);
#endif

// Error handler
ERROR Error(&Display, LED_PIN, &Mower);

// This function calls the sensor object every time there is a new signal pulse on pin2
void updateBWF() {
  Sensor.readSensor();
}


// ****************** Setup **************************************
void setup()
{
  char buffer [9]; //Format 09.00.00

  Serial.begin(115200);             // Fast communication on the serial port for all terminal messages

  Defaults.definePinsInputOutput();     // Configure all the pins for input or output
  Defaults.setDefaultLevels(&Battery, &leftMotor, &rightMotor, &CutterMotor); // Set default levels (defined in Definition.h) for your mower

  Display.initialize();             // Start up the display

  CutterMotor.initialize();
  Battery.resetVoltage(); // Reset the battery voltage reading
  Compass.initialize();

  attachInterrupt(0, updateBWF, RISING);    // Run the updateBWF function every time there is a pulse on digital pin2
  Sensor.select(0);

  // Print version information for five seconds before starting
  Display.clear();
  Display.print(F("--- LIAM ---\n"));
  Display.print(F(VERSION_STRING "\n"));
  Display.print(__DATE__ " " __TIME__ "\n");

  #ifdef DEBUG_ENABLED
  Serial.println(F("----------------"));
  Serial.println(F("Send D to enter setup and debug mode"));
  //SetupAndDebug.initialize(&Serial);
  state = SetupAndDebug.tryEnterSetupDebugMode(0);
  #endif
  delay(5000);
  Display.clear();

  if (state != SETUP_DEBUG) {
    if (Battery.isBeingCharged()) {     // If Liam is in docking station then
      state = CHARGING;           // continue charging
      Mower.stopCutter();
    }
    else {                    // otherwise
      state = MOWING;
      Mower.startCutter();          // Start up the cutter motor
      //Mower.runForward(FULLSPEED);
    }
  }

}



// ***************** Main loop ***********************************
void loop()
{

  #ifdef DEBUG_ENABLED
  state = SetupAndDebug.tryEnterSetupDebugMode(state);
  if (state == SETUP_DEBUG) {
    return;
  }
  #endif
  long looptime= millis();
  boolean in_contact;
  boolean mower_is_outside;
  int err=0;
  LCDi++;  //Loops 0-10
  if (LCDi % 2500 == 0 ){
    Display.update();
  }

  // Security check Mower is flipped/lifted.
#if defined __MS9150__ || defined __MS5883L__
  if (Mower.hasFlipped())
  {
    Serial.print("Mower has flipped ");
    Mower.stopCutter();
    Mower.stop();
    Error.flag(ERROR_TILT);
  }
#endif

#if defined __Lift_Sensor__
  if (Mower.isLifted())
  {
    Serial.println("Mower is lifted");
    Mower.stopCutter();
    Mower.stop();
    delay(500);
    Mower.runBackward(FULLSPEED);
    delay(2000);
    if(Mower.isLifted())
      Error.flag(ERROR_LIFT);
    Mower.turnRight(90);
    //Mover.startCutter();
    Mower.runForward(FULLSPEED);
  }
#endif

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

  switch (state) {

    //------------------------- MOWING ---------------------------
    case MOWING:
      Battery.updateVoltage();
      Display.update();

      if (Battery.mustCharge()) {
        state = LOOKING_FOR_BWF;
        break;
      }

      if((millis() - time_at_turning) > TURN_INTERVAL)
      {
        int angle = random(90, 160);
        Mower.runBackward(FULLSPEED);
        delay(1200);

        if (random(0, 100) % 2 == 0) {
          Mower.turnRight(angle);
        }
        else {
          Mower.turnLeft(angle);
        }
        time_at_turning = millis();
        Compass.setNewTargetHeading();
        Mower.runForward(FULLSPEED);
      }

      Sensor.select(0);

      mower_is_outside = Sensor.isOutOfBounds();


      // Check left sensor (0) and turn right if needed
      if (mower_is_outside) {
        Serial.println("Left outside");
        Serial.println(Battery.getVoltage());
        Mower.stop();
#ifdef GO_BACKWARD_UNTIL_INSIDE
        err=Mower.GoBackwardUntilInside (&Sensor);
        if(err)
          Error.flag(err);
#endif


        // Tries to turn, but if timeout then reverse and try again
        if (err = Mower.turnToReleaseRight(30) > 0) {
          Mower.runBackward(FULLSPEED);
          delay(1000);
          Mower.stop();
          if (err = Mower.turnToReleaseRight(30) > 0)
            Error.flag(err);
        }

        time_at_turning = millis();
        Compass.setNewTargetHeading();

        if (Mower.allSensorsAreOutside()) {
          Mower.runBackward(FULLSPEED);
          delay(1000);
          Mower.stop();
          if (Mower.allSensorsAreOutside())
            Error.flag(ERROR_OUTSIDE);
        }
      }

      Sensor.select(1);

	  mower_is_outside = Sensor.isOutOfBounds();

      // Check right sensor (1) and turn left if needed
      if (mower_is_outside) {
        Serial.println("Right Outside");
        Serial.println(Battery.getVoltage());
        Mower.stop();

#ifdef GO_BACKWARD_UNTIL_INSIDE
        err=Mower.GoBackwardUntilInside(&Sensor);
        if(err)
          Error.flag(err);
#endif

        // Tries to turn, but if timeout then reverse and try again
        if (err = Mower.turnToReleaseLeft(30) > 0) {
          Mower.runBackward(FULLSPEED);
          delay(1000);
          Mower.stop();
          if (err = Mower.turnToReleaseLeft(30) > 0)
            Error.flag(err);
        }

        time_at_turning = millis();
        Compass.setNewTargetHeading();

        if (Mower.allSensorsAreOutside()) {
          Mower.runBackward(FULLSPEED);
          delay(1000);
          Mower.stop();
          if (Mower.allSensorsAreOutside())
            Error.flag(ERROR_OUTSIDE);
        }
      }

      Mower.startCutter();
      Mower.runForwardOverTime(SLOWSPEED, FULLSPEED, ACCELERATION_DURATION);

      // Adjust the speed of the mower to the grass thickness
      Mower.compensateSpeedToCutterLoad();

      // Adjust the speed of the mower to the compass heading
      Compass.updateHeading();
      Mower.compensateSpeedToCompassHeading();

#if defined __Lift_Sensor__
      if (Mower.isLifted())
      {
        Serial.println("Mower is lifted");
        Mower.stopCutter();
        Mower.runBackward(FULLSPEED);
        delay(2000);
        if(Mower.isLifted())
          Error.flag(ERROR_LIFT);
        Mower.turnRight(90);
        Mower.startCutter();
        Mower.runForward(FULLSPEED);
      }
#endif

      // Check if mower has tilted (providing you have one enabled)
#if defined __MS9150__ || defined __MS5883L__


      // Check if mower has flipped (providing you have one enabled)
      if (Mower.hasFlipped())
      {
        Serial.print("Mower has flipped ");
        Mower.stopCutter();
        Mower.stop();
        Error.flag(ERROR_TILT);
      }
#endif
      Mower.turnIfObstacle();


      break;


      //----------------------- LAUNCHING ---------------------------
    case LAUNCHING:
      Mower.runBackward(FULLSPEED);

      delay(7000);
      Mower.stop();

      // Turn right in random degree
      Mower.turnRight(random(30,60));
      Mower.startCutter();
      Mower.waitWhileChecking(5000);

      time_at_turning = millis();
      Compass.setNewTargetHeading();

      state = MOWING;

      // Reset the running average
      Battery.resetVoltage();

      break;

      //----------------------- DOCKING -----------------------------
    case DOCKING:

		if (Battery.isBeingCharged()) {
			// Stop
			Mower.stop();
			Mower.resetBalance();
			state = CHARGING;
			break;
		}

		if (millis() - lastCollision > 10000) {
			dockCollisionCount = 0;
		}

      // If the mower hits something, reverse and try again
    if (Mower.wheelsAreOverloaded()){
		  dockCollisionCount++;
		  lastCollision = millis();

		  Serial.println(dockCollisionCount);

      //Pause to let charge starting current pan out.
		  delay(1000);
		  if (Battery.isBeingCharged()) {
			  Mower.stop();
			  Mower.resetBalance();
			  state = CHARGING;
			  break;
		  }
        Mower.runBackward(FULLSPEED);
        delay(1300);
		Mower.stop();

		if (dockCollisionCount > 2) {
			Mower.turnRight(70);
			Mower.stop();
			dockCollisionCount = 0;
			dockingInsideSince =0;
			Mower.runForward(FULLSPEED);
			break;
		} 

		Mower.runForward(FULLSPEED);
	  }

	  if (millis() - lastDockingAllOutsideCheck > 500) {
		  //Serial.println("Check for for right outside");
		  Sensor.select(1);
		  if (Sensor.isOutOfBounds()) {
			  Mower.stop();
			  Mower.runBackward(FULLSPEED);
			  delay(700);
			  Mower.stop();
			  Mower.turnRight(20);
			  Mower.stop();
			  Mower.runForward(FULLSPEED);
		  }
		  lastDockingAllOutsideCheck = millis();

	  }



	  Sensor.select(0);
	  if (!Sensor.isOutOfBounds()) {
		  if (dockingInsideSince == 0) {
			  dockingInsideSince = millis();
		  }
		  else if (millis() - dockingInsideSince > 10000){

			  Mower.stop();
			  Mower.turnLeft(30);
			  dockingInsideSince = 0;
			  state = LOOKING_FOR_BWF;
			  break;
		  }
	  }
	  else {
		  dockingInsideSince = 0;
		  //Serial.println("Out");
	  }


      // Track the BWF by compensating the wheel motor speeds
      Mower.adjustMotorSpeeds();

      // Clear signal to allow the mower to track the wire closely
      //Sensor.clearSignal();

      break;

	case LOOKING_FOR_BWF:
    Mower.stopCutter();

    if((millis() - time_at_turning) > TURN_INTERVAL)
    {
      int angle = random(90, 160);
      Mower.runBackward(FULLSPEED);
      delay(1200);

      if (random(0, 100) % 2 == 0) {
        Mower.turnRight(angle);
      }
      else {
        Mower.turnLeft(angle);
      }
      time_at_turning = millis();
      Compass.setNewTargetHeading();
      Mower.runForward(FULLSPEED);
    }

		Sensor.select(0);
		
		if (Sensor.isOutOfBounds()) {
			Serial.println("BWF found");
			state = DOCKING;
    }
    else {
      Mower.runForwardOverTime(SLOWSPEED, FULLSPEED, ACCELERATION_DURATION);
      Mower.turnIfObstacle();
    }
		break;
      //----------------------- CHARGING ----------------------------
    case CHARGING:
      // restore wheelmotor smoothness
      leftMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);
      rightMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);

      // Just remain in this state until battery is full
#if defined __RTC_CLOCK__
      if (Battery.isFullyCharged() && Clock.timeToCut()) {
#else
      if (Battery.isFullyCharged()) {
#endif
          //Don't launch without signal
        if (Sensor.isInside() || Sensor.isOutside())
        {
          state = LAUNCHING;
        }
      }

      in_contact = false;

      // Spend 20 seconds collecting status if being charged
      for (int i=0; i<20; i++) {
        if (Battery.isBeingCharged())
          in_contact = true;
        delay(1000);
      }

      // If the mower is not being charged, jiggle it a bit
      if (!in_contact) {
        Mower.runBackward(20);  // Back away slow speed
        delay(500);
        Mower.runForward(20); // Dock again at slow speed
        delay(1000);
        Mower.stop();
      }

      Battery.updateVoltage();
      Serial.print("BAT:");
      Serial.println(Battery.getVoltage());

      break;

  }
  Serial.print("\n\nlooptime : ");
    Serial.println(millis() - looptime);

}


