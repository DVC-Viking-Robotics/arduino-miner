#ifndef NoDelayStepper_h
#define NoDelayStepper_h

class NoDelayStepper{
public:
    NoDelayStepper(unsigned char pins[], char stepType = 2, long stepsPerRev = 4096, double degreesPerStep = 0.087890625, short maxRPM = 60);
    long getSteps(); // return _steps counter
    double getAngle(); // calc angle
    double getValue(); // return percentual position
    void go(double); // set target using percentage [-100,100]
    void goSteps(int); // set target steps per revolution
    void goAngle(double); // set target steps via angle
    bool isChanging();
    void tick(); // non-intrupt timmer method
private:
    void resetZeroAngle();
    void releasePins();
    short step_it(bool); // step the iterator +-1
    void write(); // write pin outputs according to steps counter
    // wrap variable it w/in specified max and min
    template<typename T>T wrap_it(T max, T min, T it); 
    double wrap_angle(short theta); // wrap angle to range [0,360]
    void set_delay(); // set ending time to hold between steps
    bool isCW(); // is ClockWise shorter than counter-ClockWise?
    const unsigned char NumbPins = 4;
    // delta time, Steps Per Revolution, steps counter, steps destination
    unsigned long _dt;
    long SPR, _steps, target;
    char _it, sType;// iterator, stepper type
    short RPM; // Rotations Per Minute
    unsigned char *pin; // array of pins to motor (digital outputs)
    double DPS;
};
#endif