#ifndef Motor_h
#define Motor_h
#include <arduino.h>

class Motor{
    bool isUp;
    int rampTime;
    short targetSpeed, baseSpeed;
    long initSmooth, endSmooth;
    short fakeCellerate(long now);
public:
    Motor(unsigned char pin1, unsigned char pin2, int dt = 1000);
    virtual short getSpeed(){};
    virtual void write(short){};
    void go(short);
    void tick();
protected:
    unsigned char _pin1, _pin2, signal1, signal2;
};

class BiMotor: public Motor{
    
public:
    BiMotor(unsigned char, unsigned char, int dt = 1000);
    void write(short);
    short getSpeed();
};

class PhasedMotor: public Motor{
public:
    PhasedMotor(unsigned char, unsigned char, int dt = 1000);
    void write(short);
    short getSpeed();
};

#endif
