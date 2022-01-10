#include <Joint.hpp>

// Parent Joint Class ------------------------------------------------
Joint::Joint(int dir_pin, int step_pin, int steps_per_rev, int rpm=120) {
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->steps_per_rev = steps_per_rev;
    this->rpm = rpm;
}

void Joint::step_motor(int numSteps) {
    float step_delay = (60 / (this->steps_per_rev * this->rpm)) * 1000000;

    digitalWrite(this->dir_pin, (numSteps < 0) ? LOW : HIGH);

    for (int i = 0; i < numSteps; i++) {
        digitalWrite(this->step_pin, HIGH);
        delayMicroseconds(step_delay);
        digitalWrite(this->step_pin, LOW);
        delayMicroseconds(step_delay);
    }
}

int Joint::getDirPin() { return this->dir_pin; }

int Joint::getStepPin() { return this->step_pin; }

int Joint::getStepsPerRev() { return this->steps_per_rev; }

float Joint::getRpm() { return this->rpm; }

void Joint::setRpm(float rpm) { this->rpm = rpm; }


// RotationJoint ----------------------------------------------------

RotationJoint::RotationJoint(float gear_ratio, int dir_pin, int step_pin, int steps_per_rev, int rpm=120) 
    : Joint(dir_pin, step_pin, steps_per_rev, rpm)
{
    this->curr_angle = 0;
    this->target_angle = 0;
}

void RotationJoint::step_angle(float angle) {
    float step_angle = 360 / this->steps_per_rev;
    float num_steps = angle / step_angle;

    this->step_motor(num_steps);
}

float RotationJoint::getCurrAngle() { return this->curr_angle; }

float RotationJoint::getTargetAngle() { return this->target_angle; }

float RotationJoint::getGearRatio() { return this->gear_ratio; }

void RotationJoint::setTargetAngle(float angle) { this->target_angle = angle; }


// Linear Joint -----------------------------------------------------------

LinearJoint::LinearJoint(int mmPerRev, int dir_pin, int step_pin, int steps_per_rev, int rpm=120) 
    : Joint(dir_pin, step_pin, steps_per_rev, rpm)
{
    this->mmPerStep = mmPerRev / this->steps_per_rev;
    this->curr_pos = 0;
    this->target_pos = 0;
}

void LinearJoint::step_dist(float distance) {
    int num_steps = round(distance / this->mmPerStep);
    
    this->step_motor(num_steps);
}

float LinearJoint::getCurrPos() { return this->curr_pos; }
float LinearJoint::getTargetPos() { return this->target_pos; }
float LinearJoint::getMMPerStep() { return this->mmPerStep; }

void LinearJoint::setTargetPos(float pos) { this->target_pos = pos; }


