#include "Motor.h"
#include <arduino.h>

// Function definitions for BiMotor class
BiMotor::BiMotor(unsigned char pin1, unsigned char pin2): Motor(pin1, pin2){
    // save pin #s and set pin mode
    pOut1 = pin1;
    pOut2 = pin2;
    // initialize PWM duty cycles
    p1 = 0;
    p2 = 0;
}
void BiMotor::setSpeed(int s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s < 0){ // if going backward
        p1 = 0;
        p2 = -1 * s;
    }
    else if (s >= 0){// if going forward or stopped
        p1 = s;
        p2 = 0;
    }
    write();
}
void BiMotor::write(){
    // generate motor's signals
    analogWrite(pOut1, p1);
    analogWrite(pOut2, p2);
}

// Function definitions for PhasedMotor class
PhasedMotor::PhasedMotor(unsigned char pin1, unsigned char pin2): Motor(pin1, pin2){
    // save pin #s and set pin mode
    dirPin = pin1;
    pwmPin = pin2;
    // initialize PWM duty cycles
    dir = false;
    pwm = 0;
}
void PhasedMotor::setSpeed(int s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s <= 0){     // if going backward or stopped
        dir = false;
        pwm = -1 * s;
    }
    else if (s > 0){// if going forward
        dir = true;
        pwm = s;
    }
    write();
}
void PhasedMotor::write(){
    // generate motor's signals
    digitalWrite(dirPin, dir);
    analogWrite(pwmPin, pwm);
}
