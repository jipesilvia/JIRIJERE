#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#include "imu_task.h"


#define DEFAULT_STACK_SIZE 2048 


float accMag = 1;

absolute_time_t prevTime;

float ax, ay, az, gx, gy, gz, t;

Gyro_data gyro_data;

void gyroTaskFxn(void *arg){

    //resetGyro();
    accMag = 1;
    prevTime = get_absolute_time();

    while(1){


        if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
            
            //printf("Accel: X=%f, Y=%f, Z=%f | Gyro: X=%f, Y=%f, Z=%f| Temp: %2.2f°C\n", ax, ay, az, gx, gy, gz, t);

        } else {
            printf("Failed to read imu data\n");
        }


        double dt_s = 10 * pow(10, -3);

        dt_s = absolute_time_diff_us(prevTime, get_absolute_time()) * pow(10, -6);
        prevTime = get_absolute_time();

        accMag = sqrt(ax*ax + ay*ay + az*az);
        //accMag = 2;

        if(accMag < 0.99 || accMag > 1.01){
            gyro_data.x += gx * dt_s;
            gyro_data.y += gy * dt_s;
            gyro_data.z += gz * dt_s;
            printf("gx: %.1f, gy: %.1f, gz: %.1f \n", gyro_data.x, gyro_data.y, gyro_data.z);
            printf("accMag: %f, dt_s: %f\n", accMag, dt_s);
        } 

        
        vTaskDelay(pdMS_TO_TICKS(10));
    }




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