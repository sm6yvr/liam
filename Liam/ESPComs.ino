

#include "Battery.h"                  //Need to include Battery.h to access battery readings 
#include "SetupDebug.h"
#include "Arduino.h"
#include "BWFSensor.h"

/* INSTRUCTIONS
 * PLACE THIS FILE IN THE LIAM FOLDER. YOU NEED TO MODIDFY THE ORIGINAL CONTENT TO MAKE THIS WORK.
 * MOVE THE COMMENTED SECTIONs BELOW TO RESPECTIVE PARTS
 * GITHUB: https://github.com/WilliamLehmus/ConnectedLIAM
 */

/*
  //COPY THIS IF STATEMENT TO VOID LOOP
  if ((millis() - lastUpdate) > interval) {   //Place this if statement to void loop
    mowerData.activity = state;
    connectedLiam();
  }

  //------COPY THE SECTION BELOW TO GLOBAL VARIABLES------
  
  //TIMER
  unsigned long lastUpdate;           //Move to global variables
  int interval = 1000;                //Move to global variables

  struct MOWERDATA {          // move to global variable
    String activity;          //Current activity, I.E mowing, looking for signal, charging etc.
    int SoC;                  //Current battery level
    String SoCString;         //Current battery level in string format 
    float batMin;             //Minimum defined level of battery. Depleted level.
    float batMax;             //Maximum defined level of battery. SoC when to consider charging complete
    int leftBWF;              //Status of left BWF. IN (1) or OUT (0).
    int rightBWF;             //Status of left BWF. IN (1) or OUT (0).
    int leftRearBWF;          //Status of rear BWF. IN (1) or OUT (0).
    int rightRearBWF;         //Status of rear BWF. IN (1) or OUT (0).
    bool insideCable;         //Variable used to check if the mower is inside or outside the BWF or cannot see it.
  } mowerData;                //Object name. Fetch or edit data by calling for instance mowerData.SoC = batteryLevel;


//-----DONT COPY ANYTHING BELOW THIS POINT. --------
*/



void connectedLiam() {
  //Read battery level and format
  mowerData.SoC = Battery.getVoltage();
  if (mowerData.SoC > 4000 && mowerData.SoC < 10000) {
    int tempSoC = mowerData.SoC / 10;
    mowerData.SoCString = "0" + String(tempSoC);
  }
  else {
    int tempSoC = mowerData.SoC / 10;
    mowerData.SoCString = String(tempSoC);
  }

  //BWF Sensors
  if (!Sensor.isOutOfBounds(0)) mowerData.leftBWF = 1;      //Sensor is inside
  else mowerData.leftBWF = 0;                               //Sensor is outside

  if (!Sensor.isOutOfBounds(1)) mowerData.rightBWF = 1;     //Sensor is inside
  else mowerData.rightBWF = 0;                              //Sensor is outside

  if (mowerData.leftBWF == 1 || mowerData.rightBWF == 1) mowerData.insideCable = true;
  else mowerData.insideCable = false;

  //Format activity if none is reported from Liam
  if (mowerData.activity == "") mowerData.activity = '6';
  //Print for debugging
  Serial.println("Activity: " + mowerData.activity);
  Serial.println("SoCString: " + mowerData.SoCString);                          //State of Charge, battery level
  Serial.println("SoC Float: " + String(mowerData.SoC));                          //State of Charge, battery level
  Serial.println("Inside Cable: " + String(mowerData.insideCable));
  connectedSend();                                                        //Send data
} //ENd request event

//Send data. Data is sent in this format: @Activity+SoC+InsideCable;. For instance @416801;
void connectedSend() {
  char startBit = '@';
  char endBit = ';';
  String activity = startBit + mowerData.activity + mowerData.SoCString + mowerData.insideCable + endBit;
  Serial.println(activity);
}
