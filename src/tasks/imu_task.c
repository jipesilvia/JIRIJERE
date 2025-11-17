#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#include "imu_task.h"


#define DEFAULT_STACK_SIZE 2048 
#define CALIBRATION_SET_SIZE 200

float accMag = 1;
float dt_s = 0.01;

absolute_time_t prevTime;

float ax, ay, az, gx, gy, gz, t;
float tempGyroSums[3] = {0, 0, 0};

Gyro_data gyro_data;


uint8_t calibrationCounter = 0;
bool isCalibrating = false;




uint8_t blank = 0;

void resetGyroData();


void gyroTaskFxn(void *arg){

    //resetGyro();
    accMag = 1;
    prevTime = get_absolute_time();
    calibrateGyro();

    while(1){


        if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
            
            //printf("Accel: X=%f, Y=%f, Z=%f | Gyro: X=%f, Y=%f, Z=%f| Temp: %2.2f°C\n", ax, ay, az, gx, gy, gz, t);

        } else {
            printf("__Failed to read imu data__");
        }
        dt_s = absolute_time_diff_us(prevTime, get_absolute_time()) * pow(10, -6);
        prevTime = get_absolute_time();

        accMag = sqrt(ax*ax + ay*ay + az*az);
        //accMag = 2;

        if(isCalibrating){

            if(accMag > 0.99 && accMag < 1.01){
                gyro_data.x += gx;
                gyro_data.y += gy;
                gyro_data.z += gz;

                calibrationCounter++;

            }else{

                printf("__stop moving! %d__", calibrationCounter);

            }
            
            if(calibrationCounter >= CALIBRATION_SET_SIZE - 1){

                gyro_data.x_offSet = gyro_data.x / CALIBRATION_SET_SIZE;
                gyro_data.y_offSet = gyro_data.y / CALIBRATION_SET_SIZE;
                gyro_data.z_offSet = gyro_data.z / CALIBRATION_SET_SIZE;
                resetGyroData();
                isCalibrating = false;
                calibrationCounter = 0;
                printf("__Calibrated!__");
                printf("__Off sets: x: %f, y: %f, z: %f__", gyro_data.x_offSet, gyro_data.y_offSet, gyro_data.z_offSet);

            }

        }else{

            gyro_data.x += (gx - gyro_data.x_offSet) * dt_s;
            gyro_data.y += (gy - gyro_data.y_offSet) * dt_s;
            gyro_data.z += (gz - gyro_data.z_offSet) * dt_s;

            // prevent multiple rounds
            if (gyro_data.x < 0){
                gyro_data.x = 360 - gyro_data.x;
            } else if (gyro_data.x > 360){
                gyro_data.x -= 360;
            }

            if(blank = 10){
                // printf("gx: %.5f, gy: %.5f, gz: %.5f \n", gyro_data.x, gyro_data.y, gyro_data.z);
                // printf("accMag: %f, dt_s: %f\n", accMag, dt_s);
                blank = 0;
            }

            blank++;

        }
        
        vTaskDelay(pdMS_TO_TICKS(10));

    }
    
}

void calibrateGyro(){
    
    resetGyroData();
    isCalibrating = true;

}


void resetGyroData(){

    gyro_data.x = 0;
    gyro_data.y = 0;
    gyro_data.z = 0;

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