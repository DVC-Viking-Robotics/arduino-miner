#ifndef Drivetrain_h
#define Drivetrain_h
#include "Motor.h"
#include "NoDelayStepper.h"
#include <arduino.h>

class Drivetrain{
public:
    Drivetrain(unsigned char m1[], unsigned char m2[], unsigned char m3[], bool isPhased){
        //create object for each motor (pinMode performed on instantiation)
        if (isPhased){
            M1 = new PhasedMotor(m1[0], m1[1]);
            M2 = new PhasedMotor(m2[0], m2[1]);
        }
        else {
            M1 = new BiMotor(m1[0], m1[1]);
            M2 = new BiMotor(m2[0], m2[1]);
        }
        M3 = new NoDelayStepper(m3);
    }
    virtual void go(short, short, float){};// dummy prototype
    void tick(){ M1->tick(); M2->tick(); M3->tick(); };
protected:
    /* short clampPWM(short input){ // return a proper range of [-255, 255]
        return (input < -255 ? -255 : (input > 255 ? 255 : input));
    } */
    Motor* M1;
    Motor* M2;
    NoDelayStepper* M3;
};

class BiPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    BiPed(unsigned char m1[], unsigned char m2[], unsigned char m3[], bool);// c'tor
    void go(short, short, float);// set motors' speeds allowable range is [-255,255]
private:
    // motor objects and current speeds for left and right motors
    short right, left;
};

class QuadPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    QuadPed(unsigned char m1[], unsigned char m2[], unsigned char m3[], bool);// c'tor
    void go(short, short, float);// set motors' speeds allowable range is [-255,255]
private:
    // motor objects and current speeds for left and right motors
    short FB, LR;
};
#endif