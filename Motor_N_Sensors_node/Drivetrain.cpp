#include "Drivetrain.h"

BiPed::BiPed(unsigned char m1pin1, unsigned char m1pin2, unsigned char m2pin1, unsigned char m2pin2, bool isPhased) :Drivetrain(m1pin1, m1pin2, m2pin1, m2pin2, isPhased){
    left = 0;
    right = 0;
    M1->setSpeed(right);
    M2->setSpeed(left);
}

void BiPed::go(short x, short y){
    // make sure x and y are in proper range
    x = clampPWM(x);
    y = clampPWM(y);
    left = y,  right = y;
    // if not going forward or backward (turn on a dime)
    if (y == 0){
        right = -1 * x;
        left = x;
    }
    else{// if turning left or right adjust motors' speed accordingly
        if (x > 0)
            right *= ((255 - x) / 255.0);
        else if (x < 0)
            left *= ((-255 - x) / -255.0);
    }
    // write speed output for each motor
    M1->setSpeed(right);
    M2->setSpeed(left);
}

short BiPed::getSpeed(unsigned char m){ // let m be the motor #
    if (m) return right;
    else return left;
}

QuadPed::QuadPed(unsigned char m1pin1, unsigned char m1pin2, unsigned char m2pin1, unsigned char m2pin2, bool isPhased) :Drivetrain(m1pin1, m1pin2, m2pin1, m2pin2, isPhased){
    FB = 0;
    LR = 0;
    M1->setSpeed(LR);
    M2->setSpeed(FB);
}

void QuadPed::go(short x, short y){
    // make sure x and y are in proper range
    x = clampPWM(x);
    y = clampPWM(y);
    FB = y,  LR = x;
    // write speed output for each motor
    M1->setSpeed(LR);
    M2->setSpeed(FB);
}

short QuadPed::getSpeed(unsigned char m){ // let m be the motor #
    if (m) return LR;
    else return FB;
}
