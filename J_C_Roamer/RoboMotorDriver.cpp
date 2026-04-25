#include "RoboMotorDriver.h"
#include <Arduino.h>

ROBO_Motor::ROBO_Motor(uint8_t IN1, uint8_t IN2) {
    _IN1 = IN1;
    _IN2 = IN2;

    pinMode(_IN1, OUTPUT);
    pinMode(_IN2, OUTPUT);
    
}

void ROBO_Motor::setSpeed(uint8_t pwm) {
    _pwm = constrain(pwm, 0, 255);
}


void ROBO_Motor::forward() {
        analogWrite(_IN1, _pwm);
        digitalWrite(_IN2, LOW);

    }

void ROBO_Motor::backward() {
        digitalWrite(_IN1, LOW);
        analogWrite(_IN2, _pwm);

}


void ROBO_Motor::release() {
        analogWrite(_IN1, 0);
        analogWrite(_IN2, 0);
;

}
