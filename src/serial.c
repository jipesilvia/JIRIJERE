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