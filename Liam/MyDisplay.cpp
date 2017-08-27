// This is the library for a Display
// It will by default write to serial port, but can be replaced by subclasses for
// a built in LCD or OLED for example
// Changelog:
//     2015-08-05 - Initial version by Jonas

/* ============================================
Placed under the GNU license

===============================================
*/

#include "MyDisplay.h"

MYDISPLAY::MYDISPLAY(BATTERY* batt, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, int* state)
{
	Battery = batt;
	leftMotor = left;
    rightMotor = right;
    cutter = cut;
    sensor = bwf;
	compass = comp;
	moverstate = state;
}

// Do not override
boolean MYDISPLAY::initialize() {
	char buffer [9]; //Format 09.00.00

	current_row = 0;
	current_col = 0;

  // Removed this so that Liam can start with SerialCommand..
	//for (int i=0; i<3; i++)
		//blink();

	setCursor(0, 0);
	print("SW version:");

  	sprintf (buffer, "%d.%d.%d ", MAJOR_VERSION, MINOR_VERSION_1, MINOR_VERSION_2);
  	print(buffer);

	print(__DATE__);

	print(__TIME__);
  // Removed in order to get faster start, I donät know if something could be out of sync because of this..
  //delay(3000);
	clear();
}

// Do NOT override. Implement basic commands here
void MYDISPLAY::update() {
	int sens = 0;

    // Rad 1: Sensors
	#if __MS9150__ || __MS5883L__ || __ADXL345__ || __MMA7455__
	    setCursor(0,0);
	    print("Comp:");
	    setCursor(7,0);
	    print(compass->getHeading());
    #else
	    setCursor(0,0);
	    print("Sens:");
	    setCursor(7,0);
	    print("Disabled");
    #endif

    setCursor(10,0);
    print("InOut:");
    setCursor(17,0);
    print(sensor->isInside());

    //Rad 2: Motor loading
    setCursor(0,1);
    print("LMoto:");
    setCursor(7,1);
    print(leftMotor->getLoad());
    setCursor(10,1);
    print("RMoto:");
    setCursor(17,1);
    print(rightMotor->getLoad());

    //Rad 3: Battery data
    setCursor(0,2);
    print("Batt Soc:");
    setCursor(10,2);
    print(Battery->getSOC());

    //Rad 4: State and Error data
    setCursor(0,3);
    print("State:");
    setCursor(7,3);

    switch (*moverstate) {
    case DEFINITION::CUTTERSTATES::MOWING:
      print("MOWING");
      break;
    case DEFINITION::CUTTERSTATES::LAUNCHING:
      print("LAUNCHING");
      break;
    case DEFINITION::CUTTERSTATES::DOCKING:
      print("DOCKING");
      break;
    case DEFINITION::CUTTERSTATES::CHARGING:
      print("CHARGING");
      break;
    }
}

// This is the basic implementation of the print and println command
// Override this for each type of display
size_t MYDISPLAY::write(uint8_t s) {
	// By default just write to serial port
	Serial.write(s);
}

// Override this for each type of display
void MYDISPLAY::setCursor(int col, int row) {
	// For a serial port, do nothing
	current_col = col;
	current_row = row;
}

// Override this for each type of display
void MYDISPLAY::clear() {
	// For a serial port, do very little
	println();
	setCursor(0,0);
}

// Override this for each type of display
void MYDISPLAY::blink() {
	// For a serial port, do very little
	println("");
	delay(1000);
	//println("**********************");
  println("*");
	delay(1000);
	setCursor(0,0);
}
