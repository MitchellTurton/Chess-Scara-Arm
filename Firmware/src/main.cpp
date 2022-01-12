#include <Arduino.h>
#include "Joint.hpp"

RotationJoint joint[] {RotationJoint(2.0, 2, 3, 400), RotationJoint(2.0, 4, 5, 400)};
LinearJoint elevator = LinearJoint(8, 6, 7, 400);

float* calc_angles(float x, float y) {
  float l1 = 180.f;
  float l2 = 180.f;
  float d = sqrt(x*x + y*y);

  double theta1 = acos((l1*l1 + d*d - l2*l2) / (2*l1*d));
  double theta2 = acos((l2*l2 + l1*l1 - d*d) / (2*l2*l1));

  float J1 = theta1 + acos(x/d);
  float J2 = theta2 - (3.14159/2);

  Serial.print("J1: "); Serial.print(theta1); Serial.print(" J2: "); Serial.println(d);

  float angles[] {J1, J2};

  return angles;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Starting...");

  for (int i = 0; i < sizeof(joint); i++) {
    pinMode(joint[i].getDirPin(), OUTPUT);
    pinMode(joint[i].getStepPin(), OUTPUT);
  }

  pinMode(elevator.getDirPin(), OUTPUT);
  pinMode(elevator.getStepPin(), OUTPUT);
}

void test() {
  int x = 150;
  int y = 150;

  while (x > -150) {
    float* angles = calc_angles(x, y);

    joint[0].setTargetAngle(angles[0]);
    joint[1].setTargetAngle(angles[1]);
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
      motor_id += c;
      x_val += c;
    } else {
      num_steps_str += c;
      y_val += c;
    }

    delay(2);
  }

  // if (motor_id.length() + num_steps_str.length() > 0) {
  //   int num_steps = num_steps_str.toInt();

  //   Serial.println("Motor: " + motor_id + " Number of Steps: " + num_steps_str);

  //   joint[motor_id.toInt()].setNeededSteps(num_steps);
  // }

  // for (int i = 0; i < sizeof(joint); i++) {
  //   joint[i].update();
  // }
  if (x_val.length() + y_val.length() > 0) {
    Serial.println("X: " + x_val + " Y: " + y_val);

    calc_angles(x_val.toInt(), y_val.toInt());
  }
}
