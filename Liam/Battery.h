// This is the library for a Battery
//
/* Changelog:
   	2014-12-12 - Jonas
   		Initial version

	2015-06-10 - Morgan M
		voltage value x100
		Filter value for making it possible to set how hard to filter readings(high loads could trigger a emty state)
		Voltage divator, to compensate for differnt PCB settings
		readBatteryAndCalcValue, function for reading the soc pin
		averageSOC size is now word(0-65535);

============================================
Placed under the GNU license

===============================================
*/
#include <Arduino.h>

#ifndef _BATTERY_H_
#define _BATTERY_H_

//All voltage value need to be x100 to avoid decimals (12.54V = 1254)

#define FILTER  			5    //how hard to filter voltage readings
#define VOLTDIVATOR			40   //Voltage divider faktor x10 to avoid decimal ( 4 = 40)


class BATTERY {
    public:
    enum BATTERY_TYPE
      {
        LEAD_ACID=0,
        NIMH,
        LIION
      };
        BATTERY(BATTERY_TYPE type, int socpin, int dockpin);

        //
        void setBatterType(BATTERY::BATTERY_TYPE type);
        int getBatteryType();

        void setFullyChargedLevel(int level);
        int getFullyChargedLevel();

        void setDepletedLevel(int level);
        int getDepletedLevel();

        void setGoHomeLevel(int level);
        int getGoHomeLevel();

        bool mustCharge();
        bool isBeingCharged();
        bool isFullyCharged();

        void updateSOC();
        void resetSOC();
        int getSOC();
		    word readBatteryAndCalcValue();


    private:
      /*int LIIONFULL=1260;
      int LIIONEMPTY=1040;
      int NIMHFULL=1450;
      int NIMHEMPTY=1150;
      int LEADACIDFULL=1330;
      int LEADACIDEMPTY=1200;
*/
    	BATTERY_TYPE batType=BATTERY_TYPE::LIION;
    	int batSocpin;
    	int batDockpin;
    	int fullyChargedLevel;
    	int depletedLevel;
      int gohomeLevel;
    	word averageSOC;
      int numbersOfFullyRead=0;
};

#endif /* _BATTERY_H_ */
