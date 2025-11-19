#include <stdio.h>

#include "imu_task.h"
#include "buttons.h"
#include "serial.h"

void print_char(){
    if (gyro_data.x > 330 || gyro_data.x < 30){
        printf(".");
    } else if (gyro_data.x > 60 && gyro_data.x < 120){
        printf("-");
    } else if (gyro_data.x > 150 && gyro_data.x < 210){
        printf("\n");
    } else if (gyro_data.x > 240 && gyro_data.x < 300){
        printf(" ");
    }
}