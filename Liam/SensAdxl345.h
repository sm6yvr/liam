/*
*  SemsADXL345.h is part of the project: liam - DIY Robot Lawn mover
*
*  Description: This file is a representation of a accelerometer/gyro sensor
*              ADXL345 and are used in liam to detect the tilt angle of the mover.
*              The class structure are reused from the MotionSensor class to easy
*              plugin it in the current liam code base.
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

#include "MotionSensor.h"
#include <Wire.h>
#include "Definition.h"

#ifndef _SENSADXL345_H_
#define _SENSADXL345_H_

/* 0x53 if SDO is connected to GND otherwise 0x1D */
#define ADDRESS_I2C 0x53
#define ADDRESS_DEVID 0x00
#define ADDRESS_DATA_FORMAT 0x31
#define POWER_CTL 0x2D
#define VALUE_POWER_CTL 0x08
#define DATAX0  0x32
#define ADRESS_DATAX0 0x32

class SENSADXL345 : public MOTIONSENSOR {
  public:
    virtual boolean initialize();
    virtual void updateHeading();
    virtual int getHeading();
    virtual void setNewTargetHeading();
    virtual int headingVsTarget();
    virtual int getTiltAngle();
    virtual int getXAngle();
    virtual int getZAngle();
    virtual int getYAngle();

  protected:
    int current_heading;
    int target_heading;
    int tilt_angle;
  private:
    byte buffer[6];
    int gyro_y;
    int gyro_z;
    byte readRegisterSingle(byte registerAddress);
    void readRegister(char registerAddress, int numBytes);
    void writeRegister(byte address, byte val);
};

#endif /* _SENSADXL345_H_ */
