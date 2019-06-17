#include "NoDelayStepper.h"
#include <arduino.h>

NoDelayStepper::NoDelayStepper(unsigned char pins[], char stepType, long stepsPerRev, double degreesPerStep, short maxRPM){
    _it = 0; // iterator
    _dt = 0; // time between _steps
    SPR = stepsPerRev;
    DPS = degreesPerStep;
    RPM = maxRPM; // this may get depricated
    target = 0; // last instructed position(in _steps)
    resetZeroAngle(); // init _steps = 0
    if (stepType == 0 || stepType == 1 || stepType == 2) // is type known? 
        sType = stepType; // see step_it() def
    else sType = -1; // should do nothing instead of throwing error
    pin = pins; // save array* to object
    // expected size must = 4 as NumbPins is hardcoded to 4
    for (unsigned char i = 0; i < NumbPins; ++i){
        // Serial.print("setting up pin ");Serial.println(pin[i]);
        pinMode(pin[i], OUTPUT);
        digitalWrite(pin[i], false);
    }
}
void NoDelayStepper::resetZeroAngle(){
    _steps = 0;
}

bool NoDelayStepper::isCW(){
    long currPos = wrap_it<long>(SPR , 0, _steps % SPR);
    if (currPos > target)
        return currPos - target < SPR / 2;
    else return target - currPos > SPR / 2;
}

short NoDelayStepper::step_it(bool isCw){
    if (isCw){
        _it-=1;
        _steps-=1;
    }
    else{
        _it+=1;
        _steps+=1;
    } 
}

void NoDelayStepper::tick(){
    if (target != _steps){
        unsigned long delta = millis();
        if (delta >= _dt){
            // Serial.print("delta time = ");Serial.println(delta);
            step_it(isCW());
            write();
            // calc Delta t w/ _smooth()
    /*        if (target != _steps){
                set_delay(); // set end of wait time between _steps
                // Serial.print("stepper _steps = ");
                // Serial.println(_steps);
            }
             else {
                Serial.print("stepper finished @ ");
                Serial.print(getAngle());
                Serial.println(" degrees");
            }*/
        }
    }
}

template<typename T>
T NoDelayStepper::wrap_it(T max, T min = 0, T it = NULL){
    if (it == NULL) it = _it;
    while (it >= max) it -= max;
    while (it < min) it += max;
    return it;
}

bool NoDelayStepper::isChanging(){
    return target != NULL ? target != _steps : false;
}

void NoDelayStepper::write(){
    short max_it = 0;
    short next = 0;
    short base = 0;
    switch (sType){
    case 0:
        /*wave NoDelayStepper type*/
        max_it = 4;
        _it = wrap_it<short>(max_it);
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
        _it = wrap_it<short>(max_it);
        next = _it + 1 >= NumbPins ? 0 : _it + 1;
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
        _it = wrap_it<short>(max_it);
        base = _it / 2;
        next = base + 1 <= NumbPins ? base + 1 : 0;
        for (unsigned char i = 0; i < NumbPins; i++){
            if (i == base || (i == next && _it % 2 == 1))
                digitalWrite(pin[i], true);
            else
                digitalWrite(pin[i], false);
        }
        break;
    default:
    // should throw an error, but I'm lazy
        break;
    }
}

void NoDelayStepper::set_delay(){
    _dt = millis() + (unsigned long)(RPM * SPR / 60000.0);
    /* Serial.print("delay = ");        
    Serial.print((RPM * SPR / 60000.0));
    Serial.println(" milliseconds."); */
}

long NoDelayStepper::getSteps(){
    return _steps;
}

double NoDelayStepper::getAngle(){
    return _steps * DPS;
}
double NoDelayStepper::getValue(){
    return wrap_it<double>(SPR / 2, SPR / -2, _steps) / (SPR / 2) * 100;
}

void NoDelayStepper::releasePins(){
    for (unsigned char i = 0; i < NumbPins; i++)
        digitalWrite(pin[i], false);
}

void NoDelayStepper::go(double dest){ // % value
    if (dest == NULL){
        resetZeroAngle();
        releasePins();
    }
    else{
        dest = max(-100.0, min(100.0, dest));
        target = wrap_it<long>(SPR, 0, SPR / 200.0 * dest);
        // Serial.print("motor target: ");Serial.println(target);
        tick();
    }
}

void NoDelayStepper::goSteps(int dest){
    if (dest == NULL){
        resetZeroAngle();
        releasePins();
    }
    else{
        target = dest + _steps;
        // Serial.print("motor target: ");Serial.println(target);
        tick();
    }
}

void NoDelayStepper::goAngle(double dest){
    if (dest == NULL){
        resetZeroAngle();
        releasePins();
    }
    else{
        target = round(wrap_it<double>(360.0, 0.0, dest) / 360.0 * SPR);
        tick();
    }
}