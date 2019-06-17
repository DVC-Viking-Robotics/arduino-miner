/* 
#include <SparkFun_MAG3110.h>
MAG3110 mag = MAG3110(); //Instantiate MAG3110
void calibrateMag(); // function prototype
 */
// define motor type: 1 to enable direction + PWM or 0 to enable 2 PWM
#define phased 0
// define drivetrain type: 0 to enable BiPed (R2D2-like), 1 to enable QuadPed (like a race car)
#define motorConfig 0
#include "Drivetrain.h"

// declare drivetrain
Drivetrain* d;
// define PWM control pins for drive motors
byte M1pins[] = {3, 5};
byte M2pins[] = {6, 9};
// define controls pins for stepper motor
byte sPins[] = {2, 4, 7, 8};
// declare & initialize stepper motor
// NoDelayStepper neck(sPins);

void setup(){
  Serial.begin(115200);// open a channel to pour data into & get commands
  // instantiate drivetrain object (motors' speeds default to 0)
  if (motorConfig)
      d = new QuadPed(M1pins, M2pins, sPins, phased);
  else d = new BiPed(M1pins, M2pins, sPins, phased);
  /* 
  mag.initialize(); //Initialize the MAG3110
  calibrateMag(); // Calibrate sensor axis
  */
}

void loop(){
  d->tick();

  /* int mag_x, mag_y, mag_z;
  mag.readMag(&mag_x, &mag_y, &mag_z);
  Serial.println(mag.readHeading());
  */
  while (Serial.available() > 2){ //read from buffer if more than 2 bytes
    // LF & CR (2 bytes) seem to linger in stream buffer for 1 extra loop() iteration
    // stream expected format  = "# # #" where # == [-100,100]
    // delimiter can be any non-digit character (example above uses ' ')
    // use x for left-right steering
    // use y for forward-backward drive
    short x = round(Serial.parseInt() * 2.55);
    short y = round(Serial.parseInt() * 2.55);
    double z = Serial.parseInt();
    d->go(x, y, z);
    
  }
}
/* 
void calibrateMag(){
  while(!mag.isCalibrated()){ //If we're not calibrated
    if(!mag.isCalibrating()){ //And we're not currently calibrating
      Serial.println("Entering calibration mode");
      mag.enterCalMode(); //This sets the output data rate to the highest possible and puts the mag sensor in active mode
    }
    else{
      //Must call every loop while calibrating to collect calibration data
      //This will automatically exit calibration
      //You can terminate calibration early by calling mag.exitCalMode();
      mag.calibrate(); 
    }
    delayMicroseconds(100);
  }
  Serial.println("Calibrated!");
}
 */