#include <Arduino.h>

int step_pins[] {3, 5, 6};
int dir_pins[] {2, 4, 6};

int steps_per_rev[] {400, 400, 400};

void step_stepper(int motor_id, int num_steps, float rpm=120) {

  float step_delay = (60 / (steps_per_rev[motor_id] * rpm)) * 1000000;

  digitalWrite(dir_pins[motor_id], (num_steps < 0) ? LOW : HIGH);

  for (int i = 0; i < num_steps; i++) {
    digitalWrite(step_pins[motor_id], HIGH);
    delayMicroseconds(step_delay);
    digitalWrite(step_pins[motor_id], LOW);
    delayMicroseconds(step_delay);
  }
}

void step_angle(int motor_id, int angle, float rpm=120) {
  /*
  Steps a stepper motor a certain angle
  */
  float step_angle = 360 / steps_per_rev[motor_id];

  float num_steps = angle / step_angle;

  step_stepper(motor_id, num_steps, rpm);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Starting...");

  for (int i = 0; i < 3; i++) {
    pinMode(step_pins[i], INPUT);
    pinMode(dir_pins[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  String motor_id = "";
  String num_steps_str = "";

  bool is_motor_char = true;
  while (Serial.available()) {
    char c = Serial.read();

    if (c == ' ') { is_motor_char = false; }

    if (is_motor_char) {
      motor_id += c;
    } else {
      num_steps_str += c;
    }

  }
}