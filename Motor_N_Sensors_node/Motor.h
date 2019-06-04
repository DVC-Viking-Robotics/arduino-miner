#ifndef Motor_h
#define Motor_h
#include <arduino.h>

class Motor{
public:
    Motor(unsigned char pin1, unsigned char pin2){
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
    }
    virtual void go(short){};
};

class BiMotor: public Motor{
    // class for instantiating DC drive motor(s)
    // variables for pin #s
    unsigned char pOut1, pOut2;
public:
    BiMotor(unsigned char, unsigned char);// c'tor
    void go(short);// speed can be in range [-255, 255]
};

class PhasedMotor: public Motor{
    // class for instantiating DC drive motor(s)
    // variables for pin #s
    unsigned char dirPin, pwmPin;
public:
    PhasedMotor(unsigned char, unsigned char);// c'tor
    void go(short);// speed can be in range [-255, 255]
};

#endif
