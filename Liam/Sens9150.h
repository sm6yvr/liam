/*
 Liam - DIY Robot Lawn Mower

 MPU-9150 Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#ifndef _MS9150_H_
#define _MS9150_H_

#include "MotionSensor.h"
#include <Wire.h>  // For Compass
#include <I2Cdev.h>
#include <MPU9150.h>

class MS9150 : public MOTIONSENSOR{
  public:
    virtual boolean initialize();
    virtual void updateHeading();
    virtual int getHeading();
    virtual void setNewTargetHeading();
    virtual int headingVsTarget();
    virtual int getTiltAngle();

  private:
    MPU9150 sensor;
};

#endif /* _MS9150_H_ */
