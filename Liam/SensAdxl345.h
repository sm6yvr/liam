/*
 Liam - DIY Robot Lawn Mower

 ADXL345 Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

/*
  This file is a representation of a accelerometer/gyro
  sensor ADXL345 and are used in liam to detect the tilt
  angle of the mover.  The class structure are reused from
  the MotionSensor class to easy plugin it in the current
  liam code base.

  Copyright (c) 2017 Roberth Andersson (and team)
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
