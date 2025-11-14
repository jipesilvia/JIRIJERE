
#include <stdio.h>
#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "tkjhat/sdk.h"

#include "imu_task.h"

// Default stack size for the tasks. It can be reduced to 1024 if task is not using lot of memory.
#define DEFAULT_STACK_SIZE 2048 

//Add here necessary states
enum state { IDLE=1 , CHECK_ORITENTATION};
enum state programState = IDLE;

void buttonFxn(uint gpio, uint32_t eventMask);
void init_buttons();




void buttonFxn(uint gpio, uint32_t eventMask){
    
    toggle_led();
    calibrateGyro();

}

void init_inits(){

    init_button1();
    init_button2();

    init_led();

    init_i2c_default();

    init_ICM42670();
    init_rgb_led();
    stop_rgb_led();

}

int main() {

    stdio_init_all();
    
    // Uncomment this lines if you want to wait till the serial monitor is connected
    while (!stdio_usb_connected()){
        sleep_ms(10);
    }

    init_hat_sdk();
    init_inits();
    ICM42670_start_with_default_values();

    sleep_ms(300);

    init_imu_task();

    gpio_set_irq_enabled_with_callback(BUTTON2, GPIO_IRQ_EDGE_RISE, true, buttonFxn);
    

    // Start the scheduler (never returns)
    vTaskStartScheduler();


    // Never reach this line.
    return 0;
}

