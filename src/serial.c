#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tasks.h"
#include "buttons.h"
#include "serial.h"
#include "tkjhat/sdk.h"

#define DEFAULT_STACK_SIZE 2048
#define INPUT_BUFFER_SIZE 256

orientation up = {90, 0, 0};
orientation straight = {0, 0, 0};
orientation right = {0, 0, -90};
orientation left = {0, 0, 90};

bool compareOrientations(orientation a, orientation b);
void displayMessage(char *line);


// Prints character based on the current orientation of IMU.
// 
void print_char(){
    orientation current = getRoundedOrientation();
    if (compareOrientations(current, straight)){
        printf(".");
    } else if (compareOrientations(current, left)){
        printf("-");
    } else if (compareOrientations(current, up)){
        putchar_raw('\n');
    } else if (compareOrientations(current, right)){
        putchar_raw(' ');
    }
    
    //gyro_data.orientation = current; // sets the orientation to the rounded values.
     // The point of this is to clear any accidental drift caused by user. But this has it's flaws.
}


// Compares two orientation type objects. Returns true if equal.
// Y-axis excluded because it caused unnecessary falses.
bool compareOrientations(orientation a, orientation b){

    if(a.x != b.x){
        return false;
    }
    
    /*if(a.y != b.y){
        return false;
    }*/

    if(a.z != b.z){
        return false;
    }

    return true;

}

static void receive_task(void *arg){
    (void)arg;
    char line[INPUT_BUFFER_SIZE];
    size_t index = 0;
    
    while (1){
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT && programState == IDLE){// I have received a character
            if (c == '\r') continue; // ignore CR, wait for LF if (ch == '\n') { line[len] = '\0';
            if (c == '\n'){
                // terminate and process the collected line
                line[index] = '\0'; 
                printf("__[RX]:\"%s\"__\n", line); //Print as debug in the output
                index = 0;
                programState = DISPLAY_MESSAGE;
                vTaskDelay(pdMS_TO_TICKS(100)); // Wait for new message
            }
            else if(index < INPUT_BUFFER_SIZE - 1){
                line[index++] = (char)c;
            }
            else { //Overflow: print and restart the buffer with the new character. 
                line[INPUT_BUFFER_SIZE - 1] = '\0';
                printf("__[RX]:\"%s\"__\n", line);
                index = 0; 
                line[index++] = (char)c; 
            }
        }
        else if (programState == DISPLAY_MESSAGE){
            displayMessage(line);
            programState = IDLE;
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(100)); // Wait for new message
        }
    }
}

void displayMessage(char *line){
    clear_display();
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++){
        if (line[i] == "-"){
            draw_line(32, 32, 96, 32);
            vTaskDelay(pdMS_TO_TICKS(100));
        } else if (line[i] == "."){
            draw_circle(64, 32, 20, true);
            vTaskDelay(pdMS_TO_TICKS(100));
        } else if (line[i] == "\n"){
            return;
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void init_serial_recieve_task(){
    TaskHandle_t hReceiveTask;
    BaseType_t result = xTaskCreate(receive_task,
                "receive",
                DEFAULT_STACK_SIZE,
                NULL,
                2,
                &hReceiveTask);
}

