#ifndef Drivetrain_h
#define Drivetrain_h
#include "Motor.h"
#include <arduino.h>

class Drivetrain{
public:
    Drivetrain(unsigned char m1[], unsigned char m2[], bool isPhased){
        //create object for each motor (pinMode performed on instantiation)
        if (isPhased){
            M1 = new PhasedMotor(m1);
            M2 = new PhasedMotor(m2);
        }
        else {
            M1 = new BiMotor(m1);
            M2 = new BiMotor(m2);
        }
    }
    virtual void go(short, short){};// dummy prototype
    void tick();
protected:
    /* short clampPWM(short input){ // return a proper range of [-255, 255]
        return (input < -255 ? -255 : (input > 255 ? 255 : input));
    } */
    Solonoid* M1;
    Solonoid* M2;
};

class BiPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    BiPed(unsigned char m1[], unsigned char m2[], bool);// c'tor
    void go(short, short);// set motors' speeds allowable range is [-255,255]
private:
    // motor objects and current speeds for left and right motors
    short right, left;
};

class QuadPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    QuadPed(unsigned char m1[], unsigned char m2[], bool);// c'tor
    void go(short, short);// set motors' speeds allowable range is [-255,255]
private:
    // motor objects and current speeds for left and right motors
    short FB, LR;
};
#endif