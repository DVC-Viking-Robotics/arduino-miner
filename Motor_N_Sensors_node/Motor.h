#ifndef Motor_h
#define Motor_h
#include <arduino.h>

class Motor{
public:
    Motor(byte pin1, byte pin2){
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
    }
    void go(short){};
    void tick(){};
};

class BiMotor: public Motor{
    // class for instantiating DC drive motor(s)
    // variables for pin #s
    byte pOut1, pOut2;
public:
    BiMotor(byte, byte);// c'tor
    void go(int);// speed can be in range [-255, 255]
};

class PhasedMotor: public Motor{
    // class for instantiating DC drive motor(s)
    // variables for pin #s
    byte dirPin, pwmPin;
public:
    PhasedMotor(byte, byte);// c'tor
    void go(int);// speed can be in range [-255, 255]
};

#endif
