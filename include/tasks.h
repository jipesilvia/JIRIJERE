#pragma once

typedef struct{
    float x, y, z;
}orientation;

typedef struct {
    orientation orientation;
    orientation offset;
}Gyro_data;

enum state {IDLE=1, CENTER_GYRO, SEND_CHAR};

extern enum state programState;
extern Gyro_data gyro_data;


void init_imu_task();
void init_button_task();
void calibrateGyro();
void resetGyroData();
orientation getRoundedOrientation();