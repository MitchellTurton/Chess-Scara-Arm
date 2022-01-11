#include <Arduino.h>
#include "Joint.hpp"

Joint joint[] {RotationJoint(2.0, 2, 3, 400), RotationJoint(2.0, 4, 5, 400), LinearJoint(8, 6, 7, 400)};
float J1 = 0;
float J2 = 0;


void calc_angles(float x, float y) {
  int l1 = 150;
  int l2 = 200;
  float d = sqrt(x*x + y*y);

  double theta1 = acos((l1*l1 + d*d - l2*l2) / (2*l1*d));
  double theta2 = acos((l2*l2 + l1*l1 - d*d) / (2*l2*l1));

  J1 = theta1 + acos(x/d);
  J2 = theta2 - (PI/2);

  Serial.print("J1: "); Serial.print(theta1); Serial.print(" J2: "); Serial.println(theta2);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Starting...");

  for (int i = 0; i < 3; i++) {
    pinMode(joint[i].getDirPin(), OUTPUT);
    pinMode(joint[i].getStepPin(), OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  String motor_id = "";
  String num_steps_str = "";

  String x_val = "";
  String y_val = "";

  bool is_motor_char = true;
  while (Serial.available()) {
    char c = Serial.read();

    if (c == ' ') { is_motor_char = false; }

    if (is_motor_char) {
      // motor_id += c;
      x_val += c;
    } else {
      // num_steps_str += c;
      y_val += c;
    }

    delay(2);
  }

  // if (motor_id.length() + num_steps_str.length() > 0) {
  //   int num_steps = num_steps_str.toInt();

  //   Serial.println("Motor: " + motor_id + " Number of Steps: " + num_steps_str);

  //   joint[motor_id.toInt()].step_motor(num_steps);
  // }

  if (x_val.length() + y_val.length() > 0) {
    Serial.println("X: " + x_val + " Y: " + y_val);

    calc_angles(x_val.toInt(), y_val.toInt());
  }
}
