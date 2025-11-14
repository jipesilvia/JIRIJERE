#pragma once


typedef struct {
    float x, y, z;
}Gyro_data;

extern Gyro_data gyro_data;

void init_imu_task();