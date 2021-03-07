


/* THIS IS THE CODE FOR THE CONNECTED LIAM MOD. I'VE USED A NODE-MCU MICROCONTROLLER FOR THIS MOD TO PROVIDE WIFI.
   THE NODE MCU NEEDS A 3.3V-9V POWER SOURCE AND THE PINS RUN ON 3.3V LOGIC BUT THE PINS ARE ALSO 5V TOLERANT
   WHICH MAKES IT EASY TO CONNECT THE ARDUINO. BLUETOOTH CANNOT BE USED AT THE SAME TIME AS THIS MOD BUT YOU
   HAVE A COMPLETE TERMINAL IN THE APP INSTEAD.

   THIS MOD REQUIRES A LIAM DIY LAWNMOWER AND A NODE-MCU (ESP8266) BASED MICROCONTROLLER AND THE FREE BLYNK APP.
   THERE ARE TWO FILES IN THIS ADDON. 
   ESPcoms.ino should be placed in the LIAM folder. FOLLOW INSTRUCTIONS IN THE FILE.
   LIAM_ESP_ADDON should be uploaded to the NODE-MCU
   THE CODE ALLOWS FOR TWO WIFI CONNECTIONS WHICH IS CONVENIENT IF YOU ARE TRYING THIS OUT OF RANGE OF YOUR ROUTER
   YOU CAN SET THE PRIMARY WIFI AS YOUR PHONE WIFI ACCESS POINT AND THE SECONDARY AS YOUR ROUTER. 
   NODE-MCU WILL AUTOMATICALLY REVERT BACK TO YOUR ROUTER WIFI IF CONNECTION IS LOST WITH PHONE OR VICE VERSA

   HOW TO CONNECT:
   ARDUINO GND - NODEMCU GND
   ARDUINO RX - NODEMCU TX (Bluetooth pin on Morgan shield)
   ARDUINO TX - NODEMCU RX (Bluetooth pin on Morgan shield)

   Data is expeced in this form:
   '116801'. This string means. Activity: Launching, Voltage: 16.80v, Mower is inside BWF
   Char 1: Control Char: The code looks for the char '@' that signifies that it´s data coming from the connected Liam addon. Otheriwi
   Char 2: Activity. Datarange: 1-6 see at the bottom of this page to see what the numbers mean
   Char 3: Voltage. Datarange: 0-9
   Char 4: Voltage. Datarange: 0-9
   Char 5: Voltage. Datarange:0-9
   Char 6: Voltage. Datarange: 0-9
   Char 7: Inside or outside BWF. Char conversions gives this a 49 (1)=Inside, 48(0) Outside.

    
    Blynk Virtual Channels:
    BASE STATION:
    V28 BASE STATION RSSI
    V29 BASE STATION CABLE INTEGRITY
    V30 BWF Control ON/OFF
    V31 BASE STATION CHARGER CONTACT ON/OFF

    MOWER:
    V35 CURRENT ACTIVITY
    V36 BATTERY SOC
    V37 Inside/Outside Cable
    V38 RESERVED
    V39 RESERVED
    V40 MOWER RSSI
    V41 Segmented switch. 1) Cut my lawn 2) Go home
    v42 TERMINAL
*/

//#include <ESP8266WiFi.h>
/*
#include <BlynkSimpleEsp8266.h>
WidgetTerminal terminal(V42);
bool debug = false;                //Setting this to true will start outputing over Serial. Disconnect TX to Liam before doing this

//BLYNK
char auth[] = "1xvpeeVA2lRbNO5m6qBpTyDNbf_vJHl0"; //This is your blynk token that is generated from the app.

//WIFI
//PRIMARY WIFI
const char* ssid1 =      "NETGEAR83";    //Your wifi SSID
const char* password1 =  "";    //Your wifi password

//SECONDARY WIFI. Falls back to this if first wifi attempt fails
const char* ssid2 =      "";    //Your wifi SSID
const char* password2 =  ""; //Your wifi password
int connectionAttempts = 20;   //Number of attempts before trying nr2

//GLOBALS
String LiamData = "";
int LED1 = D0;

struct MOWERDATA {
  String activity;          //Current activity, I.E mowing, looking for signal, charging etc.
  float SoC;                //Current battery level
  float batMin;             //Minimum defined level of battery. Depleted level.
  float batMax;             //Maximum defined level of battery. SoC when to consider charging complete
  int leftBWF;              //Status of left BWF. IN (1) or OUT (0).
  int rightBWF;             //Status of left BWF. IN (1) or OUT (0).
  int leftRearBWF;          //Status of rear BWF. IN (1) or OUT (0).
  int rightRearBWF;         //Status of rear BWF. IN (1) or OUT (0).
  char insideCable;
} mowerData;               //Object name. Fetch or edit data by calling for instance mowerData.SoC = batteryLevel;


//Timer for how often to send the information from the mower
unsigned long lastTrig = 0; //Do not change this
int interval = 1000;        //How often to send the data. 1000ms = 1 sek.

void setup() {
  Serial.begin(115200);                                   //Init serial monitor
  checkConnection();
  pinMode(LED1, OUTPUT);                                    //Built in led #1

}


void loop() {
  Blynk.run();
  timer();                //Timer for how often to upload data to blynk
  checkConnection();      //Auto connection and restoring of connection.
}

void timer() {  //get data from liam with set interval
  if ((millis() - lastTrig) > interval) {
    if (Serial.available()) {  //If done receiving, send to blynk
      updateBlynk();            
      getDataFromLiam();
    }
    lastTrig = millis();
  }
}

void sendDataToLiam(String message) {      //Not used right now


}

void getDataFromLiam() {
  //This algorithm will extract the data string and discard all other serial.print commands coming from Liam
  LiamData = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '@') {
      LiamData = LiamData + c;
      while (Serial.available()) {
        char d = Serial.read();
        if (d == ';') {
          LiamData = LiamData + d;
          break;
        }//End if
        else LiamData = LiamData + d;
      }//End while
    } //End if
  } //End while

  if (LiamData != "") { //Update data if string is not empty
    if (debug)Serial.println(LiamData);
    decodeActivity(LiamData);

  }
} //End getdatafromliam

void updateBlynk() {
  if (debug)Serial.println("RAW Liamdata: " + LiamData);
  if (debug)Serial.println("Activity: " + String(mowerData.activity));
  if (debug)Serial.println("SoC: " + String(mowerData.SoC));
  if (debug)Serial.println("Inside Cable: " + String(mowerData.insideCable));
  Blynk.virtualWrite(V35, mowerData.activity);
  Blynk.virtualWrite(V36, mowerData.SoC);
  Blynk.virtualWrite(V37, mowerData.insideCable);
  Blynk.virtualWrite(V40, WiFi.RSSI());
  //long rssi = WiFi.RSSI();
  terminal.println("RawData: " + LiamData);
  terminal.println("Activity: " + String(mowerData.activity));
  terminal.println("SoC: " + String(mowerData.SoC));
  terminal.println("Inside Cable: " + String(mowerData.insideCable));


}

void decodeActivity(String incoming) {
  char activity = incoming.charAt(1); //Byte 0 is control character. 1 is activity: Mowing, charging etc..
  if (activity == '0') {
    mowerData.activity = "MOWING";
  }
  if (activity == '1') {
    mowerData.activity = "LAUNCHING";
  }
  if (activity == '2') {
    mowerData.activity = "DOCKING";
  }
  if (activity == '3') {
    mowerData.activity = "CHARGING";
  }
  if (activity == '4') {
    mowerData.activity = "LOOKING FOR BWF";
  }
  if (activity == '5') {
    mowerData.activity = "SETUP DEBUG";
  }
  if (activity == '6') {
    mowerData.activity = "IDLE";
  }
  //STATE OF CHARGE
  char SoC[6]; //Four numbers denotes SoC. Char arrays are null terminated.
  SoC[0] = incoming.charAt(2);
  SoC[1] = incoming.charAt(3);
  SoC[2] = '.';
  SoC[3] = incoming.charAt(4);
  SoC[4] = incoming.charAt(5);
  SoC[5] = '\0';
  String SoCString = String(SoC);
  //Serial.println("SoCString: " + SoCString);
  mowerData.SoC = SoCString.toFloat();
  //if (incoming.charAt(6) == '0') mowerData.insideCable == "Outside";
  //else if (incoming.charAt(6) == '1') mowerData.insideCable == "Inside";
  mowerData.insideCable = incoming.charAt(6);
} //End decode activity


/*
  BLYNK_READ(V35) {                                       //Read data from ESP to Blynk app
  Blynk.virtualWrite(V35, LiamData);

  }
*/
/*
//COMMANDS FROM APP TO LIAM. 1) CUT MY LAWN, 2) GO HOME. NOT USED YET.
BLYNK_WRITE(V41) //Recieve data from app terminal and send to Liam
{
  int action = param.asInt();
  if (action == 1) {  //Cut my lawn
    Serial.print('d');
    Serial.print('m'); 
    terminal.println("Sent Command: Cut my Lawn");
    terminal.flush();
  }
  
  if (action == 2) { //Go home
    Serial.print('d');
    Serial.print('b');   
    terminal.println("Sent Command: Look for BWF and dock");
    terminal.flush(); 
  }

}

//TWO WAY COMMUNICATION WITH LIAM THROUGH BLYNK TERMINAL
BLYNK_WRITE(V42) //Recieve data from app terminal and send to Liam
{
  String action = param.asStr();
  Serial.print(action);                   //Forward to Liam via Serial
  terminal.println("You Send Command: ");
  terminal.write(param.getBuffer(), param.getLength());
  terminal.println();
  terminal.flush();
}

void checkConnection() {
 int connectionTimer = 0;
 if (WiFi.status() != WL_CONNECTED) {
  digitalWrite(LED1, LOW);
  if (debug) Serial.println("No Connection. Trying to connect");
  WiFi.begin(ssid1,password1);                           
  while (WiFi.status() != WL_CONNECTED) {              
    if (debug) Serial.println("Connecting to: " + String(ssid1));                    
    delay(500);                                         
    connectionTimer++;
    if (connectionTimer >= connectionAttempts) {
      if (debug) Serial.println("Failed to connect to " + String(ssid1));
      WiFi.begin(ssid2,password2);
      connectionTimer = 0;
      while (WiFi.status() != WL_CONNECTED) {              
        if (debug) Serial.println("Connecting to: " + String(ssid2));                    
        delay(500);
        connectionTimer++;
        if (connectionTimer >= connectionAttempts) return;
      }
    if (debug) Serial.println("Connected to internet");
    }
  }
  Blynk.config(auth);                                 //CONNECT TO BLYNK WITH THE AUTH TOKEN PROVIDED BY THE APP
  while (Blynk.connect() == false) {                  
    if (debug) Serial.println("Connecting to Blynk");  
    delay(500);
  }
  if (debug) Serial.println("Connected to Blynk");
  }
  else {
    digitalWrite(LED1, HIGH);
  }
}
*/
