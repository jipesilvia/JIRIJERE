
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

float gyro[3] = {0,0,0};

int i = 0;

float ax, ay, az, gx, gy, gz, t;

//ehkä pitää laittaa argumentti!!!!!!
void gyroTaskFxn(void *arg){


    while(1){

        if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
            
            //printf("Accel: X=%f, Y=%f, Z=%f | Gyro: X=%f, Y=%f, Z=%f| Temp: %2.2f°C\n", ax, ay, az, gx, gy, gz, t);

        } else {
            printf("Failed to read imu data\n");
        }


        
        if(i == 10){
            printf("gx: %.5f, gy: %.5f, gz: %.5f \n", gyro[0], gyro[1], gyro[2]);

            i = 0;
        }
        i++;


        uint8_t GYRO_SENSIVITY = 131;
        float dt_s = 0.01;

        /*if(fabsf(gx) > 1) gyro[0] += gx / GYRO_SENSIVITY * dt_s;
        if(fabsf(gy) > 1) gyro[1] += gy / GYRO_SENSIVITY * dt_s;
        if(fabsf(gz) > 1) gyro[2] += gz / GYRO_SENSIVITY * dt_s;*/

        gyro[0] += gx / GYRO_SENSIVITY * dt_s;
        gyro[1] += gy / GYRO_SENSIVITY * dt_s;
        gyro[2] += gz / GYRO_SENSIVITY * dt_s;

        
        for(int n = 0; n < 3; n++){
            if(fabsf(gyro[n]) > 180){
                gyro[n] = gyro[n] - ((gyro[n]/fabsf(gyro[n])) * 360);
            }
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }


}



void buttonFxn(uint gpio, uint32_t eventMask){
    
    printf("%f, %f, %f\n", gx, gy, gz);
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

    sleep_ms(300); //Wait some time so initialization of USB and hat is done.
    
    // Uncomment this lines if you want to wait till the serial monitor is connected
    while (!stdio_usb_connected()){
        sleep_ms(10);
    }

    init_hat_sdk();
    init_inits();
    ICM42670_start_with_default_values();

    printf("nyt vittu!\n");

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

