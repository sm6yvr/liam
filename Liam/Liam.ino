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
int state = DEFINITION::CUTTERSTATES::IDLE;
long time_at_turning = millis();
int turn_direction = 1;
//int LCDi = 0;
bool sensorwasout=false;
int spikeTime=300;
int LeftwheelOverload=0;
int RightwheelOverload=0;

int WeHaveDocked=0; /* denna används för att kolla om vi dockat i laddstation.
vid varje loop under dockning så räknas denna upp med 1 om dockning, avlusning var 300 ms */
long timeToUpdate=millis();
long looptime = millis();

int GenericCounter=0;
int bumpCount = 0;

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
CONTROLLER Mower(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Defaults);

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

SETUPDEBUG SetupDebug(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Battery);

//API
API api(&leftMotor, &rightMotor, &CutterMotor, &Sensor, &Compass, &Battery, &Defaults, &state, &Mower); // add state to be able to set state from api.
// Error handler
ERROR Error(&Display, LED_PIN, &Mower, &api, Defaults.GetUseAPI());
short SetState(short In_state)
{
  switch (In_state) {
    case DEFINITION::CUTTERSTATES::MOWING:
    Serial.println("IN SET MOWING state");
    Mower.startCutter();
    Mower.runForward(Defaults.get_FULL_SPEED());
    return DEFINITION::CUTTERSTATES::MOWING;

    case DEFINITION::CUTTERSTATES::LAUNCHING:
    time_at_turning = millis();
    return DEFINITION::CUTTERSTATES::LAUNCHING;

    case DEFINITION::CUTTERSTATES::CHARGING:
    // restore wheelmotor smoothness
    leftMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);
    rightMotor.setSmoothness(WHEELMOTOR_SMOOTHNESS);
    Mower.stopCutter();
    Mower.stop();
    time_at_turning=millis();
    return DEFINITION::CUTTERSTATES::CHARGING;

    case DEFINITION::CUTTERSTATES::DOCKING:
    Mower.stopCutter();
    Mower.stop();
    Sensor.select(0);
    time_at_turning=millis();
    //Make the wheel motors extra responsive
    leftMotor.setSmoothness(10);
    rightMotor.setSmoothness(10);
	bumpCount = 0;
    return DEFINITION::CUTTERSTATES::DOCKING;
	case DEFINITION::CUTTERSTATES::DOCKING_EVADE:
		Mower.stop();
		Mower.turnRight(90);

		time_at_turning = millis();
		return DEFINITION::CUTTERSTATES::DOCKING_EVADE;


    case DEFINITION::CUTTERSTATES::IDLE:
    Mower.stopCutter();
    Mower.stop();
    return DEFINITION::CUTTERSTATES::IDLE;

    case DEFINITION::CUTTERSTATES::PREDOCK:
    Mower.stopCutter();
    Mower.stop();
    return DEFINITION::CUTTERSTATES::PREDOCK;

    case DEFINITION::CUTTERSTATES::PRE_DOCK_LEFT_OUT:
    Mower.stop();
    Sensor.select(0);
    return DEFINITION::CUTTERSTATES::PRE_DOCK_LEFT_OUT;

    case DEFINITION::CUTTERSTATES::PRE_DOCK_RIGHT_OUT:
    Mower.stop();
    Sensor.select(0);
    return DEFINITION::CUTTERSTATES::PRE_DOCK_RIGHT_OUT;

    case DEFINITION::CUTTERSTATES::ERROR:
    Serial.println("IN SET ERROR state");
    *api.errorNumber=Error.ERRORCODE();
    Mower.stopCutter();
    Mower.stop();
    return DEFINITION::CUTTERSTATES::ERROR;
  }
}
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
  if (Battery.isBeingCharged())	{			// If Liam is in docking station then
    state = SetState(DEFINITION::CUTTERSTATES::CHARGING);						// continue charging

  }
  else {										// otherwise
    state = SetState(DEFINITION::CUTTERSTATES::MOWING);

  }
  state = SetState(DEFINITION::CUTTERSTATES::IDLE);
Serial.print(";");
Serial.print(API::API_RESPONSE::ONLINE);
Serial.println('#');
}
void updateListeners()
{
  api.sendHeartBeat();
  if(Defaults.GetUseAPI())
  {

}
  else
  {
    Display.update();
  }
}
// ***************** Main loop ***********************************
void loop() {
	api.looptime = int(millis() - looptime);
	looptime = millis();

	if (api.inputComplete)
		api.ValidateCommand();
	else
		APIEvent();

	if (api.get_StateHasBeenChanged())
	{
		state = SetState(state); // API kan ändra denna utan att det märks i detta scope.
		api.ResetStateHasBeenChanged();
	}

	/* MAIN PROGRAM */
	boolean in_contact;
	boolean mower_is_outside;
	int err = 0;
	Battery.updateSOC();
	//LCDi++;  //Loops 0-10
	if (millis() - timeToUpdate > Defaults.get_HeartBeatTime())
	{
		updateListeners();
		timeToUpdate = millis();
	}

	// Security check Mower is flipped/lifted.
#if __MS9150__ || __MS5883L__ || __ADXL345__ || __MMA7455__
	if (Mower.hasFlipped()) {
		if (!Defaults.GetUseAPI())
			Serial.print("Mower has flipped ");
		Mower.stopCutter();
		Mower.stop();
		Error.flag(9);
		SetState(DEFINITION::CUTTERSTATES::ERROR);
		break;
	}
#endif

#if __Lift_Sensor__
	if (Mower.isLifted())
	{
		if (!Defaults.GetUseAPI())
			Serial.println("Mower is lifted");
		Mower.stopCutter();
		Mower.stop();
		delay(500);
		Mower.runBackward(Defaults.get_FULL_SPEED());
		delay(2000);
		if (Mower.isLifted())
		{
			Error.flag(4);
			SetState(DEFINITION::CUTTERSTATES::ERROR);
		}
		Mower.turnRight(90);
		//Mover.startCutter();
		Mower.runForward(Defaults.get_FULL_SPEED());
	}
#endif

	// Check if stuck and trigger action
	Mower.updateBalance();
	// denna triggas vid hemgång.
	if (!DEFINITION::CUTTERSTATES::DOCKING)
	{
		if (abs(Mower.getBalance()) > BALANCE_TRIGGER_LEVEL) {
			Serial.println("LOOP 3");
			Mower.storeState();
			Mower.runBackward(Defaults.get_FULL_SPEED());
			delay(1000);
			Mower.stop();
			Mower.restoreState();
			Mower.resetBalance();
		}
	}

	switch (state) {
		//LCDi++;
		//------------------------- IDLE ---------------------------
	case DEFINITION::CUTTERSTATES::IDLE:
		delay(5);
		break;

		//------------------------- MOWING ---------------------------
	case DEFINITION::CUTTERSTATES::MOWING:
		for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
			Sensor.select(i);

			if (BWF_DETECTION_ALWAYS)
				mower_is_outside = !Sensor.isInside();
			else
				mower_is_outside = Sensor.isOutside();

			// Check left sensor (i) and turn right if needed
			if (mower_is_outside) {
				if (!Defaults.GetUseAPI())
				{
					if (i == 0)
						Serial.println("Left outside");
					else if (i == 1)
						Serial.println("Right outside");
				}
				else
				{
					if (api.get_ApiDebug())
					{
						if (i == 0)
							api.Debug("Left Outside");
						else
							api.Debug("Right Outside");
					}
				}
				Mower.stop();
				if (Battery.mustCharge()) {
					state = SetState(DEFINITION::CUTTERSTATES::PREDOCK);
					break;
				}

#if GO_BACKWARD_UNTIL_INSIDE
				Mower.GoBackwardUntilInside(&Sensor);
#endif

				// test.. kolla om båda sensorerna är ute
				if (Mower.allSensorsAreOutside()) {
					Serial.println("LOOP 5");
					Mower.runBackward(Defaults.get_FULL_SPEED());
					delay(1000);
					Mower.stop();
					if (Mower.allSensorsAreOutside())
					{
						Serial.println("All sensors are outside");
						Error.flag(4);
						SetState(DEFINITION::CUTTERSTATES::ERROR);
						break;
					} // if
				} // if

				if (i == 0)
				{
					// Tries to turn, but if timeout then reverse and try again
					if ((err = Mower.turnToReleaseRight(30) > 0))
					{
						Serial.println("LOOP 6");
						Mower.runBackward(Defaults.get_FULL_SPEED());
						delay(1000);
						Mower.stop();
						if ((err = Mower.turnToReleaseRight(30) > 0))
						{
							Error.flag(err);
							SetState(DEFINITION::CUTTERSTATES::ERROR);
							Serial.print("Error ==");
							Serial.println(err, DEC);

							break;
						}
					}
				}
				else if (i == 1)
				{
					if ((err = Mower.turnToReleaseLeft(30) > 0)) {
						Serial.println("LOOP 7");
						Mower.runBackward(Defaults.get_FULL_SPEED());
						delay(1000);
						Mower.stop();
						if ((err = Mower.turnToReleaseLeft(30) > 0))
						{
							Error.flag(err);

							Serial.print("Error ==");
							Serial.println(err, DEC);
							SetState(DEFINITION::CUTTERSTATES::ERROR);
							break;
						}
					}
				}
				Compass.setNewTargetHeading();
				sensorwasout = true;
			}// if (mower_is_outside)
		}//For loop

		Mower.runForward(Defaults.get_FULL_SPEED());
		// Adjust the speed of the mower to the grass thickness
		Mower.compensateSpeedToCutterLoad();
		// Adjust the speed of the mower to the compass heading
		Compass.updateHeading();
		Mower.compensateSpeedToCompassHeading();
		if (sensorwasout)
		{
			spikeTime = 300;
			sensorwasout = false;
		}
		else
		{
			spikeTime = 0;
		}
		if (Mower.wheelsAreOverloaded(spikeTime))
		{
			if (!Defaults.GetUseAPI())
				Serial.print("Wheel overload ");
			Mower.runBackward(Defaults.get_FULL_SPEED());
			if (Mower.waitWhileInside(2000) == 0);
			Mower.turnRight(90);
			Compass.setNewTargetHeading();
			Mower.runForward(Defaults.get_FULL_SPEED());
		}
		break;


		//----------------------- LAUNCHING ---------------------------
	case DEFINITION::CUTTERSTATES::LAUNCHING:

		Mower.runBackward(Defaults.get_FULL_SPEED());
		if (millis() - time_at_turning > 7000)
		{
			Mower.stop();
			// Turn right in random degree
			Mower.turnRight(random(30, 60));
			Mower.waitWhileChecking(5000);
			Compass.setNewTargetHeading();
			state = SetState(DEFINITION::CUTTERSTATES::MOWING);
		}
		// Reset the running average
		Battery.resetSOC();
		break;

		//----------------------- DOCKING -----------------------------
	case DEFINITION::CUTTERSTATES::DOCKING:
		// If the mower hits something, reverse and try again
		GenericCounter++;
		if (Battery.isBeingCharged())
		{
			WeHaveDocked++;
		}
		// kolla om hjulen går tugnt
		// använd motor.getload() == amp per motor..
		LeftwheelOverload += leftMotor.getLoad();
		RightwheelOverload += rightMotor.getLoad();
		if (millis() - time_at_turning > 300) // Kolla vart 300ms om vi ändrat status på dockpinnen
		{
			LeftwheelOverload /= GenericCounter;
			RightwheelOverload /= GenericCounter;
			if (LeftwheelOverload > Defaults.get_WheelOverload() ||
				RightwheelOverload > Defaults.get_WheelOverload())
			{
				bumpCount++;

				if (time_at_turning + 10000 < millis()) {
					bumpCount = 0;
				}
				
				if (bumpCount <= 3) {
					Mower.runBackward(Defaults.get_FULL_SPEED());
					delay(1500);
					Mower.runForward(Defaults.get_FULL_SPEED());
				}
				else {
					state = SetState(DEFINITION::CUTTERSTATES::DOCKING_EVADE);
				}
			}
			if (WeHaveDocked > 0) // if greater than 0 we have docked.
			{
				// Stop
				Mower.stop();
				Mower.resetBalance();
				state = SetState(DEFINITION::CUTTERSTATES::CHARGING);
				WeHaveDocked = 0;
				break;
			}
			GenericCounter = 0;
			WeHaveDocked = 0;
			time_at_turning = millis();
		}


		// Track the BWF by compensating the wheel motor speeds
		Mower.adjustMotorSpeeds(Defaults.GetSlowWheelWhenDocking());
		break;

	case DEFINITION::CUTTERSTATES::DOCKING_EVADE:
		if (Mower.wheelsAreOverloaded(spikeTime))
		{
			if (!Defaults.GetUseAPI())
				Serial.print("Wheel overload ");
			Mower.turnRight(90);
			Compass.setNewTargetHeading();
			state = SetState(DEFINITION::CUTTERSTATES::PREDOCK);
		}

		for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
			Sensor.select(i);

			if (BWF_DETECTION_ALWAYS)
				mower_is_outside = !Sensor.isInside();
			else
				mower_is_outside = Sensor.isOutside();

			// Check left sensor (i)
			if (mower_is_outside)
			{
				Mower.stop();
				if (i == 0)
				{
					state = SetState(DEFINITION::CUTTERSTATES::DOCKING);
					break;
				}
				else
				{
					state = SetState(DEFINITION::CUTTERSTATES::PRE_DOCK_RIGHT_OUT);
					break;
				}
			}// if (mower_is_outside)
			else
			{
				leftMotor.setSpeed((int)Defaults.get_FULL_SPEED() * 0.7);
				rightMotor.setSpeed(Defaults.get_FULL_SPEED());
			}
		}//For loop
		break;
		//----------------------- CHARGING ----------------------------
	case DEFINITION::CUTTERSTATES::CHARGING:

		if (Battery.isBeingCharged())
			WeHaveDocked++;

		if (millis() - time_at_turning > 300)
		{
			if (WeHaveDocked == 0) // if greater than 0 we are in charger.
			{
				Mower.runBackward(20); 	// Back away slow speed
				delay(200);
				Mower.stop();
				delay(50);
				Mower.runForward(20);	// Dock again at slow speed
				delay(400);
				Mower.stop();
			}
			WeHaveDocked = 0;
			time_at_turning = millis();
		}

		// Just remain in this state until battery is full
#if  __RTC_CLOCK__
		if (Battery.isFullyCharged() && myClock.timeToCut())
			state = SetState(DEFINITION::CUTTERSTATES::LAUNCHING);
#else
		if (Battery.isFullyCharged())
			state = SetState(DEFINITION::CUTTERSTATES::LAUNCHING);
#endif

		break;

	case DEFINITION::CUTTERSTATES::PREDOCK:

		for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
			Sensor.select(i);

			if (BWF_DETECTION_ALWAYS)
				mower_is_outside = !Sensor.isInside();
			else
				mower_is_outside = Sensor.isOutside();

			// Check left sensor (i)
			if (mower_is_outside)
			{
				Mower.stop();
				if (i == 0)
				{
					state = SetState(DEFINITION::CUTTERSTATES::DOCKING);
					break;
				}
				else
				{
					state = SetState(DEFINITION::CUTTERSTATES::PRE_DOCK_RIGHT_OUT);
					break;
				}
			}// if (mower_is_outside)
			else
			{
				Mower.runForward(Defaults.get_FULL_SPEED());
			}
		}//For loop

		break;

	case DEFINITION::CUTTERSTATES::PRE_DOCK_RIGHT_OUT:
		// höger spole är triggad. Kör vänster hjul.
		if (Sensor.isInside()) // medans vänster spole är inne..
		{
			Mower.turnRight(2);
		}
		else // vänster spole ute, sätt state docking
		{
			Mower.stop();
			state = SetState(DEFINITION::CUTTERSTATES::PRE_DOCK_LEFT_OUT);
		}
		break;

	case DEFINITION::CUTTERSTATES::PRE_DOCK_LEFT_OUT:
		// Vänster spole är triggad. Kolla vart vi är och justera.
		if (Sensor.isInside()) // medans vänster spole är inne..
		{
			Mower.turnRight(2);
		}
		else // vänster spole ute, sätt state docking
		{
			state = SetState(DEFINITION::CUTTERSTATES::DOCKING);
		}
		break;

	case DEFINITION::CUTTERSTATES::ERROR:
		Serial.println("IN ERror Case");
		Error.waitForReset();
		break;

	}// switch state
}//VOID loop
/*
NOTE::
1. Stänger av NOTIFY klart
2. Adderar HeartBeat 1,2,3,4 eller vart 5:e sekund ?? gör ställbart.
3. Alla  set svar är ;kommando:OK/NOK#
4. alla kommandon måste avslutas med # ok
5. Börja fundera på en subscribe funktion.
6. se till att om utanför BWF hamnar som eget state. Detta för att
kunna räkna tid och inte använda delayer.
7. ERROR funkar inte som tänkt.
 typ ;kommando:SUBSCRIBE
*/
