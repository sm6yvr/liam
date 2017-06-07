/*
*  SemsADXL345.cpp is part of the project: liam - DIY Robot Lawn mover
*
*  Description: This file is a representation of a accelerometer/gyro sensor
*              ADXL345 and are used in liam to detect the tilt angle of the mover.
*              The class structure are reused from the MotionSensor class to easy
*              plugin it in the current liam code base.
*
*              The Heading/Target methods is just mock because the controller are built
*              to use a compass sensor with included gyro to get the tilt angle.
*              This sensor has not any compass but the methods must be implemented.
*
*  2017-06-06  - Initial commit
*
*  Copyright (c) 2017 Roberth Andersson (and team)
*
*  liam - DIY Robot LAwn mower is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Foobar is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "SensAdxl345.h"
#include "Definition.h"

boolean SENSADXL345::initialize() {

  Wire.begin();

  /* Health check of the sensor, test if it's possible to read from it */
  byte id = 0;
  id = readRegisterSingle(ADDRESS_DEVID);

  if (id != 0xE5){
    Serial.print("Could not read data from adxl345 on address ");
    Serial.println(ADDRESS_DEVID);
    return false;
  }

  /* ADXL345 Settings */
  writeRegister(ADDRESS_DATA_FORMAT, 0x01);

  /* Setup sensor to start read sensor data */
  writeRegister(POWER_CTL, 0x08);
  Serial.println("SensADXL345 Initialized!");
	return true;
}


void SENSADXL345::updateHeading() {
    current_heading = getHeading();
}

void SENSADXL345::setNewTargetHeading() {
  /* take 10 readings to get a stable value */
	for (int i=0; i<10; i++)
		target_heading = getHeading();
}

/* Will return angle difference between heading and target */
int SENSADXL345::headingVsTarget() {
	return 	copysign(1.0,current_heading - target_heading) *
			copysign(1.0,abs(current_heading-target_heading)-180) *
			(180-abs(abs(current_heading-target_heading)-180));
}

int SENSADXL345::getHeading() {
  return 0;
}

/* Reading Y and Z angles from the sensor and return the difference from the normal
*  angle when the mover is standing flat. It returns that axis that differs most */
int SENSADXL345::getTiltAngle() {

  SENSADXL345::readRegister(DATAX0, 6);
  gyro_y = ((int)buffer[3]<<8)|(int)buffer[2];
  gyro_z = ((int)buffer[5]<<8)|(int)buffer[4];

  /* Do X and Y positive */
  gyro_y = gyro_y + 180;
  gyro_z = gyro_z + 180;

  int y_angle_diff;
  if (gyro_y > Y_ANGLE_NORMAL) {
      y_angle_diff = gyro_y - Y_ANGLE_NORMAL;
  } else {
      y_angle_diff = Y_ANGLE_NORMAL - gyro_y;
  }

  int z_angle_diff;
  if (gyro_z > Z_ANGLE_NORMAL) {
      z_angle_diff = gyro_z - Z_ANGLE_NORMAL;
  } else {
      z_angle_diff = Z_ANGLE_NORMAL - gyro_z;
  }

  if (y_angle_diff > z_angle_diff) {
    return y_angle_diff;
  } else {
    return z_angle_diff;
  }
}

/* Reading single value from the register and retun it as an byte */
byte SENSADXL345::readRegisterSingle(byte registerAddress) {
  byte v = 0;
  Wire.beginTransmission(ADDRESS_I2C);
  Wire.write(registerAddress);
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS_I2C,1,true);
  v = Wire.read();
  return v;
}

/* Reading multiple values from the register and store in the buffer */
void SENSADXL345::readRegister(char registerAddress, int numBytes){

  Wire.beginTransmission(ADDRESS_I2C);
  Wire.write(registerAddress);
  Wire.endTransmission();

  /* Setup to read numBytes from the register */
  Wire.requestFrom(ADDRESS_I2C, numBytes, true);

  int i = 0;
  while(Wire.available())
  {
    buffer[i] = Wire.read();
    i++;
  }
}

/* Write value to the sensors register */
void SENSADXL345::writeRegister(byte address, byte val) {
  Wire.beginTransmission(ADDRESS_I2C);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}
