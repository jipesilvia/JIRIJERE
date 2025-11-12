
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <math.h>

#include "tkjhat/sdk.h"

// Default stack size for the tasks. It can be reduced to 1024 if task is not using lot of memory.
#define DEFAULT_STACK_SIZE 2048 

//Add here necessary states
enum state { IDLE=1 , CHECK_ORITENTATION};
enum state programState = IDLE;

void buttonFxn(uint gpio, uint32_t eventMask);
void init_buttons();
void resetGyro();

float gyro[3] = {0,0,0};
float accMag = 1;
bool fT = true;

absolute_time_t prevTime;

float ax, ay, az, gx, gy, gz, t;

void gyroTaskFxn(void *arg){

    resetGyro();
    accMag = 1;
    prevTime = get_absolute_time();

    while(1){


        if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
            
            //printf("Accel: X=%f, Y=%f, Z=%f | Gyro: X=%f, Y=%f, Z=%f| Temp: %2.2f°C\n", ax, ay, az, gx, gy, gz, t);

        } else {
            printf("Failed to read imu data\n");
        }


        double dt_s = 10 * pow(10, -3);

        dt_s = absolute_time_diff_us(prevTime, get_absolute_time()) * pow(10, -6);
        prevTime = get_absolute_time();

        accMag = sqrt(ax*ax + ay*ay + az*az);
        //accMag = 2;

        if(accMag < 0.99 || accMag > 1.01){
            gyro[0] += gx * dt_s;
            gyro[1] += gy * dt_s;
            gyro[2] += gz * dt_s;
            printf("gx: %.1f, gy: %.1f, gz: %.1f \n", gyro[0], gyro[1], gyro[2]);
            printf("accMag: %f, dt_s: %f\n", accMag, dt_s);
        } 


        vTaskDelay(10/portTICK_PERIOD_MS);
    }


}

void calibrateGyro(){

    

}

void resetGyro(){

    for(uint8_t i = 0; i < 3; i++){
        gyro[i] = 0;
    }

}

void buttonFxn(uint gpio, uint32_t eventMask){
    
    resetGyro();
    toggle_led();

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


    TaskHandle_t gyroTask = NULL;

    BaseType_t gyroResult = xTaskCreate(
        gyroTaskFxn,
        "GYRO_TASK",
        DEFAULT_STACK_SIZE,
        NULL,
        2,
        &gyroTask);

    
    gpio_set_irq_enabled_with_callback(BUTTON2, GPIO_IRQ_EDGE_RISE, true, buttonFxn);
    

    // Start the scheduler (never returns)
    vTaskStartScheduler();


    // Never reach this line.
    return 0;
}

