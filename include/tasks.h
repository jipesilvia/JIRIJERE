#pragma once

typedef struct{
    float x, y, z;
}orientation;

typedef struct {
    orientation orientation;
    orientation offset;
}Gyro_data;


extern Gyro_data gyro_data;


void init_imu_task();
void calibrateGyro();
void resetGyroData();
orientation getRoundedOrientation();