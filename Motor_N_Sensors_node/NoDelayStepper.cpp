#include "NoDelayStepper.h"
#include <arduino.h>

NoDelayStepper::NoDelayStepper(unsigned char pins[], char stepType, long stepsPerRev, float degreesPerStep, short maxRPM){
    _it = 0;
    steps = 0;
    _dt = 0;
    SPR = stepsPerRev;
    DPS = degreesPerStep;
    RPM = maxRPM;
    target = 0;
    if (stepType == 0 || stepType == 1 || stepType == 2) 
        sType = stepType;
    else sType = -1;
    pin = pins;
    for (unsigned char i = 0; i < NumbPins; ++i){
        // Serial.print("setting up pin ");Serial.println(pin[i]);
        pinMode(pin[i], OUTPUT);
        digitalWrite(pin[i], false);
    }
}

bool NoDelayStepper::isCW(){
    if (abs((target % SPR) - (steps % SPR)) < SPR - abs((target % SPR) - (steps % SPR))){
        return target <= steps;
    }
    else return target >= steps;
}

short NoDelayStepper::step_it(){
    if (isCW()){
        _it--;
        steps = steps - 1 < 0 ? SPR - 1 : steps - 1;
    }
    else{
        _it++;
        steps = steps + 1 >= SPR ? 0 : steps + 1;
    } 
}

void NoDelayStepper::tick(){
    if (target != steps){
        unsigned long delta = millis();
        if (delta >= _dt){
            // Serial.print("delta time = ");Serial.println(delta);
            step_it();
            write();
            if (target != steps){
                set_delay();
                // Serial.print("stepper steps = ");
                // Serial.println(steps);
            }
    /*         else {
                Serial.print("stepper finished @ ");
                Serial.print(getAngle());
                Serial.println(" degrees");
            }*/
        }
    }
}

template<typename T>
T NoDelayStepper::clamp_it(T max, T min = 0, T it = NULL){
    if (it == NULL) it = _it;
    while (it >= max) it -= max;
    while (it < min) it += max;
    return it;
}

void NoDelayStepper::write(){
    short max_it = 0;
    short next = 0;
    short base = 0;
    switch (sType){
    case 0:
        /*wave NoDelayStepper type*/
        max_it = 4;
        _it = clamp_it<short>(max_it);
        for (unsigned char i = 0; i < NumbPins; i++){
            if (i == _it)
                digitalWrite(pin[i], true);
            else
                digitalWrite(pin[i], false);
        }
        break;
    case 1:
        /*full NoDelayStepper type*/
        max_it = 4;
        _it = clamp_it<short>(max_it);
        next = _it + 1 >= max_it ? 0 : _it + 1;
        for (unsigned char i = 0; i < NumbPins; i++){
            if (i == _it || i == next)
                digitalWrite(pin[i], true);
            else
                digitalWrite(pin[i], false);
        }
        break;
    case 2:
        /*half NoDelayStepper type*/
        max_it = 8;
        _it = clamp_it<short>(max_it);
        base = _it / 2;
        next = base + 1 >= NumbPins ? 0 : base + 1;
        for (unsigned char i = 0; i < NumbPins; i++){
            if (i == base || (i == next && _it % 2 == 1))
                digitalWrite(pin[i], true);
            else
                digitalWrite(pin[i], false);
        }
        break;
    default:
        break;
    }
}

void NoDelayStepper::set_delay(){
    _dt = millis() + (long)((long)(RPM) * SPR / 60000.0);
    /* Serial.print("delay = ");
    Serial.print((RPM * SPR / 60000.0));
    Serial.println(" milliseconds."); */
}

float NoDelayStepper::getAngle(){
    return steps * DPS;
}

void NoDelayStepper::go(int dest){
    target = clamp_it<long>(SPR, 0, dest);
    // Serial.print("motor target: ");Serial.println(target);
    if (target != steps){
        write();
        set_delay();
    }
}

void NoDelayStepper::go(float dest){
    target = (int)(clamp_it<float>(360.0, 0.0, dest) / 360.0 * SPR);
    if (target != steps){
        write();
        set_delay();
    }
}