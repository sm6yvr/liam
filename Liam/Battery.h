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

#define LEAD_ACID            0 // this device only has one address
#define NIMH    			 1
#define LIION    			 2

//All voltage value need to be x100 to avoid decimals (12.54V = 1254)
#define LIIONFULL			1340	
#define LIIONEMPTY			1100
#define NIMHFULL			1450
#define NIMHEMPTY			1150
#define LEADACIDFULL		1330
#define LEADACIDEMPTY		1200

#define FILTER  			5    //how hard to filter voltage readings
#define VOLTDIVATOR			42   //Voltage divider faktor x10 to avoid decimal ( 4 = 40)


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
