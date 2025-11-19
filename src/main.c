
#include <stdio.h>
#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"

#include "imu_task.h"
#include "buttons.h"

// Default stack size for the tasks. It can be reduced to 1024 if task is not using lot of memory.
#define DEFAULT_STACK_SIZE 2048 

//Add here necessary states
enum state { IDLE=1 , CHECK_ORITENTATION};
enum state programState = IDLE;

void init_devices();

void init_devices(){

    init_button1();
    init_button2();

    init_led();

    init_ICM42670();

}

int main() {

    stdio_init_all();

    init_hat_sdk();

    init_devices();

    init_i2c_default();

    ICM42670_start_with_default_values();

    init_imu_task();

    init_buttons_irq();


    // Uncomment this lines if you want to wait till the serial monitor is connected
    while (!stdio_usb_connected()){
        sleep_ms(10);
    }

    sleep_ms(300);

    // Start the scheduler (never returns)
    vTaskStartScheduler();


    // Never reach this line.
    return 0;
}

