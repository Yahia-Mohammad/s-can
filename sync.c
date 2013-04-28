#include "sync.h"
#include "uc.h"

static uint8_t bitState = 0;
/*we might need to multiply by TIME_QUANTUM*/
uint8_t segmentLengths[4] = {SYNC_LN, PROP_LN, PHS1_LN, PHS2_LN};
uint8_t processedLastBit = 1;


/*Function: void interruptBitTiming
 This function gets called when the timer reaches the value in the capture (or match) register.*/
void interruptBitTiming()       {
    
    if(bitState == PHS2_SEG)
        bitState = SYNC_SEG;
    else 
        bitState++;
    updateTimer();
    /*
     * We update the state and the timer as soon as possible so that we keep synchronized 
     * as close as possible. Note that the following condition should've been (bitState == PHS1_SEG) if
     * the state update would be delayed after it.
     */
    
    if(bitState == PHS2_SEG)    {
        doSample();
        processedLastBit = 0;
    }
}

void interruptOnChange()        {
    /*
     1)if in SYNC_SEG or PROP_SEG := reset counter
     2)if in PHS1_SEG :=increase PHS1 length.
     3)if in PHS2_SEG :=decrease PHS2 length.
     */
}