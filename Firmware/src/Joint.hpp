#ifndef JOINT_HPP
#define JOINT_HPP

#include <Arduino.h>

class Joint {

    public:

    Joint(int dir_pin, int step_pin, int steps_per_rev, int rpm=120);
    
    void step_motor(int numSteps);
    virtual void update();

    int getDirPin();
    int getStepPin();
    int getStepsPerRev();

    float getRpm();

    void setRpm(float rpm);
    void setDir(int dir);
    void setNeededSteps(int steps);

    protected:

    int step_pin;
    int dir_pin;
    int direction;
    int steps_per_rev;

    int rpm;
    int time_per_step;  // In microseconds

    bool isHigh;  // Used in update() to tell if output is HIGH or LOW
    bool isStepping;

    int next_step_time;
    int needed_steps;
};

class RotationJoint : public Joint {

    public:

    RotationJoint(float gear_ratio, int dir_pin, int step_pin,
                  int steps_per_rev, int rpm=120);

    void step_motor_angle(float angle);

    void update();

    float getCurrAngle();
    float getTargetAngle();
    float getGearRatio();

    void setTargetAngle(float angle);

    private:

    float curr_angle;
    float target_angle;

    float step_angle;

    float gear_ratio;
};

class LinearJoint : public Joint {

    public:

    LinearJoint(int mmPerRev, int dir_pin, int step_pin, 
                int steps_per_rev, int rpm=120);

    void step_dist(float distance);

    float getCurrPos();
    float getTargetPos();
    float getMMPerStep();

    void setTargetPos(float pos);

    private:

    float curr_pos;
    float target_pos;

    float mmPerStep;
};

#endif