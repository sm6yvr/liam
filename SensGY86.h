/*
 Liam - DIY Robot Lawn Mower

 GY-86 Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#ifndef _GY86_H_
#define _GY86_H_

#include "MotionSensor.h"
#include <Wire.h>  // For Compass
#include <I2Cdev.h>
#include <HMC5883L.h>
#include <MPU6050.h>

class GY86 : public MOTIONSENSOR {
  public:
    virtual boolean initialize();
    virtual void updateHeading();
    virtual int getHeading();
    virtual void setNewTargetHeading();
    virtual int headingVsTarget();
    virtual int getTiltAngle();

  private:
    HMC5883L sensor_mag;
    MPU6050 sensor_gyro;

};

#endif /* _GY86_H_ */
