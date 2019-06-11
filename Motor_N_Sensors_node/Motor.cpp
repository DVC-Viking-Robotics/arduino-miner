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
    signal1 = 0, signal2 = 0, endSpeed = 0, initSpeed = 0;
    initSmooth = 0, endSmooth = 0, rampTime = dt;
}

short Solonoid::fakeCellerate(unsigned long now){
    unsigned long timeI = now - initSmooth;
    // float delta_speed = sin( timeI / (endSmooth - initSmooth) * PI / 2 + ((endSpeed > initSpeed ? -1 : 1) * PI / 2) ) + (endSpeed > initSpeed);
    float delta_speed = 1 - cos(timeI / (float)(endSmooth - initSmooth) * PI / 2 );
    // Serial.print("_ds=sin((");Serial.print(timeI);
    // Serial.print("/");Serial.print(endSmooth-initSmooth);
    // Serial.print("+");Serial.print(endSpeed > initSpeed ? -1 : 1);
    // Serial.print(")*PI/2)+");Serial.println(endSpeed > initSpeed);
    // Serial.print("pwm = ");Serial.print(delta_speed);
    // Serial.print(" * (");Serial.print(endSpeed);
    // Serial.print(" - ");Serial.print(initSpeed);
    // Serial.print(") + ");Serial.print(initSpeed);
    // Serial.print(" = ");Serial.println(delta_speed * (endSpeed - initSpeed) + initSpeed);
    write((short)(delta_speed * (endSpeed - initSpeed) + initSpeed));
}

void Solonoid::go(short s){
    endSpeed = max(-255, min(255, round(s)));
    initSmooth = millis();
    initSpeed = getSpeed(); 
    float deltaT = abs((float)(endSpeed - initSpeed) / 255.0);
    endSmooth = initSmooth + deltaT * rampTime;
    // endSmooth = initSmooth + rampTime;
    // Serial.print("init_T=");Serial.print(initSmooth);
    // Serial.print(" target_T=");Serial.println(endSmooth);
    // Serial.print("init_s=");Serial.print(initSpeed);
    // Serial.print(" target_s=");Serial.println(endSpeed);
    // Serial.print(" going_Fw=");Serial.println(endSpeed > initSpeed);
    fakeCellerate(initSmooth + 1);
}

short Solonoid::getSpeed(){ 
       return (short)signal1;
}
 
void Solonoid::write(short s){
    // input speed 's' must be previously constrained to range of [-255, 255]
    signal1 = s;
    analogWrite(_pin1, s);
    // Serial.print(s);
    // Serial.println(" = Speed wrote with Solonoid Parent class");
}

void Solonoid::tick(){
    if (getSpeed() != endSpeed){
        unsigned long currTime = millis();
        if (currTime < endSmooth){
            fakeCellerate(currTime);
        }
        else {
            write(endSpeed);
        }
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
    // Serial.print("motor pwm: ");Serial.println(s);
}
short PhasedMotor::getSpeed(){ 
    if (_pin2 != NULL){
        return (short)signal2 - (!signal1 * 255);
    }
    else{
       return (short)signal1 * 255;
    }
}
