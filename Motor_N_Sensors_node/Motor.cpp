#include "Motor.h"
#include <arduino.h>


Motor::Motor(unsigned char pin1, unsigned char pin2, int dt){
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    rampTime = dt;
    isUp= false;
    // save pin #s and set pin mode
    _pin1 = pin1;
    _pin2 = pin2;
    signal1 = 0;
    signal2 = 0;
}

short Motor::fakeCellerate(long now){
    long timeI = now - initSmooth;
    float delta_speed = sin( timeI / rampTime * PI / 2 + (-1 * isUp * PI / 2) ) + isUp;
    write((short)(delta_speed * (targetSpeed - baseSpeed) + baseSpeed));
}

void Motor::go(short s){
    targetSpeed = max(-100, min(100, round(s * 100)));
    initSmooth = millis();
    baseSpeed = getSpeed(); 
    float deltaT = abs((targetSpeed - baseSpeed) / 100.0);
    endSmooth = initSmooth + deltaT * rampTime;
    isUp = targetSpeed > baseSpeed;
    fakeCellerate(initSmooth + 1);
}

void Motor::tick(){
    short currTime = millis();
    if (abs(getSpeed() - targetSpeed) >= 2  && currTime < endSmooth){
        fakeCellerate(currTime);
    }
}

// Function definitions for BiMotor class
BiMotor::BiMotor(unsigned char pin1, unsigned char pin2, int dt): Motor(pin1, pin2, dt){}

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
    analogWrite(_pin2, signal2);
    // Serial.print("motor pwm: ");Serial.println(s);
}

short BiMotor::getSpeed(){
    return (short)signal1 - signal2;
}

// Function definitions for PhasedMotor class
PhasedMotor::PhasedMotor(unsigned char pin1, unsigned char pin2, int dt): Motor(pin1, pin2, dt){
}
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
    analogWrite(_pin2, signal2);
}
short PhasedMotor::getSpeed(){ 
    return (short)signal2 - (!signal1 * 255);
}
