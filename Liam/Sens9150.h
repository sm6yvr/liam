// It uses a 9150 compass/gyro/accelerometer combo
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
  Placed under the GNU license

  ===============================================
*/

#ifndef _MS9150_H_
#define _MS9150_H_

#include "MotionSensor.h"
#include <Wire.h>  // For Compass
#include <I2Cdev.h>
#include <MPU9150.h>

class MS9150 : public MOTIONSENSOR {
  public:
    virtual boolean initialize();
    virtual void updateHeading();
    virtual int getHeading();
    virtual void setNewTargetHeading();
    virtual int headingVsTarget();
    virtual int getTiltAngle();
    virtual int getZAngle();
    virtual int getYAngle();
    virtual int getXAngle();


  private:
    MPU9150 sensor;
};

#endif /* _MS9150_H_ */
