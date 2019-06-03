#include "Motor.h"
#include <arduino.h>

// Function definitions for BiMotor class
BiMotor::BiMotor(byte pin1, byte pin2): Motor(pin1, pin2){
    // save pin #s and set pin mode
    pOut1 = pin1;
    pOut2 = pin2;
}
void BiMotor::go(int s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s < 0){ // if going backward
        analogWrite(pOut1, 0);
        analogWrite(pOut2, -1 * s);
    }
    else if (s >= 0){// if going forward or stopped
        analogWrite(pOut1, s);
        analogWrite(pOut2, 0);
    }
}
// Function definitions for PhasedMotor class
PhasedMotor::PhasedMotor(byte pin1, byte pin2): Motor(pin1, pin2){
    // save pin #s and set pin mode
    dirPin = pin1;
    pwmPin = pin2;
}
void PhasedMotor::go(int s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s <= 0){     // if going backward or stopped
        digitalWrite(dirPin, false);
        analogWrite(pwmPin, -1 * s);
    }
    else if (s > 0){// if going forward
        digitalWrite(dirPin, true);
        analogWrite(pwmPin, s);
    }
}
