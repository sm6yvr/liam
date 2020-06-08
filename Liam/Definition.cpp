/*
 Liam - DIY Robot Lawn Mower

 Default Definitions

 ======================
  Licensed under GPLv3
 ======================
*/

/*
  This file holds all configuration parameters, user
  specific, common and software specific parameters.
*/

#include "Definition.h"


/* Define all pins */
void DEFINITION::definePinsInputOutput() {

  pinMode(WHEEL_MOTOR_A_CURRENT_PIN, INPUT);
  pinMode(WHEEL_MOTOR_B_CURRENT_PIN, INPUT);
  pinMode(BAT_PIN, INPUT);
  pinMode(CUTTER_CURRENT_PIN, INPUT);

  /*  Some pins are better leave undefined as default
      pinMode(I2C_SDA_PIN, INPUT);
      pinMode(I2C_SDL_PIN, OUTPUT);

      pinMode(RX_PIN, INPUT);
      pinMode(TX_PIN, OUTPUT);
      */
  pinMode(BWF_SENSOR_INPUT_PIN, INPUT);
  pinMode(WHEEL_MOTOR_A_PWM_PIN, OUTPUT);
  pinMode(BWF_SELECT_A_PIN, OUTPUT);
  pinMode(DOCK_PIN, INPUT);
  pinMode(CUTTER_PWM_PIN, OUTPUT);
  pinMode(BWF_SELECT_B_PIN, OUTPUT);

#if __Bumper__
  pinMode(BUMPER, INPUT);
  digitalWrite(BUMPER, HIGH);
#else
  pinMode(BUMPER, OUTPUT);
  digitalWrite(BUMPER, LOW);
#endif

#if __Lift_Sensor__
  pinMode(LIFT_SENSOR_PIN, INPUT);
  digitalWrite(LIFT_SENSOR_PIN, HIGH);
#else
  pinMode(LIFT_SENSOR_PIN, OUTPUT);
  digitalWrite(LIFT_SENSOR_PIN, LOW);
#endif

  pinMode(LED_PIN, OUTPUT);
  pinMode(WHEEL_MOTOR_B_PWM_PIN, OUTPUT);
  pinMode(WHEEL_MOTOR_A_DIRECTION_PIN, OUTPUT);
  pinMode(WHEEL_MOTOR_B_DIRECTION_PIN, OUTPUT);

}

void DEFINITION::setDefaultLevels(BATTERY* battery, WHEELMOTOR* left, WHEELMOTOR* right, CUTTERMOTOR* cutter) {
  left->setOverloadLevel(WHEELMOTOR_OVERLOAD);
  right->setOverloadLevel(WHEELMOTOR_OVERLOAD);
  cutter->setOverloadLevel(CUTTER_OVERLOAD);
}
