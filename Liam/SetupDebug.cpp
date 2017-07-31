
#include "SetupDebug.h"
#include "Definition.h"

SETUPDEBUG::SETUPDEBUG(WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cut, BWFSENSOR* bwf, MOTIONSENSOR* comp, BATTERY* batt) {
    leftMotor = left;
    rightMotor = right;
    cutter = cut;
    sensor = bwf;
    compass = comp;
    battery = batt;
    _Serial = NULL;
    pitch = 0;
}

void SETUPDEBUG::initialize(HardwareSerial *serIn) {
  _Serial = serIn;
  _Serial->begin(115200);
  _Serial->println("Welcome to Liam Test Program");
  _Serial->println("Send 'H' for list of commands");
}

void SETUPDEBUG::startListeningOnSerial() {

  char inChar;
  while (!Serial.available());			// Stay here until data is available
  inChar = (char)Serial.read();	// get the new byte:

  switch (inChar) {
      case 'H':
      case 'h':
        SETUPDEBUG::printHelp();
        break;

      case 'D':
      case 'd':
        SETUPDEBUG::toggleLed();
        break;

      case 'L':
      case 'l':
        SETUPDEBUG::toggleWheelLeft();
        break;

      case 'R':
      case 'r':
        SETUPDEBUG::togglewheelRight();
        break;

      case 'S':
      case 's':
        SETUPDEBUG::getBwfSignals();
        break;

        case 'C':
        case 'c':
            if (cutter_motor_is_on) {
              Serial.println("Ramping down cutter");
              for (int i=100; i>=0; i--) {
                cutter->setSpeed(i);
                delay(10);
              }
              Serial.println("Ramp down completed");
              cutter_motor_is_on = false;
            }
            else
            {
              Serial.println("Ramping up cutter");
              for (int i=0; i<100; i++) {
                cutter->setSpeed(i);
                delay(10);
              }
              Serial.println("Ramp up completed");
              cutter_motor_is_on = true;
            }
        break;

        case 'T':
        case 't':
            for (int i=0; i<100; i++) {
              sensor->select(0);
              delay(100);
              rightMotor->setSpeed((sensor->isInside()?100:-100));

              sensor->select(1);
              delay(100);
              leftMotor->setSpeed((sensor->isInside()?100:-100));
            }
            leftMotor->setSpeed(0);
            rightMotor->setSpeed(0);
        break;

        case '+':
            cutterspeed += 10;
            cutter->setSpeed(cutterspeed);
            Serial.println(cutterspeed);
        break;

        case '-':
            cutterspeed -= 10;
            cutter->setSpeed(cutterspeed);
            Serial.println(cutterspeed);
        break;

        case 'P':
        case 'p':
                Serial.print(" LMot: ");
                Serial.print(leftMotor->getLoad());
                Serial.print(" RMot: ");
                Serial.print(rightMotor->getLoad());
                Serial.print(" SOC: ");
                battery->resetSOC();
                Serial.print(battery->getSOC());
                Serial.print(" Dock: ");
                Serial.print(battery->isBeingCharged());

        break;

        case 'e':
        case 'E':
            if (cutter_is_attached) {
            //  cutter->detachMotor();
              Serial.println("Cutter is detached");
            }
            else {
            //  cutter.initialize();
              Serial.println("Cutter is attached");
            }
            cutter_is_attached = !cutter_is_attached;

        break;

        case 'g':
        case 'G':
            #if __MMA7455__
              compass->autoupdate();
            #endif

            tilt_angle = compass->getTiltAngle();

            x = compass->getXAngle();
            y = compass->getYAngle();
            z = compass->getZAngle();

            Serial.print("Z = ");
            Serial.println(z);
            Serial.print("Y = ");
            Serial.println(y);
            Serial.print("X = ");
            Serial.println(x);

            Serial.print("Tilt angle = ");
            Serial.println(tilt_angle);

            // Compass.getHeading();
            // Compass.headingVsTarget();
            // Compass.updateHeading();

            // my9150.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
            // Serial.print("TiltXZ: ");
            // Serial.print(abs(atan2(ax,az))* 180 / M_PI);
            // Serial.print(" TiltYZ: ");
            // Serial.print(abs(atan2(ay,az))* 180 / M_PI);
            // Serial.print("Heading: ");
            // Serial.print(current_heading = atan2(my,mz)* 180 / M_PI);
            // Serial.print("Diff to last heading: ");
            // Serial.println(copysign(1.0,current_heading - target_heading) *
            // copysign(1.0,abs(current_heading-target_heading)-180) *
            // (180-abs(abs(current_heading-target_heading)-180)));
            // target_heading = current_heading;

        break;

  }
  //   case 'g':
  //   case 'G':
  //   break;
  //
  //
  // }
  //     Serial.println("P = print SOC & debug values");
  //     Serial.println("E = Cutter motor calibrate");

  inChar = 0;


}

void SETUPDEBUG::printHelp() {
      _Serial->println("------- Help menu ------------");
      _Serial->println("L = Left Wheel motor on/off");
      _Serial->println("R = Right Wheel motor on/off");
      _Serial->println("C = Cutter motor on/off");
      _Serial->println("S = test BWF Sensor");
      _Serial->println("G = test Gyro/Compass/Accelerometer");
      _Serial->println("D = turn LED on/off");
      _Serial->println("T = make a 10 second test run");
      _Serial->println("P = print SOC & debug values");
      _Serial->println("E = Cutter motor calibrate");
}

void SETUPDEBUG::toggleWheelLeft() {
  if (left_wheel_motor_is_on == true) {
    Serial.println("Ramping down left wheel");
      for (int i=100; i>0; i--) {
         leftMotor->setSpeed(i);
         delay(10);
      }
       Serial.println("Ramp down completed");
       left_wheel_motor_is_on = false;
  } else {
       Serial.println("Ramping up left wheel");
       for (int i=0; i<100; i++) {
         leftMotor->setSpeed(i);
         delay(10);
       }
       Serial.println("Ramp up completed");
       left_wheel_motor_is_on = true;
  }
}

void SETUPDEBUG::togglewheelRight() {
  if (right_wheel_motor_is_on == true) {
      Serial.println("Ramping down right wheel");
      for (int i=100; i>0; i--) {
        rightMotor->setSpeed(i);
        delay(10);
      }
      Serial.println("Ramp down completed");
      right_wheel_motor_is_on = false;
    } else {
      Serial.println("Ramping up right wheel");
      for (int i=0; i<100; i++) {
        rightMotor->setSpeed(i);
        delay(10);
      }
      Serial.println("Ramp up completed");
      right_wheel_motor_is_on = true;
    }
}

void SETUPDEBUG::toggleCutterMotor() {

}

void SETUPDEBUG::updateBWF() {
  sensor->readSensor();
}
void SETUPDEBUG::getBwfSignals() {
      Serial.println("-------- Testing Sensors 0 -> 3 --------");
      for (int i=0; i<4; i++) {
        sensor->select(i);
        delay(1000);
        Serial.print(i);
        Serial.print(": ");
        sensor->printSignal();
        Serial.print(" in:");
        Serial.print(sensor->isInside());
        Serial.print(" out:");
        Serial.print(sensor->isOutside());
        Serial.println();
      }
      Serial.println("Sensor test completed");
}

void SETUPDEBUG::getMotionValues() {

}

void SETUPDEBUG::toggleLed() {

  if (led_is_on)
    digitalWrite(LED_PIN,LOW);
  else
    digitalWrite(LED_PIN,HIGH);

  led_is_on = (led_is_on?false:true);
}
