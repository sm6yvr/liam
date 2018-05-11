/*
 Liam - DIY Robot Lawn Mower

 HMC-5883L Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Sens5883L.h"

boolean MS5883L::initialize() {
  boolean test;

  Wire.begin();                 // Join I2C bus
  Serial.begin(115200);         // Set conmmunication rate
  sensor.initialize();            // Initialize the compass
  test = sensor.testConnection();
  current_heading = getHeading();

  return test;            // true = connection OK
}



void MS5883L::updateHeading() {
  current_heading = getHeading();
}

void MS5883L::setNewTargetHeading() {
  for (int i=0; i<10; i++)      // take 10 readings to get a stable value
    target_heading = getHeading();
}

int MS5883L::headingVsTarget() {        // Will return angle difference between heading and target

  return  copysign(1.0,current_heading - target_heading) *
    copysign(1.0,abs(current_heading-target_heading)-180) *
    (180-abs(abs(current_heading-target_heading)-180));

}

int MS5883L::getHeading() {
  int16_t nmx, nmy, nmz;

  sensor.getHeading(&nmx, &nmy, &nmz);

  mx += 0.2*(nmx - mx);
  my += 0.2*(nmy - my);

  float heading = atan2(my,mx);
  if (heading < 0)
    heading += 2*3.1415;

  return heading * 180/3.1415;
}

int MS5883L::getTiltAngle() {
  return 0;
}
