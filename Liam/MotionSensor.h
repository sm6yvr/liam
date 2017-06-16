// This is the library for a Compass
// It uses a 9150 compass/gyro/accelerometer combo
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
  Placed under the GNU license

  ===============================================
*/

#ifndef _MOTIONSENSOR_H_
#define _MOTIONSENSOR_H_

#include <Wire.h>  // For Compass
#include <I2Cdev.h>
#include "Definition.h"

class MOTIONSENSOR {
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
    void autoupdate(void);

  protected:
    int current_heading;
    int target_heading;
    int tilt_angle;
    int16_t mx, gx, ax;
    int16_t my, gy, ay;
    int16_t mz, gz, az;
};

#endif /* _MOTIONSENSOR_H_ */
