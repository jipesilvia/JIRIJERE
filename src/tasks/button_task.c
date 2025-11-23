#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"
#include "buttons.h"
#include "serial.h"
#include "tasks.h"

#define DEFAULT_STACK_SIZE 2048



void buttonTaskFxn(void *arg){

    while(1){

        if(programState == SEND_CHAR){

            toggle_led();
            print_char();
            vTaskDelay(pdMS_TO_TICKS(200));
            toggle_led();
            programState = IDLE;

        }else if(programState == CENTER_GYRO){

            toggle_led();
            resetGyroData();
            vTaskDelay(pdMS_TO_TICKS(200));
            toggle_led();
            programState = IDLE;
            
        }

        vTaskDelay(pdMS_TO_TICKS(50));
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