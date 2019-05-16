/*  ********************************************* 
 *  SparkFun_MAG3110_Calibrated
 *  Triple Axis Magnetometer Breakout - MAG3110 
 *  
 *  Import Sparkfun MAG3110 library to use. 
 *  
 * Pre-calibrated arduino magnetometer heading output on serial 
 * Preset for Diablo Valley College campus, PH.
 *  
 *  
 *  
 *  
 *  Hardware Specifications:
 *  SparkFun MAG3110
 *  Bi-directional Logic Level Converter
 *  Arduino Micro
 *  
 *  *********************************************/

#include <SparkFun_MAG3110.h>

MAG3110 mag = MAG3110(); //Instantiate MAG3110

void setup() {
  Serial.begin(9600);

  mag.initialize(); //Initialize the MAG3110 with all previously calibration values (these were read during previous setup calibrations) 
  mag.enterCalMode();
  float x_min = 10;
  float x_max = 380;
  float y_min = -1020;
  float y_max = -600;
  

  mag.x_scale = 1.0/(x_max - x_min);
  mag.y_scale = 1.0/(y_max - y_min);

  
  mag.setOffset(MAG3110_X_AXIS, 184);
  mag.setOffset(MAG3110_Y_AXIS, -820);

  mag.rawData(false);
  mag.calibrated = true;
  
}

void loop() {

  int x, y, z;

  
  
  mag.readMag(&x, &y, &z);

  float heading = mag.readHeading()*-1;
  
  if (heading < 0){
    heading += 360;
  }
  
 
  Serial.println(heading);


    
  delay(10);
}

