
#include <stdio.h>
#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"

#include "tasks.h"
#include "buttons.h"
#include "serial.h"

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

    init_i2c_default();

    init_ICM42670();
    

}

int main() {

    stdio_init_all();
    
    // Uncomment this lines if you want to wait till the serial monitor is connected


    init_hat_sdk();
    init_devices();
    ICM42670_start_with_default_values();
    

    sleep_ms(300);

    while (!stdio_usb_connected()){
        sleep_ms(10);
    }

    init_imu_task();
    init_serial_recieve_task();
    init_buttons_irq();



    // Start the scheduler (never returns)
    vTaskStartScheduler();


    // Never reach this line.
    return 0;
}

