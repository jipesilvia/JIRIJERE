#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#include "tasks.h"


#define DEFAULT_STACK_SIZE 2048 
#define CALIBRATION_SET_SIZE 200

// Acceleration magnitude
float accMag = 1;
float dt_s = 0.01;

absolute_time_t prevTime;

float ax, ay, az, gx, gy, gz, t;

// Initializes global gyro_data variable
Gyro_data gyro_data;

// Sums for calculating calibration offset
float tempXsum = 0, tempYsum = 0, tempZsum = 0;
uint8_t calibrationCounter = 0;
bool isCalibrating = false;

// For debugging!
int blank = 0;

void init_calibration();
float roundAngle(float angle);

void gyroTaskFxn(void *arg){

    accMag = 1;
    prevTime = get_absolute_time();
    calibrateGyro();

    while(1){

        if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
            //printf("Accel: X=%f, Y=%f, Z=%f | Gyro: X=%f, Y=%f, Z=%f| Temp: %2.2f°C\n", ax, ay, az, gx, gy, gz, t);
        } else {
            printf("__Failed to read imu data__");
        }

        dt_s = absolute_time_diff_us(prevTime, get_absolute_time()) * pow(10, -6); // calculate the time passed from previous iteration.
        prevTime = get_absolute_time(); // set previoustime to current time.

        accMag = sqrt(ax*ax + ay*ay + az*az);

        // Either calibrate the gyro or update the orientation.
        if(isCalibrating){
            // add gyroscore drift to the sums only if the device is steady.
            // otherwise starts over the calibration.
            if(accMag > 0.99 && accMag < 1.01){
                tempXsum += gx;
                tempYsum += gy;
                tempZsum += gz;

                calibrationCounter++;

            }else{

                printf("__Don't move the device while calibrating!__");
                vTaskDelay(pdMS_TO_TICKS(2000));
                calibrateGyro();

            }
            
            // calculates the average drift of gyro when enough data is collected.
            if(calibrationCounter >= CALIBRATION_SET_SIZE - 1){

                gyro_data.offset.x = tempXsum / CALIBRATION_SET_SIZE;
                gyro_data.offset.y = tempYsum / CALIBRATION_SET_SIZE;
                gyro_data.offset.z = tempZsum / CALIBRATION_SET_SIZE;
                resetGyroData();
                
                isCalibrating = false;

                printf("__Calibrated!__");
                printf("__Off sets: x: %f, y: %f, z: %f__", gyro_data.offset.x, gyro_data.offset.y, gyro_data.offset.z);
                // start new message
                printf(" ");
                printf(" ");
                printf("\n");
            }

        }else{

            gyro_data.orientation.x += (gx - gyro_data.offset.x) * dt_s;
            gyro_data.orientation.y += (gy - gyro_data.offset.y) * dt_s;
            gyro_data.orientation.z += (gz - gyro_data.offset.z) * dt_s;

            
            //for debugging
            if(blank = 1000){
                //printf("gx: %.5f, gy: %.5f, gz: %.5f \n", gyro_data.orientation.x, gyro_data.orientation.y, gyro_data.orientation.z);
                //printf("accMag: %f, dt_s: %f\n", accMag, dt_s);
                blank = 0;
            }
            blank++;
            

        }
        
        vTaskDelay(pdMS_TO_TICKS(10));

    }
    
}


// Rounds orientation x,y and z to 0 or +-90
orientation getRoundedOrientation(){

    orientation rounded;
    
    rounded.x = roundAngle(gyro_data.orientation.x);
    rounded.y = roundAngle(gyro_data.orientation.y);
    rounded.z = roundAngle(gyro_data.orientation.z);

    return rounded;
}

// Rounds angle to 0 or +-90
float roundAngle(float angle){

    float angleAbs = fabsf(angle);
    float angleSign = angleAbs / angle;

    if(angleAbs < 40){
        return 0;
    }else if(angleAbs > 50){
        return 90 * angleSign;
    }else{
        return 0;
    }

}

// Starts calibration
void calibrateGyro(){
    printf("__Calibrating gyro. Keep device still!__");
    init_calibration();
    resetGyroData();
    isCalibrating = true;

}

// Resets gyro orientation to zero
void resetGyroData(){

    gyro_data.orientation.x = 0;
    gyro_data.orientation.y = 0;
    gyro_data.orientation.z = 0;

}

// Initializes variables needed for calibration
void init_calibration(){

    calibrationCounter = 0;
    tempXsum = 0;
    tempYsum = 0;
    tempZsum = 0;

}

void init_imu_task(){

    TaskHandle_t gyroTask = NULL;

    BaseType_t gyroResult = xTaskCreate(
    gyroTaskFxn,
    "GYRO_TASK",
    DEFAULT_STACK_SIZE,
    NULL,
    2,
    &gyroTask);

}