#ifndef ROBO_MOTOR_DRIVER_H
#define ROBO_MOTOR_DRIVER_H

#include <Arduino.h>


class ROBO_Motor {
    public:
        ROBO_Motor(uint8_t IN1, uint8_t IN2);

        void setSpeed(uint8_t pwm);
        void forward();
        void backward();
        void release();
       


    private:
        byte _IN1;
        byte _IN2;
        byte _pwm;
};

#endif