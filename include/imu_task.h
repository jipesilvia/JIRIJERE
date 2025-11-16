#pragma once


typedef struct {
    float x, y, z;
    float x_offSet, y_offSet, z_offSet;
}Gyro_data;


extern Gyro_data gyro_data;


void init_imu_task();
void calibrateGyro();