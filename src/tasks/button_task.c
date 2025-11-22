#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#define DEFAULT_STACK_SIZE 2048



void buttonTaskFxn(void *arg){

    while(1){

        


    }


}




void init_button_task(){

    TaskHandle_t buttonTask = NULL;

    BaseType_t gyroResult = xTaskCreate(
    buttonTaskFxn,
    "GYRO_TASK",
    DEFAULT_STACK_SIZE,
    NULL,
    2,
    &buttonTask);

}