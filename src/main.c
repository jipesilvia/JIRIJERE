
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

void buttonFxn(uint gpio, uint32_t eventMask);
void init_buttons();






void init_inits(){

    init_button1();
    init_button2();

    init_led();

    init_i2c_default();

    init_ICM42670();
    

}

int main() {

    stdio_init_all();
    
    // Uncomment this lines if you want to wait till the serial monitor is connected
    while (!stdio_usb_connected()){
        sleep_ms(10);
    }

    init_hat_sdk();
    init_inits();
    //ICM42670_start_with_default_values();

    ICM42670_enable_accel_gyro_ln_mode();
    ICM42670_startAccel(ICM42670_ACCEL_ODR_DEFAULT,
                             ICM42670_ACCEL_FSR_DEFAULT);
    ICM42670_startGyro(ICM42670_GYRO_ODR_DEFAULT,
                             500);
    
    

    sleep_ms(300);

    init_imu_task();

    init_buttons();

    // Start the scheduler (never returns)
    vTaskStartScheduler();


    // Never reach this line.
    return 0;
}

