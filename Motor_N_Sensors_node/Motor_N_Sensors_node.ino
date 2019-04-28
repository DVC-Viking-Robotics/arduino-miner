// for parseing datatype String
#include <String.h>
// include ibrary for the mag3110 sensor 
#include <SparkFun_MAG3110.h>

// include library for ultrasonic distance sensors
#include <NewPing.h>

// define HC-SR04 pins (echo and trigger are connected to same pin) for NewPing library
#define sensor1     13
#define sensor2     12
#define sensor3     8
#define sensor4     7
// define max sensor distance for HC-SR04 (in centimeters)
#define max_distance 400

// declare size of/& array for HCSR04 using NewPing library
const unsigned char totalDistSensors = 4;
NewPing sonar[totalDistSensors] = {
    NewPing(sensor1, sensor1, max_distance),
    NewPing(sensor2, sensor2, max_distance),
    NewPing(sensor3, sensor3, max_distance),
    NewPing(sensor4, sensor4, max_distance)};

//Instantiate MAG3110
MAG3110 mag = MAG3110(); 
// declare variables for mag sensor
int x, y, z;
// bool to check if calibrated
bool firstRun = true;

void setup(){
	Serial.begin(115200);// open a channel to pour data into & get commands
  //Initialize the MAG3110
  mag.initialize();
}

void getIMUdata(bool raw = false){
  // get & print IMU sensor data
  Serial.print("OUT");
  if (raw){ 
    Serial.print(",");Serial.print(x);
    Serial.print(",");Serial.print(y);
    Serial.print(",");Serial.print(z);
  }
  else{
    Serial.print(",");
    Serial.println(mag.readHeading());
  }
}

void getDistanceData(){
  // get & print distance sensor data
  Serial.print("Dist");
  for (unsigned char i = 0; i < totalDistSensors; i++){
    Serial.print(",");
    Serial.print(sonar[i].ping_cm());
  }
  Serial.println();
  // ensure samples validity by waiting 60ms between readings
  delayMicroseconds(60);
}


void loop(){
  // read data from magnetometer
  if(!mag.isCalibrated()){ //If we're not calibrated
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
  }
  else if (firstRun){
    Serial.println("Calibrated!");
    firstRun = false;
    mag.readMag(&x, &y, &z);
    getIMUdata();
  }
  else {
    mag.readMag(&x, &y, &z);
    getIMUdata();
  }
/* 
  while (Serial.available()){ //read from buffer if more than 2 bytes
  // LF & CR (2 bytes) seem to linger in stream buffer for 1 extra loop() iteration
    parseInput();
  }
 */
}

void parseInput(){
    // stream expected format  = "cmd [args]]"
    // delimiter must be ' ' (1 space) and cmd must terminate w/ a ' '
    // read all chars until 1st space into variable cmd of type String
    String cmd = Serial.readStringUntil(' ');
    if (cmd == "Dist"){
        getDistanceData();
    }
    else if (cmd == "IMU"){
      bool readRaw = Serial.parseInt();
      getIMUdata((bool)readRaw);
    }
}