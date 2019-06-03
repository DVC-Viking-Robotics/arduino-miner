#ifndef NoDelayStepper_h
#define NoDelayStepper_h

class NoDelayStepper{
public:
    NoDelayStepper(unsigned char pins[], char stepType = 2, long stepsPerRev = 4096, short maxRPM = 60);
    float getAngle(); // calc angle
    void go(int); // set target steps
    void go(float); // set target steps via angle
    void tick(); // non-intrupt timmer method
private:
    short step_it(); // step the iterator +-1
    void write(); // write pin outputs accordingto steps counter
    // wrap variable it w/in specified max and min
    template<typename T>T clamp_it(T max, T min, T it); 
    float wrap_angle(short theta); // wrap angle to range [0,360]
    void set_delay(); // set ending time to hold between steps
    bool isCW(); // is ClockWise shorter than counter-ClockWise?
    const unsigned char NumbPins = 4;
    // delta time, Steps Per Revolution, steps counter, steps destination
    long _dt, SPR, steps, target;
    char _it, sType;// iterator, stepper type
    short RPM; // Rotations Per Minute
    unsigned char *pin; // array of pins to motor (digital outputs)
    float DPS;
};
#endif