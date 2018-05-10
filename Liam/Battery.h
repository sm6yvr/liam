/*
 Liam - DIY Robot Lawn Mower

 Battery Library

 ======================
  Licensed under GPLv3
 ======================
*/

#include <Arduino.h>

#ifndef _BATTERY_H_
#define _BATTERY_H_

#define LEAD_ACID            0 // this device only has one address
#define NIMH           1
#define LIION          2

//All voltage value need to be x100 to avoid decimals (12.54V = 1254)
#define LIIONFULL       1256
#define LIIONEMPTY      1040
#define NIMHFULL        1450
#define NIMHEMPTY       1150
#define LEADACIDFULL    1330
#define LEADACIDEMPTY   1200

#define FILTER        5    //how hard to filter voltage readings
#define VOLTDIVATOR     42   //Voltage divider faktor x10 to avoid decimal ( 4 = 40)


class BATTERY {
  public:
    BATTERY(int type, int socpin, int dockpin);

    //
    int getBatteryType();

    void setFullyChargedLevel(int level);
    int getFullyChargedLevel();

    void setDepletedLevel(int level);
    int getDepletedLevel();

    bool mustCharge();
    bool isBeingCharged();
    bool isFullyCharged();

    void updateSOC();
    void resetSOC();
    int getSOC();
    word readBatteryAndCalcValue();

  private:
    int batType;
    int batSocpin;
    int batDockpin;
    int fullyChargedLevel;
    int depletedLevel;
    word averageSOC;
};

#endif /* _BATTERY_H_ */
