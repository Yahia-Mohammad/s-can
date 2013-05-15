#include "hal.h"

void getSample() {
    /*This when we should read the next bit (from Rx pin) */
    
    /*... read from the micro IO pin
     nxtBit = ...
     */
}

void putBit(uint8_t bit)   {
    /* Write a bit on the Tx pin */
}

void updateCapture(int8_t shift)      {
    /* This is where we should update the capture/match register with the curren bit segment length */
    /* a shift is added to length right before the update */
}

void Intialize_uC()      {
    /* Initializes the uC, and install the interrupt service routines (ISRs)*/
    return ;
}

void resetTimer()   {
    /* Resets the timer back to zero */
}

uint8_t getTimer() {
    /* Get the timer value */
}