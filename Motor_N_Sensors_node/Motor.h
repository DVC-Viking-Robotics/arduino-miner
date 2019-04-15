#ifndef Motor_h
#define Motor_h

class Motor{
public:
    void setSpeed(short){};
};

class BiMotor: public Motor{
    // class for instantiating DC drive motor(s)
public:
    BiMotor(unsigned char, unsigned char);// c'tor
    void setSpeed(int);// speed can be in range [-255, 255]
private:
    void write();// write PWM  to motor pins
    // variables for PWM duty cyles and pin #s respectively
    short p1, p2;
    unsigned char pOut1, pOut2;
};

class PhasedMotor: public Motor{
    // class for instantiating DC drive motor(s)
public:
    PhasedMotor(unsigned char, unsigned char);// c'tor
    void setSpeed(int);// speed can be in range [-255, 255]
private:
    void write();// write PWM  to motor pins
    // variables for PWM duty cyles and pin #s respectively
    bool dir;
    short pwm;
    unsigned char dirPin, pwmPin;
};

#endif
