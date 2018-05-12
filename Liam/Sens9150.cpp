/*
 Liam - DIY Robot Lawn Mower

 MPU-9150 Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "Sens9150.h"
#include "Definition.h"

boolean MS9150::initialize() {
  boolean test;

  Wire.begin();                 // Join I2C bus
  Serial.begin(115200);         // Set conmmunication rate
  sensor.initialize();            // Initialize the compass
  test = sensor.testConnection();
  current_heading = getHeading();

  return test;            // true = connection OK
}


void MS9150::updateHeading() {
  current_heading = getHeading();
}

void MS9150::setNewTargetHeading() {
  for (int i=0; i<10; i++)      // take 10 readings to get a stable value
    target_heading = getHeading();
}

int MS9150::headingVsTarget() {       // Will return angle difference between heading and target

  return  copysign(1.0,current_heading - target_heading) *
    copysign(1.0,abs(current_heading-target_heading)-180) *
    (180-abs(abs(current_heading-target_heading)-180));

}

int MS9150::getHeading() {
  int16_t nmx, nmy, nmz;

  sensor.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &nmx, &nmy, &nmz);

  mx += 0.2*(nmx-mx);
  my += 0.2*(nmy-my);

  float heading = atan2(my,mx);
  if (heading < 0)
    heading += 2*3.1415;

  return heading * 180/3.1415;
}

int MS9150::getTiltAngle() {
  sensor.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

  float tiltxz = abs(atan2(ax,az));
  float tiltyz = abs(atan2(ay,az));

  if (tiltxz > tiltyz)
    return tiltxz * 180 / 3.1415;
  else
    return tiltyz * 180 / 3.1415;
}
