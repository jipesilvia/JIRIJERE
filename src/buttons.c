#include <stdio.h>
#include <math.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/stdlib.h>
#include "tkjhat/sdk.h"

#include "tasks.h"
#include "buttons.h"
#include "serial.h"


// Using state machine and task to debounce buttons.
void buttonFxn(uint gpio, uint32_t eventMask){
    
    if(programState != IDLE) return;
    
    if (gpio == BUTTON1){
        programState = SEND_CHAR;

    } else if (gpio == BUTTON2){
        programState = CENTER_GYRO;

    }
} 

void init_buttons_irq(){
    gpio_set_irq_enabled_with_callback(BUTTON1, GPIO_IRQ_EDGE_RISE, true, buttonFxn);
    gpio_set_irq_enabled_with_callback(BUTTON2, GPIO_IRQ_EDGE_RISE, true, buttonFxn);
}
