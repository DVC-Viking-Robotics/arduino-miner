#ifndef Motor_h
#define Motor_h
#include <arduino.h>

class Solonoid{
    bool isUp;
    int rampTime;
    short targetSpeed, baseSpeed;
    long initSmooth, endSmooth;
    short fakeCellerate(long now);
public:
    Solonoid(unsigned char pins[], int dt = 1000);
    short getSpeed();
    void write(short);
    void go(short);
    void tick();
protected:
    unsigned char _pin1, _pin2, signal1, signal2;
};

class BiMotor: public Solonoid{
    
public:
    BiMotor(unsigned char pins[], int dt = 1000);
    void write(short);
    short getSpeed();
};

class PhasedMotor: public Solonoid{
public:
    PhasedMotor(unsigned char pins[], int dt = 1000);
    void write(short);
    short getSpeed();
};

#endif
