#include <Arduino.h>
#include "Joint.hpp"

// Global Variables -------------------------------------------

// Lengths of each arm segment for inv. kin. calculations
const float l1 = 150.f;
const float l2 = 275.f;

// Pin the magnet is connected to + if it is enabled or not
const int mag_pin = 9;
bool magIsEnabled;

const float elevator_rest_height = 100.0;

// The two types of joint classes representing each motor
RotationJoint joint[2]; 
LinearJoint elevator; 


// Struct for storing moves ------------------------------------
struct Move {
  /*
  Struct to hold the position information for a move
  */

  Move(String fenMove, float pieceHeight, float captureHeight) {
    this->pos1[0] = ((int) fenMove[0]) - 96;
    this->pos1[1] = (int) fenMove[1];

    this->pos2[0] = ((int) fenMove[2]) - 96;
    this->pos2[1] = (int) fenMove[3];

    this->pieceHeight = pieceHeight;
    this->captureHeight = captureHeight;
  }

  Move(String fenMove, float pieceHeight) {
    Move(fenMove, pieceHeight, 0);
  }

  int pos1[2];
  int pos2[2];
  float pieceHeight;
  float captureHeight;
};


// Utility functions ------------------------------------------

void go_to_pos(float x, float y) {
  /*
  Calculates the angles and moves the motors to position
  the end effector over the given (x, y) coordinate
  */

  float d = sqrt(x*x + y*y);  // Distance

  // Using Law of cosines to calculate local thetas
  double theta1 = acos((l1*l1 + d*d - l2*l2) / (2*l1*d));
  double theta2 = acos((l2*l2 + l1*l1 - d*d) / (2*l2*l1));

  // Converting to global angle 
  float J1 = theta1 + acos(x/d); 
  float J2 = theta2 - (PI/2);

  // Converts from radians to degrees
  J1 *= 180/PI;
  J2 *= 180/PI;
  
  // Stepping motors to position
  joint[0].step_motor_toangle(J1);
  joint[1].step_motor_toangle(J2);
}


void go_to_square(int row, int col) {
  /*
  Moves end effector over specified square
  */
  go_to_pos(-85 + ((float) col - 1) * 35.75, 150 + ((float) row - 1) * 35.75);
}


void play_move(int init_x, int init_y, int final_x, int final_y, float piece_height) {
  /*
  Takes the position information and performs the sequence of moves
  */
  
  // Moving piece
  elevator.step_to_pos(elevator_rest_height);
  go_to_square(init_x, init_y);
  elevator.step_to_pos(piece_height);
  
  if (digitalRead(12) == HIGH) {
    delay(500);
    digitalWrite(mag_pin, HIGH);
    delay(1000);
  }
  
  elevator.step_to_pos(elevator_rest_height);
  go_to_square(final_x, final_y);
  elevator.step_to_pos(piece_height);
  
  if (digitalRead(12) == HIGH) {
    delay(500);
    digitalWrite(mag_pin, LOW);
    delay(1000);
  }
  elevator.step_to_pos(elevator_rest_height);
}


void read_input() {
  /*
  Reads from the Serial input
  */

  String block1 = "";
  String block2 = "";

  bool is_second_block = true;
  while (Serial.available()) {
    char c = Serial.read();

    if (c == ' ') {
      is_second_block = false;
    } else {
      
      if (is_second_block) {
        block1 += c;
      } else {
        block2 += c;
      }

    }
    delay(2);
  }

  // Commands based on message from serial
  if (block1.length() + block2.length() > 0) {
    Serial.println("Block1: " + block1);

    if (block1 == "mag") {

      if (magIsEnabled) {
        Serial.println("Magnet Disabled");
        magIsEnabled = false;
        digitalWrite(mag_pin, LOW);
      } else {
        Serial.println("Magnet Enabled");
        magIsEnabled = true;
        digitalWrite(mag_pin, HIGH);
      }

    } else if (block1 == "rook") {
      Serial.println("Set elevator to height of rook");
      elevator.step_to_pos(53.0);
    } else if (block1 == "above") {
      Serial.println("Moving up");
      elevator.step_to_pos(115.0);
    } else if (block1 == "start") {

      while(true) {
      play_move(2,2, 2,7, 52.0);
      delay(1000);
      play_move(2,7, 7,7, 52.0);
      delay(1000);
      play_move(7,7, 7,2, 53.0);
      delay(1000);
      play_move(7,2, 2,2, 53.0);
      delay(1000);
      }
    }else {
      Serial.println("Moving to square: (" + block1 + ", " + block2 + ")");
      go_to_square(block1.toInt(), block2.toInt());
    }
  }
}


// Setup (Run at beginning)------------------------------------

void setup() {

  // Starting Serial monitor
  Serial.begin(9600);
  Serial.println("Starting...");

  // Define and setup each joint
  joint[0] = RotationJoint(4.0, 2, 3, 400);
  joint[1] = RotationJoint(2.0, 4, 5, 400);

  elevator = LinearJoint(8, 6, 7, 400);

  for (int i = 0; i< 2; i++) {
    pinMode(joint[i].getDirPin(), OUTPUT);
    pinMode(joint[i].getStepPin(), OUTPUT);
  }

  pinMode(elevator.getDirPin(), OUTPUT);
  pinMode(elevator.getStepPin(), OUTPUT);

  joint[0].setRpm(23);
  joint[1].setRpm(15);
  elevator.setRpm(200);

  // Zeroing position of joints
  joint[0].setCurrAngle(90);
  joint[1].setCurrAngle(0);
  elevator.setCurrPos(0);

  // Setup Magnet
  pinMode(mag_pin, OUTPUT);
  // digitalWrite(mag_pin, HIGH);
  magIsEnabled = false;
}


// Loop -------------------------------------------------------

void loop() {
  read_input();
}
