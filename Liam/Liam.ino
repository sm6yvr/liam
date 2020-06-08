/*
 Liam - DIY Robot Lawn Mower

Using an Arduin Uno

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
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>

#include <I2Cdev.h>
#include "RTClib.h"
//#include "HMC5883L.h"
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
  unsigned long lastUpdate;           //Move to global variables
  int interval = 1000;                //Move to global variables
  struct MOWERDATA {          // move to global variable
    String activity;          //Current activity, I.E mowing, looking for signal, charging etc.
    int SoC;                  //Current battery level
    String SoCString;         //Current battery level in string format 
    float batMin;             //Minimum defined level of battery. Depleted level.
    float batMax;             //Maximum defined level of battery. SoC when to consider charging complete
    int leftBWF;              //Status of left BWF. IN (1) or OUT (0).
    int rightBWF;             //Status of left BWF. IN (1) or OUT (0).
    int leftRearBWF;          //Status of rear BWF. IN (1) or OUT (0).
    int rightRearBWF;         //Status of rear BWF. IN (1) or OUT (0).
    bool insideCable;         //Variable used to check if the mower is inside or outside the BWF or cannot see it.
  } mowerData;                //Object name. Fetch or edit data by calling for instance mowerData.SoC = batteryLevel;


int state;
long time_at_turning = millis();
int extraTurnAngle = 0;

long olastemp = millis();
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
#elif defined __ADXL345__
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

void setupInterrupt() {

  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  //cli();//stop interrupts

  //TCCR0A = 0;// set entire TCCR0A register to 0
  //TCCR0B = 0;// same for TCCR0B
  //TCNT0 = 0;//initialize counter value to 0
  //          // set compare match register for 2khz increments
  //OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  //            // turn on CTC mode
  //TCCR0A |= (1 << WGM01);
  //// Set CS01 and CS00 bits for 64 prescaler
  //TCCR0B |= (1 << CS01) | (1 << CS00);
  //// enable timer compare interrupt
  //TIMSK0 |= (1 << OCIE0A);

  //sei();//allow interrupts
}
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
   doInterruptThings();
}


void doInterruptThings() {
  Sensor.selectNext();
}

// ****************** SETUP ******************************************
void setup() {
  // Slow communication on the serial port for all terminal messages.
  // Slowed down for better stability. 
  Serial.begin(9600);

  // Configure all the pins for input or output
  Defaults.definePinsInputOutput();

  // Turn off the cutter motor as fast as possible
  CutterMotor.initialize();

  // Set default levels (defined in Definition.h) for your mower
  Defaults.setDefaultLevels(&Battery, &leftMotor, &rightMotor, &CutterMotor);

  setupInterrupt();

  Serial.println("Init display (setup)");

  // Start up the display
  Display.initialize();

  // Reset the battery voltage reading
  Serial.println("Reset batt reading");

  Battery.resetVoltage();
  Compass.initialize();

  // Run the updateBWF function every time there is a pulse on digital pin2
  attachInterrupt(0, updateBWF, RISING);
  Sensor.setup();
  // Print version information for five seconds before starting
  Display.clear();
  Display.print(F("--- LIAM ---\n"));
  Display.print(F(VERSION_STRING "\n"));
  Display.print(__DATE__ " " __TIME__ "\n");

  #ifdef DEBUG_ENABLED
  Serial.println(F("----------------"));
  Serial.println(F("Send D to enter setup and debug mode"));
  #endif
  delay(5000);
  #ifdef DEBUG_ENABLED
  state = SetupAndDebug.tryEnterSetupDebugMode(0);
  #endif
  Display.clear();

  if (state != SETUP_DEBUG) {
    if (Battery.isBeingCharged()) {
      state = CHARGING;
      Mower.stopCutter();
    }
    else {
      state = MOWING;
    }
  }
// state = IDLE;
} // setup.

// TODO: This should probably be in Controller
void randomTurn(bool goBack) {
  if(goBack) {
    Mower.runBackward(FULLSPEED);
    delay(2000);
  }

  int angle = random(90, 160);
  if (random(0, 100) % 2 == 0) {
    Mower.turnRight(angle);
  } else {
    Mower.turnLeft(angle);
  }
  time_at_turning = millis();
  Compass.setNewTargetHeading();
  Mower.runForward(MOWING_SPEED);
}


// ***************** SAFETY CHECKS ***********************************
void checkIfFlipped() {
#if defined __MS9150__ || defined __MS5883L__ || __ADXL345__
Serial.println("FlipSensor Defined");
  if (Mower.hasFlipped()) {
    Serial.print("Mower has flipped ");
    Mower.stopCutter();
    Mower.stop();
    Error.flag(ERROR_TILT);
  }
#endif
}

void checkIfLifted() {
#if defined __Lift_Sensor__
  Serial.println("LiftSensor Defined");
  if (Mower.isLifted()) {
    Serial.println("Mower is lifted");
    Mower.stopCutter();
    Mower.stop();
    Mower.runBackward(FULLSPEED);
    delay(2000);
    if(Mower.isLifted())
      Error.flag(ERROR_LIFT);
    Mower.randomTurn(false);
  }
#endif
}


// ***************** MOWING ******************************************
void doMowing() {
  if (Battery.mustCharge()) {
    state = LOOKING_FOR_BWF;
    return;
  }

  // Make regular turns to avoid getting stuck on things
  if ((millis() - time_at_turning) > TURN_INTERVAL) {
    randomTurn(true);
    return;
  }

  // Check if any sensor is outside
  for(int i = 0; i < 2; i++) {
    // If sensor is inside, don't do anything
    if(!Sensor.isOutOfBounds(i))
      continue;
    // ... otherwise ...

    Mower.stop();

    int err = Mower.GoBackwardUntilInside(i);
      if(err)
        Error.flag(err);


      if (millis()- time_at_turning < 3000) {
        extraTurnAngle = extraTurnAngle + 10;
      }
      else {
        extraTurnAngle = 0;
      }
      int turnAngle = 50 + extraTurnAngle ;

    // Try to turn away from BWF
    if(i == 0)
      err = Mower.turnToReleaseRight(turnAngle);
    else
      err = Mower.turnToReleaseLeft(turnAngle);

    if(err) {
      // If turning failed, reverse and try once more
      Mower.runBackward(FULLSPEED);
      delay(1000);
      Mower.stop();

      if(i == 0)
        err = Mower.turnToReleaseRight(turnAngle);
      else
        err = Mower.turnToReleaseLeft(turnAngle);

      if (err && err != ERROR_OVERLOAD) {
        Error.flag(err);
      }
    }

    time_at_turning = millis();
    Compass.setNewTargetHeading();
    return; //Stale sensor data after previous delays
  }

  // Avoid obstacles
  Mower.turnIfObstacle();

  // When mowing, the cutter should be on and we should be going forward
  Mower.startCutter();
  Mower.runForwardOverTime(SLOWSPEED, MOWING_SPEED, ACCELERATION_DURATION);

  // Adjust the speed of the mower to the grass thickness
  Mower.compensateSpeedToCutterLoad();

  // Adjust the speed of the mower to the compass heading
  Compass.updateHeading();

  Mower.compensateSpeedToCompassHeading();
}

// ***************** LAUNCHING ***************************************
void doLaunching() {
  // Back out of charger, turn and start mowing
  Mower.runBackward(FULLSPEED);
  delay(5000);
  Mower.stop();
  Mower.turnRight(90);
  //randomTurn(false);

  Battery.resetVoltage();
  time_at_turning = millis();
  state = MOWING;
}

// ***************** DOCKING *****************************************
void doDocking() {
  static int collisionCount = 0;
  static long lastCollision = 0;
  static long lastOutside = 0;
  static bool currentSideIsOutSide = true;

  Mower.stopCutter();

  if (currentSideIsOutSide && !Sensor.isOutOfBounds(0)) {
    currentSideIsOutSide = Sensor.isOutOfBounds(0);
    time_at_turning = millis();
  }

  // Make regular turns to avoid getting stuck on things
  if ((millis() - time_at_turning) > TURN_INTERVAL) {
    Mower.stop();
    Mower.runBackward(DOCKING_WHEEL_HIGH_SPEED);
    delay(2000);
    Mower.stop();
    time_at_turning = millis();
    return;
  }

#if defined __Bumper__
  if (Mower.hasBumped()) {
    Mower.stop();
    Mower.runBackward(DOCKING_WHEEL_HIGH_SPEED);
    delay(2000);
    Mower.stop();
    if (Mower.hasBumped()) {
      Error.flag(ERROR_BUMPERSTUCK);
    }
    time_at_turning = millis();
    return;
  }
#endif

  if(Sensor.isOutOfBounds(0))
    lastOutside = millis();

  if(Battery.isBeingCharged()) {
    Mower.stop();
    state = CHARGING;
    return;
  }

  // If the mower hits something along the BWF
  if(Mower.wheelsAreOverloaded()) {
    if(millis() - lastCollision > 10000)
      collisionCount = 0;
    collisionCount++;
    lastCollision = millis();

    // Serial.print("Collision while docking: ");
    // Serial.println(collisionCount);
    Mower.stop();
    // Let it run for a bit and check if we hit the charger
    delay(1000);
    if(Battery.isBeingCharged()) {
      Mower.stop();
      state = CHARGING;
      return;
    }

    // Go back a bit and try again
    Mower.runBackward(DOCKING_WHEEL_HIGH_SPEED);
    delay(1300);
    Mower.stop();
    Mower.runForward(DOCKING_WHEEL_HIGH_SPEED);

    // After third try. Try to go around obstacle
    if(collisionCount >= 3) {
      Mower.turnRight(70);
      Mower.stop();
      collisionCount = 0;
      lastOutside = millis();
      Mower.runForward(DOCKING_WHEEL_HIGH_SPEED);
    }
    time_at_turning = millis();
    return;
  }

  // Check regularly if right sensor is outside
  if (Sensor.isOutOfBounds(1)) {

#ifdef DOCKING_BACK_WHEN_INNER_SENSOR_IS_OUT
    Mower.stop();
    Mower.runBackward(FULLSPEED);
    delay(500);
    Mower.stop();
    Mower.turnRight(DOCKING_TURN_ANGLE_AFTER_BACK_UP);
    Mower.stop();
    Mower.runForward(DOCKING_WHEEL_HIGH_SPEED);
    time_at_turning = millis();
    lastOutside = millis();
#else
    long turnstart = millis();
    while (millis() - turnstart < 1500 && Sensor.isOutOfBounds(1)) {
      Mower.turnRight(20);
    }
    if (Sensor.isOutOfBounds(1)) {
      Mower.stop();
      Mower.runBackward(FULLSPEED);
      delay(700);
      Mower.stop();
    }
    else {
      Mower.runForward(MOWING_SPEED);
    }
    time_at_turning = millis();
    return;

#endif
  }

  // If left sensor has been inside fence for a long time
  if(millis() - lastOutside > DOCKING_INSIDE_TIMEOUT) {
    Mower.stop();
    Mower.turnLeft(DOCKING_TURN_AFTER_TIMEOUT);
    state = LOOKING_FOR_BWF;
    Serial.println("Start look for BWF");
    time_at_turning = millis();
    return;
  }

  // Track the BWF by compensating the wheel motor speeds
  //Sensor.select(0);
  Mower.adjustMotorSpeeds(Sensor.isOutOfBounds(0));
}
void doWait()
{
  char buf[30];
  for (int i = 0; i < 2; i++)
  {
    // only here for debug purpose
    // If sensor is inside, don't do anything
    if (!Sensor.isOutOfBounds(i))
      continue;
    // ... otherwise ...

    sprintf(buf,"Sensor %i is outside",i);
    Serial.println(buf);
  }
  delay(500);
}

void doLookForBWF() {
  Mower.stopCutter();

  // If sensor is outside, then the BWF has been found
  if(Sensor.isOutOfBounds(0) || Sensor.isOutOfBounds(1)) {
    state = DOCKING;
    time_at_turning = millis();
    return;
  }

  // Make regular turns to avoid getting stuck on things
  if ((millis() - time_at_turning) > TURN_INTERVAL) {
    randomTurn(true);
    return;
  }

  Mower.runForwardOverTime(SLOWSPEED, MOWING_SPEED, ACCELERATION_DURATION);
  Mower.turnIfObstacle();
}

// ***************** CHARGING ****************************************
void doCharging() {
  static long lastContact = 0;
  Mower.stop();
  Mower.stopCutter();

  if(Battery.isBeingCharged()) {
    lastContact = millis();
  }
  // If not charging for a long time, try to re-dock
  if(millis() - lastContact > 20000) {
    Mower.runBackward(SLOWSPEED);
    delay(500);
    Mower.runForward(SLOWSPEED);
    delay(2000);
    Mower.stop();
    // TODO: This resets the timer whether contact was made or not
    // The number of contact attempts should be counted, and some different action
    // should be taken when a certain number of unsuccessfull attempts has been made
    lastContact = millis();
  }

  if(Battery.isFullyCharged()
#if defined __RTC__CLOCK__
    && Clock.timeToCut()
#endif
    ) {
    // Don't launch if no BWF signal is present
    if(Sensor.isInside(0) || Sensor.isOutside(0)) {
      state = LAUNCHING;
      return;
    }
  }
}

//void awareDelay(int ms) {
//  unsigned long exitAt = millis() + ms;
//  int sensor = Sensor.getCurrentSensor();
//  while (millis() < exitAt) {
//    Sensor.select(sensor % NUMBER_OF_SENSORS);
//    Sensor.sensorOutside[Sensor.getCurrentSensor()] = Sensor.isOutOfBounds();
//  }
//}
// ***************** MAIN LOOP ***************************************
void loop() {

  static long lastDisplayUpdate = 0;
  static int previousState;

  long looptime = millis();

  if ((millis() - lastUpdate) > interval) {   //Place this if statement to void loop
    mowerData.activity = state;
    connectedLiam();
  }

  if((state = SetupAndDebug.tryEnterSetupDebugMode(state)) == SETUP_DEBUG)
    return;

  Battery.updateVoltage();


  // int startingSensor = Sensor.getCurrentSensor();
  // Check state of all sensors
  // for(int i = startingSensor; i < startingSensor + NUMBER_OF_SENSORS; i++) {

  //   Sensor.select(i % NUMBER_OF_SENSORS);
  //   Sensor.sensorOutside[Sensor.getCurrentSensor()] = Sensor.isOutOfBounds();
  // }

  // Safety checks
  checkIfFlipped();
  checkIfLifted();

  switch(state) {
    case MOWING:
      doMowing();
      break;
    case LAUNCHING:
      doLaunching();
      break;
    case DOCKING:
      doDocking();
      break;
    case LOOKING_FOR_BWF:
      doLookForBWF();
      break;
    case CHARGING:
      doCharging();
      break;
      case IDLE:
      doWait();
      break;
  }

  if(millis()-lastDisplayUpdate > 5000) {
    // olastemp = millis();
    //Mower.stop();
    Display.update();
    //Mower.runForwardOverTime(SLOWSPEED, MOWING_SPEED, ACCELERATION_DURATION);
    // Serial.print("\nprintTime : ");
    // Serial.println(millis() -olastemp);
    lastDisplayUpdate = millis();
  }
}
