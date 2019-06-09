#include "Motor.h"
#include <arduino.h>


Solonoid::Solonoid(unsigned char pins[], int dt){
    // save pin #s and set pin mode
    pinMode(pins[0], OUTPUT);
    _pin1 = pins[0];
    if (sizeof(pins) > 1){
        pinMode(pins[1], OUTPUT);
        _pin2 = pins[1];
    }
    else _pin2 = NULL;
    signal1 = 0;
    signal2 = 0;
    rampTime = dt;
    isUp= false;
}

short Solonoid::fakeCellerate(long now){
    long timeI = now - initSmooth;
    // float delta_speed = sin( timeI / (endSmooth - initSmooth) * PI / 2 + ((isUp ? -1 : 1) * PI / 2) ) + isUp;
    float delta_speed = sin( (timeI / (endSmooth - initSmooth) + (isUp ? -1 : 1)) * PI / 2 ) + isUp;
    write((short)(delta_speed * (targetSpeed - baseSpeed) + baseSpeed));
}

void Solonoid::cellerate(short s){
    targetSpeed = max(-255, min(255, round(s * 255)));
    initSmooth = micros();
    baseSpeed = getSpeed(); 
    float deltaT = abs((targetSpeed - baseSpeed) / 255.0);
    endSmooth = initSmooth + deltaT * rampTime * 1000;
    isUp = targetSpeed > baseSpeed;
    fakeCellerate(initSmooth + 1);
}
short Solonoid::getSpeed(){ 
       return (short)signal1;
}

void Solonoid::write(short s){
    // input speed 's' must be previously constrained to range of [-255, 255]
    signal1 = s;
    analogWrite(_pin1, s);
}

void Solonoid::tick(){
    short currTime = micros();
    if (abs(getSpeed() - targetSpeed) >= 2  && currTime < endSmooth){
        fakeCellerate(currTime);
    }
}

// Function definitions for BiMotor class
BiMotor::BiMotor(unsigned char pins[], int dt): Solonoid(pins, dt){}

void BiMotor::write(short s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s < 0){ // if going backward
        signal1 = 0;
        signal2 = -1 * s;
    }
    else if (s >= 0){// if going forward or stopped
        signal1 = s;
        signal2 = 0;
    }
    analogWrite(_pin1, signal1);
    if (_pin2 != NULL)
        analogWrite(_pin2, signal2);
    // Serial.print("motor pwm: ");Serial.println(s);
}

short BiMotor::getSpeed(){
    if (_pin2 != NULL){
        return (short)signal1 - signal2;
    }
    else{
       return (short)signal1;
    }
}

// Function definitions for PhasedMotor class
PhasedMotor::PhasedMotor(unsigned char pins[], int dt): Solonoid(pins, dt){}

void PhasedMotor::write(short s){
    //input speed 's' must be previously constrained to range of [-255, 255]
    if (s <= 0){     // if going backward or stopped
        signal1 = false;
        signal2 = -1 * s;
    }
    else if (s > 0){// if going forward
        signal1 = true;
        signal2 = s;
    }
    digitalWrite(_pin1, signal1);
    if (_pin2 != NULL)
        analogWrite(_pin2, signal2);
}
short PhasedMotor::getSpeed(){ 
    if (_pin2 != NULL){
        return (short)signal2 - (!signal1 * 255);
    }
    else{
       return (short)signal1 * 255;
    }
}
