// include libraries for 9DoF sensor
#include <LSM9DS1_Registers.h>
#include <LSM9DS1_Types.h>
#include <SparkFunLSM9DS1.h>
// include libraries for 6DoF sensor
#include <MPU6050_tockn.h>
#include <Wire.h>
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
// declare & initialize stepper motor
Stepper neck(stepsPerRevolution, sPins[0], sPins[1], sPins[2], sPins[3]);
// declare size of/& array for HCSR04 using NewPing library
const unsigned char totalDistSensors = 4;
NewPing sonar[totalDistSensors] = {
    NewPing(sensor1, sensor1, max_distance),
    NewPing(sensor2, sensor2, max_distance),
    NewPing(sensor3, sensor3, max_distance),
    NewPing(sensor4, sensor4, max_distance)};

// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M	0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG	0x6B // Would be 0x6A if SDO_AG is LOW
// declare 9DoF chip
LSM9DS1 imu;
// declare 6DoF chip
MPU6050 mpu6050(Wire);

void setup(){
	Serial.begin(115200);// open a channel to pour data into & get commands
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
        Serial.println("Failed to communicate with LSM9DS1.");
        Serial.println("Double-check wiring.");
        Serial.println("Default settings in this sketch will " \
                      "work for an out of the box LSM9DS1 " \
                      "Breakout, but may need to be modified " \
                      "if the board jumpers are.");
        while (1); // infinite while loop to stop program if sensor not found
    }
    // calibrate the 9DoF sensors. Stores bias if arg == true
    imu.calibrate(true);// calibrates Accel & gyro
    imu.calibrateMag(true);// calibrates magnetometer

    // initialize the 6DoF chip & self-calibrate
    Wire.begin();
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);// robot should be still during boot up
}

void loop(){
    // Update the sensor values whenever new data is available
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

    // get & print 6DoF sensor data
    Serial.print("6DoF");Serial.print(mpu6050.getAccX());
    Serial.print(",");Serial.print(mpu6050.getAccY());
    Serial.print(",");Serial.print(mpu6050.getAccZ());
    Serial.print(",");Serial.print(mpu6050.getGyroX());
    Serial.print(",");Serial.print(mpu6050.getGyroY());
    Serial.print(",");Serial.println(mpu6050.getGyroZ());
    
    
    // get & print distance sensor data
    Serial.print("Dist");
    for (unsigned char i = 0; i < totalDistSensors; i++){
        Serial.print(",");
        Serial.print(sonar[i].ping_cm());
    }
    Serial.println("");
    // get and print drive motor speed (PWM: 0-255)
    Serial.print("Driv,");
    Serial.print(d->getSpeed(0));
    Serial.print(",");
    Serial.println(d->getSpeed(1));

    while (Serial.available() > 2){ //read from buffer if more than 2 bytes
	    // LF & CR (2 bytes) seem to linger in stream buffer for 1 extra loop() iteration
	    // stream expected format  = "# #" where # == [-100,100]
	    // delimiter can be any non-digit character (example above uses ' ')
	    // use x for left-right steering
        // use y for forward-backward drive
        short x = Serial.parseInt();
        short y = Serial.parseInt();
        d->go(x, y);
	}
    delayMicroseconds(60);
}
