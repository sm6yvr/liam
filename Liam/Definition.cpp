/*
  Definition.h is part of the project: liam - DIY Robot Lawn mover

  Description: This file holds all configuration parameters, user specific,
               common and software specific parameters.

  2017-06-06 Roberth Andersson:
             - Restructured the file and made differens sections of parameters. Added setup debug mode.
  2014-12-12 Jonas Forsell:
            - Initial version.

  Copyright (c) 2017 Jonas Forsell (and team)

  liam - DIY Robot LAwn mower is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Definition.h"


/* Define all pins */
void DEFINITION::definePinsInputOutput() {

	pinMode(WHEEL_MOTOR_A_CURRENT_PIN, INPUT);
	pinMode(WHEEL_MOTOR_B_CURRENT_PIN, INPUT);
	pinMode(SOC_PIN, INPUT);
	pinMode(CUTTER_CURRENT_PIN, INPUT);

/*	Some pins are better leave undefined as default
	pinMode(I2C_SDA_PIN, INPUT);
	pinMode(I2C_SDL_PIN, OUTPUT);

	pinMode(RX_PIN, INPUT);
	pinMode(TX_PIN, OUTPUT);
*/
	pinMode(BWF_SENSOR_INPUT_PIN, INPUT);
	pinMode(WHEEL_MOTOR_A_PWM_PIN, OUTPUT);
	pinMode(BWF_SELECT_A_PIN, OUTPUT);
	pinMode(DOCK_PIN, INPUT);
	pinMode(CUTTER_PWM_PIN, OUTPUT);
	pinMode(BWF_SELECT_B_PIN, OUTPUT);

#if __Bumper__
	pinMode(BUMPER, INPUT);
	digitalWrite(BUMPER, HIGH);
#else
	pinMode(BUMPER, OUTPUT);
	digitalWrite(BUMPER, LOW);
#endif

#if __Lift_Sensor__
	pinMode(LIFT_SENSOR_PIN, INPUT);
	digitalWrite(LIFT_SENSOR_PIN, HIGH);
#else
	pinMode(LIFT_SENSOR_PIN, OUTPUT);
	digitalWrite(LIFT_SENSOR_PIN, LOW);
#endif

	pinMode(LED_PIN, OUTPUT);
	pinMode(WHEEL_MOTOR_B_PWM_PIN, OUTPUT);
	pinMode(WHEEL_MOTOR_A_DIRECTION_PIN, OUTPUT);
	pinMode(WHEEL_MOTOR_B_DIRECTION_PIN, OUTPUT);

}

void DEFINITION::setDefaultLevels(BATTERY* battery, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cutter) {
	left->setOverloadLevel(WHEELMOTOR_OVERLOAD);
	right->setOverloadLevel(WHEELMOTOR_OVERLOAD);
	cutter->setOverloadLevel(CUTTER_OVERLOAD);
	battery->setBatterType(this->get_MY_BATTERY_TYPE());
	battery->setFullyChargedLevel(this->batteryFullLevel);
	battery->setDepletedLevel(this->batteryEmptyLevel);
	battery->setGoHomeLevel(this->batteryGoHomeLevel);

}

void DEFINITION::set_SETUP_AND_DEBUG(bool & value)
{
	this->setupAndDebug=value;
}
bool DEFINITION::get_SETUP_AND_DEBUG()
{
	return this->setupAndDebug;
}
BATTERY::BATTERY_TYPE  DEFINITION::get_MY_BATTERY_TYPE()
{
return this->my_battery_type;

}
int DEFINITION::getBattyFullLevel()
{return this->batteryFullLevel;}
int DEFINITION::getBattyEmptyLevel()
{return this->batteryEmptyLevel;}
int DEFINITION::getBattyGoHomeLevel()
{return batteryGoHomeLevel;}
void DEFINITION::setBatteryType(BATTERY::BATTERY_TYPE type)
{
	this->my_battery_type=type;
}
void DEFINITION::setBatteryFullLevel(int & value)
{
	this->batteryFullLevel = value;
}
void DEFINITION::setBatteryEmptyLevel(int & value)
{
	this->batteryEmptyLevel = value;
}
void DEFINITION::setBatteryGoHomeLevel(int & value)
{
	this->batteryGoHomeLevel = value;
}
bool DEFINITION::GetUseAPI()
{
	return this->useapi;
}
short DEFINITION::GetSlowWheelWhenDocking()
{
	return this->turnpercentWhenDocking;
}
void DEFINITION::SetSlowWheelWhenDocking(short value)
{
	this->turnpercentWhenDocking = value;
}
int DEFINITION::get_FULL_SPEED()
{return this->FULLSPEED;}
void DEFINITION::set_FULL_SPEED(int value)
{this->FULLSPEED = value;}
int DEFINITION::get_SLOW_SPEED()
{return this->SLOWSPEED;}
void DEFINITION::set_SLOW_SPEED(int value)
{this->SLOWSPEED = value;}
int DEFINITION::get_CUTTER_SPEED()
{return this->CUTTERSPEED;}
void DEFINITION::set_CUTTER_SPEED(int value)
{this->CUTTERSPEED = value;}
int DEFINITION::get_HeartBeatTime()
{
	return this->heartBeatTime;
}
void DEFINITION::set_HeartBeatTime(int value)
{
	this->heartBeatTime = value;
}
int DEFINITION::get_WheelOverload()
{
	return this->WHEELMOTOR_OVERLOAD;
}
void DEFINITION::set_WheelOverload(int value)
{
	this->WHEELMOTOR_OVERLOAD = value;
}
