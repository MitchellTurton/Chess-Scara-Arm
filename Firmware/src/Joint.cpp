#include <Joint.hpp>

// Parent Joint Class ------------------------------------------------
Joint::Joint() {
}

Joint::Joint(int dir_pin, int step_pin, int steps_per_rev, int rpm=240) {
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->direction = 0;
    this->steps_per_rev = steps_per_rev;
    this->rpm = rpm;
    this->time_per_step = (60000000 / (rpm * steps_per_rev));

    this->isHigh = true;
    this->isStepping = false;
    this->next_step_time = 0;
    this->needed_steps = 0;
}

void Joint::step_motor(int numSteps) {
    // Serial.println("HI");

    digitalWrite(this->dir_pin, (numSteps < 0) ? LOW : HIGH);

    for (int i = 0; i < abs(numSteps); i++) {

        if (digitalRead(12) == HIGH) {
            digitalWrite(this->step_pin, HIGH);
            delayMicroseconds(this->time_per_step);
            digitalWrite(this->step_pin, LOW);
            delayMicroseconds(this->time_per_step);
        }
    }
}

void Joint::update() {
    if (micros() >= this->next_step_time && this->isStepping) {

        digitalWrite(this->step_pin, (this->isHigh) ? HIGH : LOW);

        this->isHigh = !this->isHigh;

        needed_steps--;

        if (needed_steps == 0) {
            this->isStepping = false;
            this->next_step_time = 0;
        } else {
            this->next_step_time = micros() + this->time_per_step;
        }
    }
}

int Joint::getDirPin() { return this->dir_pin; }

int Joint::getStepPin() { return this->step_pin; }

int Joint::getStepsPerRev() { return this->steps_per_rev; }

float Joint::getRpm() { return this->rpm; }

void Joint::setRpm(float rpm) { 
    this->rpm = rpm;
    this->time_per_step = (60000000 / (rpm * steps_per_rev)); 
}

void Joint::setDir(int dir) {
    this->direction = dir;

    if (dir != 0) {
        digitalWrite(this->dir_pin, (dir < 0) ? LOW : HIGH);
    }
}

void Joint::setNeededSteps(int steps) {
    this->needed_steps = abs(steps);
    this->setDir(steps / abs(steps));
    this->isStepping = true;
    this->next_step_time = micros();
}

// RotationJoint ----------------------------------------------------

RotationJoint::RotationJoint() { 
}

RotationJoint::RotationJoint(float gear_ratio, int dir_pin, int step_pin, int steps_per_rev, int rpm=240) 
    : Joint(dir_pin, step_pin, steps_per_rev, rpm)
{
    this->curr_angle = 0.f;
    this->target_angle = 0;
    this->gear_ratio = gear_ratio;

    this->step_angle = 360 / (this->steps_per_rev * this->gear_ratio);

    Serial.println(String(dir_pin) + " Step Angle: " + String(this->step_angle) + " Steps per rev: " + String(this->steps_per_rev) + " Gear Ratio: " + String(this->gear_ratio));
}

void RotationJoint::step_motor_angle(float angle) {
    float num_steps = angle / this->step_angle;

    // Serial.println(num_steps);

    this->step_motor(num_steps);
}

void RotationJoint::step_motor_toangle(float target_angle) {

    Serial.println("J" + String(this->dir_pin/2) + " stepping to: " + String(target_angle));
    float delta_angle = target_angle - this->curr_angle;

    int num_steps = round(delta_angle / this->step_angle);

    this->step_motor(num_steps);

    this->curr_angle += num_steps * this->step_angle;
}

void RotationJoint::step_motor_toangle() {
    float delta_angle = this->target_angle - this->curr_angle;

    this->step_motor_angle(delta_angle);

    this->curr_angle = this->target_angle;
}

void RotationJoint::update() {
    Joint::update();

    this->curr_angle += this->step_angle;
}

float RotationJoint::getCurrAngle() { return this->curr_angle; }

float RotationJoint::getTargetAngle() { return this->target_angle; }

float RotationJoint::getGearRatio() { return this->gear_ratio; }

void RotationJoint::setCurrAngle(float angle) { this->curr_angle = angle; }

void RotationJoint::setTargetAngle(float angle) { 
    this->target_angle = angle;

    // float delta_angle = this->target_angle - this->curr_angle;

    // float step_angle = (360 / this->steps_per_rev) * this->gear_ratio;
    // float num_steps = delta_angle / step_angle;

    // this->setNeededSteps(num_steps);
}


// Linear Joint -----------------------------------------------------------

LinearJoint::LinearJoint() {

}

LinearJoint::LinearJoint(int mmPerRev, int dir_pin, int step_pin, int steps_per_rev, int rpm=240) 
    : Joint(dir_pin, step_pin, steps_per_rev, rpm)
{
    this->mmPerStep = ((float) mmPerRev) / ((float) steps_per_rev);
    this->curr_pos = 0;
    this->target_pos = 0;

    Serial.println("Elevator mmPerStep: " + String(this->mmPerStep));
}

void LinearJoint::step_dist(float distance) {
    int num_steps = round(distance / this->mmPerStep);
    
    this->step_motor(num_steps);
}

void LinearJoint::step_to_pos(float target_pos) {
    float delta_dist = target_pos - this->curr_pos;

    int num_steps = round(delta_dist / this->mmPerStep);

    Serial.println("Num Steps: " + String(num_steps));

    this->step_motor(num_steps);

    this->curr_pos += num_steps * this->mmPerStep;
}

float LinearJoint::getCurrPos() { return this->curr_pos; }
float LinearJoint::getTargetPos() { return this->target_pos; }
float LinearJoint::getMMPerStep() { return this->mmPerStep; }

void LinearJoint::setCurrPos(float pos) { this->curr_pos = pos; }
void LinearJoint::setTargetPos(float pos) { this->target_pos = pos; }
