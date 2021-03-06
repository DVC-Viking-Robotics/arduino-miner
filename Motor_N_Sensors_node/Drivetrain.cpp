#include "Drivetrain.h"
void Drivetrain::tick(){
    M1->tick(); 
    M2->tick(); 
}

BiPed::BiPed(unsigned char m1[], unsigned char m2[], bool isPhased) :Drivetrain(m1, m2, isPhased){
    left = 0;
    right = 0;
    // M1->go(right);
    // M2->go(left);
}

void BiPed::go(short x, short y){
    // make sure x and y are in proper range
    // x = clampPWM(x);
    // y = clampPWM(y);
    left = y,  right = y;
    // if not going forward or backward (turn on a dime)
    if (y == 0){
        right = -1 * x;
        left = x;
    }
    else{// if turning left or right adjust motors' speed accordingly
        if (x > 0)
            right *= (255 - x) / 255.0;
        else if (x < 0)
            left *= ((-255 - x) * -1) / 255.0;
    }
    // write speed output for each motor
    M1->go(right);
    M2->go(left);
}

QuadPed::QuadPed(unsigned char m1[], unsigned char m2[], bool isPhased) :Drivetrain(m1, m2, isPhased){
    FB = 0;
    LR = 0;
    // M1->go(LR);
    // M2->go(FB);
}

void QuadPed::go(short x, short y){
    // make sure x and y are in proper range
    // x = clampPWM(x);
    // y = clampPWM(y);
    FB = y,  LR = x;
    // write speed output for each motor
    M1->go(LR, 0);
    M2->go(FB);
}
