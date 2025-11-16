#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#include "imu_task.h"
#include "buttons.h"


void buttonFxn(uint gpio, uint32_t eventMask){
    
    toggle_led();
    calibrateGyro();

}


void init_buttons(){

    gpio_set_irq_enabled_with_callback(BUTTON2, GPIO_IRQ_EDGE_RISE, true, buttonFxn);

}