/*
 Liam - DIY Robot Lawn Mower

 GY86 Motion Sensor Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include "SensGY86.h"
#include "Definition.h"

boolean GY86::initialize() {
  boolean test_gyro, test_mag;

  Wire.begin();                 // Join I2C bus
  Serial.begin(115200);         // Set conmmunication rate
  sensor_gyro.setI2CMasterModeEnabled(false);
  sensor_gyro.setI2CBypassEnabled(true) ;
  sensor_gyro.setSleepEnabled(false);
  sensor_gyro.initialize();            // Initialize the compass
  sensor_mag.initialize();
  test_gyro = sensor_gyro.testConnection();
  test_mag = sensor_mag.testConnection();
  current_heading = getHeading();

  return test_gyro && test_mag;            // true = connection OK
}

void GY86::updateHeading() {
  current_heading = getHeading();
}

void GY86::setNewTargetHeading() {
  for (int i=0; i<10; i++)      // take 10 readings to get a stable value
    target_heading = getHeading();
  #ifdef DEBUG_ENABLED
  Serial.print(F("New Target Heading:"));
  Serial.println(target_heading);
  #endif  
}


int GY86::headingVsTarget() {       // Will return angle difference between heading and target

  return  copysign(1.0,current_heading - target_heading) *
    copysign(1.0,abs(current_heading-target_heading)-180) *
    (180-abs(abs(current_heading-target_heading)-180));

}


int GY86::getHeading() {
  int16_t nmx, nmy, nmz;

  sensor_mag.getHeading(&nmx, &nmy, &nmz);

  mx += 0.2*(nmx - mx);
  my += 0.2*(nmy - my);

  float heading = atan2(my,mx);
  if (heading < 0)
    heading += 2*3.1415;

  return heading * 180/3.1415;
}
int GY86::getTiltAngle() {
  sensor_gyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float tiltxz = abs(atan2(ax,az));
  float tiltyz = abs(atan2(ay,az));

  if (tiltxz > tiltyz)
    return tiltxz * 180 / 3.1415;
  else
    return tiltyz * 180 / 3.1415;
}
