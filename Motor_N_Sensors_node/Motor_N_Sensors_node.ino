// for parseing datatype String
#include <String.h>

// include libraries for 9DoF sensor
#include <LSM9DS1_Registers.h>
#include <LSM9DS1_Types.h>
#include <SparkFunLSM9DS1.h>
// need to find optimized library for the MPU6050; library by _tockn is a memory hog

// include library for GPS sensor
#include <SparkFun_Ublox_Arduino_Library.h>
// include library for ultrasonic distance sensors
#include <NewPing.h>

// include library for stepper motors
#include <Stepper.h>

// define motor type: 1 to enable direction + PWM or 0 to enable 2 PWM
#define phased 1
// define drivetrain type: 0 to enable BiPed (R2D2-like), 1 to enable QuadPed (like a race car)
#define motorConfig 0
#include "Drivetrain.h"

// define PWM control pins for drive motors
#define M1Forward 	6
#define M1Backward 	9
#define M2Forward 	10
#define M2Backward 	11

// define HC-SR04 pins (echo and trigger are connected to same pin) for NewPing library
#define sensor1     13
#define sensor2     12
#define sensor3     8
#define sensor4     7
// define max sensor distance for HC-SR04 (in centimeters)
#define max_distance 400

// define controls pins for stepper motor
unsigned char sPins[4] = {2, 3, 4, 5};
// change this to fit the number of steps per revolution for your stepper motor
const int stepsPerRevolution = 200;
// declare drivetrain
Drivetrain* d;
short rpm = 60; // rpms are based on <= stepper motor's rated max RPM
// declare & initialize stepper motor
Stepper neck(stepsPerRevolution, sPins[0], sPins[1], sPins[2], sPins[3]);

// declare size of/& array for HCSR04 using NewPing library
const unsigned char totalDistSensors = 4;
NewPing sonar[totalDistSensors] = {
    NewPing(sensor1, sensor1, max_distance),
    NewPing(sensor2, sensor2, max_distance),
    NewPing(sensor3, sensor3, max_distance),
    NewPing(sensor4, sensor4, max_distance)};

// SDO_M and SDO_AG are both pulled LOW, so our addresses are:
#define LSM9DS1_M	0x1C // Would be 0x1E if SDO_M is FLOATING
#define LSM9DS1_AG	0x6A // Would be 0x6B if SDO_AG is FLOATING
// declare 9DoF chip
LSM9DS1 imu;
#define DECLINATION -13.44
long timer, dt; // a timer to save delta timer for each loop.
double roll, pitch ,yaw; //These are the angles in the complementary filter

// declare 6DoF chip
// MPU6050 mpu6050(Wire);

// declare gps object
SFE_UBLOX_GPS myGPS;


void setup(){
	Serial.begin(115200);// open a channel to pour data into & get commands
    neck.setSpeed(rpm);
    
    // instantiate drivetrain object (motors' speeds default to 0)
    if (motorConfig)
        d = new QuadPed(M1Forward, M1Backward, M2Forward, M2Backward, phased);
    else d = new BiPed(M1Forward, M1Backward, M2Forward, M2Backward, phased);
    
    // Before initializing the IMU, there are a few settings
    // we may need to adjust. Use the settings struct to set
    // the device's communication mode and addresses:
    imu.settings.device.commInterface = IMU_MODE_I2C;
    imu.settings.device.mAddress = LSM9DS1_M;
    imu.settings.device.agAddress = LSM9DS1_AG;
    // The above lines will only take effect AFTER calling
    // imu.begin(), which verifies communication with the IMU
    // and turns it on.
    if (!imu.begin()){
        Serial.println("Failed to communicate with LSM9DS1. Double-check wiring.");
        while (1); // infinite while loop to stop program if sensor not found
    }
    // calibrate the 9DoF sensors. Stores bias if arg == true
    imu.calibrate(true);// calibrates Accel & gyro
    imu.calibrateMag(true);// calibrates magnetometer
    //start a timer
    timer = micros();
}

void loop(){
    while (Serial.available()){ //read from buffer if more than 2 bytes
	    // LF & CR (2 bytes) seem to linger in stream buffer for 1 extra loop() iteration
	    parseInput();
	}

}

void parseInput(){
    // stream expected format  = "cmd [args]]"
    // delimiter must be ' ' (1 space) and cmd must terminate w/ a ' '
    // read all chars until 1st space into variable cmd of type String
    String cmd = Serial.readStringUntil(' ');
    if (cmd == "Driv"){
        // args expected format = "x y" where both x & y == [-255,255]
        // use x for left-right steering
        // use y for forward-backward drive
        short x = 0;
        short y = 0;
        x = Serial.parseInt();
        y = Serial.parseInt();
        d->go(x, y);
        /* 
        // for debugging
        Serial.print("received args: ");
        Serial.print(x);
        Serial.print(",");
        Serial.println(y);
         */
        getDriveData();
    }
    else if (cmd == "Driv?"){
        getDriveData();
    }
    else if (cmd == "Dist"){
        getDistanceData();
    }
    else if (cmd == "IMU"){
        getIMUdata();
    }
    else if (cmd == "HYPR"){
        // HYPR = Heading Yaw Pitch Roll
        getHYPR();
    }
    else if (cmd == "Step"){
        // args expected format = "steps [rpm]" 
        // where steps == # of steps to turn stepper 
        // steps can be negative for reversing direction
        // rpm is optional and is used to set new speed of rotation
        // rpm cannot be negative
        short steps = Serial.parseInt();
        unsigned short newRPM = Serial.parseInt();
        if (newRPM != 0){
            rpm = newRPM;
            neck.setSpeed(rpm);
        }
        // Serial.print("args received: ");
        // Serial.print("RPM=");Serial.print(rpm);
        // Serial.print(" steps=");Serial.println(steps);
        neck.step(steps);
    }
}

void getDriveData(){
    // get and print drive motor speed (PWM: 0-255)
    Serial.print("Driv,");
    Serial.print(d->getSpeed(0));
    Serial.print(",");
    Serial.println(d->getSpeed(1));
}

void readIMU(){
    timer = micros(); // reset timer
    if ( imu.gyroAvailable() ){
        // To read from the gyroscope,  first call the
        // readGyro() function. When it exits, it'll update the
        // gx, gy, and gz variables with the most current data.
        imu.readGyro();
    }
    if ( imu.accelAvailable() ){
        // To read from the accelerometer, first call the
        // readAccel() function. When it exits, it'll update the
        // ax, ay, and az variables with the most current data.
        imu.readAccel();
    }
    if ( imu.magAvailable() ){
        // To read from the magnetometer, first call the
        // readMag() function. When it exits, it'll update the
        // mx, my, and mz variables with the most current data.
        imu.readMag();
    }
    dt = micros() - timer;
    timer = micros(); // reset timer
}
void getIMUdata(){
    readIMU();
/* 
    // get & print 6DoF sensor data
    Serial.print("6DoF");Serial.print(mpu6050.getAccX());
    Serial.print(",");Serial.print(mpu6050.getAccY());
    Serial.print(",");Serial.print(mpu6050.getAccZ());
    Serial.print(",");Serial.print(mpu6050.getGyroX());
    Serial.print(",");Serial.print(mpu6050.getGyroY());
    Serial.print(",");Serial.println(mpu6050.getGyroZ());
 */    
    // get & print 9DoF sensor data
    Serial.print("9DoF,");Serial.print(imu.gx);
    Serial.print(",");Serial.print(imu.gy);
    Serial.print(",");Serial.print(imu.gz);
    Serial.print(",");Serial.print(imu.ax);
    Serial.print(",");Serial.print(imu.ay);
    Serial.print(",");Serial.print(imu.az);
    Serial.print(",");Serial.print(imu.mx);
    Serial.print(",");Serial.print(imu.my);
    Serial.print(",");Serial.println(imu.mz);
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

void getHYPR(){
    double heading;
    if (imu.mx == 0)
        heading = imu.my < 0 ? PI / 2 : 0;
    else heading = atan2(imu.my, imu.mx);

    // Convert everything from radians to degrees:
    heading *= 180 / PI;
    heading -= DECLINATION;

    // ensure proper range of [0, 360]
    if (heading > 360) heading -= 360;
    else if (heading < 0) heading += 360;

    Serial.print("HYPR: "); Serial.print(heading, 2);
/*   
    If heading is greater than 337.25 degrees or less than 22.5 degrees – North
    If heading is between 292.5 degrees and 337.25 degrees – North-West
    If heading is between 247.5 degrees and 292.5 degrees – West
    If heading is between 202.5 degrees and 247.5 degrees – South-West
    If heading is between 157.5 degrees and 202.5 degrees – South
    If heading is between 112.5 degrees and 157.5 degrees – South-East
    If heading is between 67.5 degrees and 112.5 degrees – East
    If heading is between 0 degrees and 67.5 degrees – North-East
 */
    // calculate the orientation of the accelerometer and convert the output of atan2 from radians to degrees
    // this data is used to correct any cumulative errors in the orientation that the gyroscope develops.
    double rollangle=atan2(imu.ay,imu.az)*180/PI;
    double pitchangle=atan2(imu.ax,sqrt(imu.ay*imu.ay+imu.az*imu.az))*180/PI;

    //THE COMPLEMENTARY FILTER
    //This filter calculates the angle based MOSTLY on integrating the angular velocity to an angular displacement.
    //dt is the time between loop() iterations. 
    //We'll pretend that the angular velocity has remained constant over the time dt, and multiply angular velocity by time to get displacement.
    //The filter then adds a small correcting factor from the accelerometer ("roll" or "pitch"), so the gyroscope knows which way is down.
    roll = 0.99 * (roll + imu.gx * (dt / 1000000.0)) + 0.01 * rollangle; // Calculate the angle using a Complimentary filter
    pitch = 0.99 * (pitch + imu.gy * (dt / 1000000.0)) + 0.01 * pitchangle;
    yaw=imu.gz;

    Serial.print(",");Serial.println(yaw);
    Serial.print(",");Serial.print(pitch);
    Serial.print(",");Serial.print(roll);
}

