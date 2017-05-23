// This is the library for a Compass
// It uses a 9150 compass/gyro/accelerometer combo
// Changelog:
//     2014-12-12 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#ifndef _MS5883L_H_
#define _MS5883L_H_

#include "MotionSensor.h"
#include <Wire.h>  // For Compass
#include <I2Cdev.h>
#include <HMC5883L.h>

class MS5883L : public MOTIONSENSOR {
    public:
        virtual boolean initialize();
        virtual void updateHeading();
        virtual int getHeading();
        virtual void setNewTargetHeading();
        virtual int headingVsTarget();
        virtual int getTiltAngle();
        
    private:
    	HMC5883L sensor;

};

#endif /* _COMPASS_H_ */
