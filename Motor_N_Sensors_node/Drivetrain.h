#ifndef Drivetrain_h
#define Drivetrain_h
#include "Motor.h"

class Drivetrain{
public:
    Drivetrain(unsigned char m1pin1, unsigned char m1pin2, unsigned char m2pin1, unsigned char m2pin2, bool isPhased){
        //create object for each motor (pinMode performed on instantiation)
        if (isPhased){
            M1 = new PhasedMotor(m1pin1, m1pin2);
            M2 = new PhasedMotor(m2pin1, m2pin2);
        }
        else {
            M1 = new BiMotor(m1pin1, m1pin2);
            M2 = new BiMotor(m2pin1, m2pin2);
        }
    }
    virtual void go(short, short){};
    virtual short getSpeed(unsigned char){};
protected:
    short clampPWM(short input){ // return a proper range of [-255, 255]
        return (input < -255 ? -255: (input > 255 ? 255 : input));
    }
    Motor* M1;
  	Motor* M2;
};

class BiPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    BiPed(unsigned char, unsigned char, unsigned char, unsigned char, bool);// c'tor
    void go(short, short);// set motors' speeds allowable range is [-255,255]
    short getSpeed(unsigned char); //return PWM duty cycle in range [-255, 255]
private:
    // motor objects and current speeds for left and right motors
    short right, left;
};

class QuadPed: public Drivetrain{
    // class for controlling the DC drive motors in tandem
public:
    QuadPed(unsigned char, unsigned char, unsigned char, unsigned char, bool);// c'tor
    void go(short, short);// set motors' speeds allowable range is [-255,255]
    short getSpeed(unsigned char m = 0); //return PWM duty cycle in range [-255, 255]
private:
    // motor objects and current speeds for left and right motors
    short FB, LR;
};
#endif