#include "controller.h"
#include "can_bus.h"
#include "sync.h"
#include "error.h"
#include "crc.h"

void (*stateFunction[NUM_STATES])() = {stateIdle, stateSOF,
stateArbitration, stateControl, stateData, stateCRC,
stateACK, stateEOF, stateIntermission, stateSuspend,
stateOverload, stateOverloadDelimiter, stateError, stateErrorDelimiter};

uint8_t nxtBit = RECESSIVE;

uint8_t incomingBuffer[INCOMING_BUFFER_SIZE] = {0};
uint32_t nodeFilters [FILTERS_NUM] = {0};
uint8_t dataLength = 0;
uint8_t RTR = 0;
uint8_t matchedFilterIndex = MAX_FILTERS_NUM;
uint8_t initializeState = 1; 
uint8_t generateACK = 0;
uint8_t matchCRC = 0;
uint8_t delimiterCRC = 0;
uint8_t delimiterACK = 0;

uint8_t controllerMode = CM_RECEIVE;


/* IMPORTANT : Should be initialized to  stateFunction[BS_IDLE]*/
void (*currentStateFunction)() = 0;

void stateIdle()        {
    processedLastBit = 1;
    if(nxtBit == DOMINANT)  {
        initializeState = 1;
        currentStateFunction = stateFunction[BS_ARBITRATION_CONTROL];  
    }
}

void stateSOF() {
    /* This function is redundant and should be removed */
}

void stateArbitration() {
    static uint8_t extended = 0;
    static uint8_t bitCounter = 0;
    static uint8_t identifierCounter = 11;
    static uint32_t identifier = 0;    
    static uint8_t endSection = 0;
    processedLastBit = 1;
    
    if(initializeState) {
        extended = 2;
        bitCounter = 0;
        identifierCounter = 0;
        identifier = 0;
        endSection = 0;
        localCRC = 0;
        initializeState = 0;
        
        /* Counting for SOF */
        UPDATE_CRC(localCRC, DOMINANT); 
        
        lstBit = DOMINANT;
        bitRepetitionCount = 1;
    }
    
    uint8_t i; /* Used as an iterator, a precaution in case the compiler used only support ISO C */
      
    /* we should check for other errors as well */
    
    /* Increase the bit counter */
    bitCounter++;
    
    UPDATE_CRC(localCRC, nxtBit);
    
    if(identifierCounter != 0)  {  
        UPDATE_REGISTER(identifier, nxtBit);
        identifierCounter--;
    }
    
    if(bitCounter == 12)
        RTR = nxtBit;
    
    if(bitCounter == 13)        {
        
        extended = nxtBit;
        if(extended == RECESSIVE) 
            identifierCounter = 18;
    }
    
    /* Extended Frame Format */
    if(extended == RECESSIVE)   {
        if(bitCounter == 42)    
            RTR = nxtBit;
        
        if((bitCounter >= 45) && (bitCounter <= 48))
            UPDATE_REGISTER(dataLength, !nxtBit);
        
        if(bitCounter == 48)
            endSection = 1;
    }
    
    /* Standard Frame Format */
    if(extended == DOMINANT)    {
        if((bitCounter >= 15) && (bitCounter <= 18 ))
            UPDATE_REGISTER(dataLength, !nxtBit);
        
        if(bitCounter == 18)
            endSection = 1;
    }
    
    if(endSection)      {    
        if(RTR == RECESSIVE)
            dataLength = 0; /* The CAN specifications state that in RTR, data length should be 0 regardless of the DLC */
        /* Compare with stored filters, then set a flag if there is a match */  
        /* Linear search algorithm is good enough (or even best choice) for the small array of filters we have. */   
        for(i = 0; i < FILTERS_NUM; i++)
            if(nodeFilters[i] == identifier)    {
                matchedFilterIndex = i; break;
            }
        
        if(RTR)
            currentStateFunction = stateFunction[BS_CRC];
        else
            currentStateFunction = stateFunction[BS_DATA];
        
        initializeState = 1;
        
        /* All the clean up goes here, all static and related global variables, then set the next state 
         * WE DO NOT CLEAR stuffingRegister here 
         * When setting the next state, we might set it to CRC state directly if it's 0 data length (like RTR) */
    }
    
}

void stateControl()     {
    /* Redundant : should be removed */
}

void stateData()        {
    /* 
     * These variables hold the remaining expected data, so if (for example) the data length is 1, then we set :
     * 
     * bitCounter = 8;
     * byteCounter = 0;
     * 
     * and if we have (for example) 8 byte data length, then we set :
     * bitCounter = 8
     * byteCounter = 7;
     *
     */
    static uint8_t bitCounter; 
    static uint8_t byteCounter;

    processedLastBit = 1;
    if(initializeState) {
        bitCounter = 8;
        byteCounter = dataLength - 1;
        initializeState = 0;
    }
    
    if(bitCounter == 0) {
        byteCounter--;
        bitCounter = 7;
    }
    else
        bitCounter--;
    
    UPDATE_REGISTER(incomingBuffer[byteCounter], nxtBit);
    
    UPDATE_CRC(localCRC, nxtBit);
      
    if((bitCounter == 0) && (byteCounter == 0)) {
        /* do the clean up and prepare for the next state 
         * clear corresponding global and static variables, clear buffers and set next state */
        currentStateFunction = stateFunction[BS_CRC];
        initializeState = 1;
    }   
}


void stateCRC() {
    static uint8_t bitCounter = 0;
    processedLastBit = 1;
    if(initializeState) {
        bitCounter = 0;
        receivedCRC = 0;
        initializeState = 0;
    }
    
    if(bitCounter <= 15) {
        
        if(bitCounter < 15) {
            UPDATE_REGISTER(receivedCRC, nxtBit);
        }
        else    {
            delimiterCRC = 1;
            currentStateFunction = stateFunction[BS_CRC_DELIMITER];
        }
        bitCounter++;
    }
    
#if 0
    if(bitCounter == 16)    {
        /* transition to next state*/
        
    }
#endif
}

void stateCRC_Delimiter()   {
    /* When this function is called : the controller is on the CRC delimiter, and preparing for ACK bit generation
     * on the beginning of the next bit. When it reaches the sample point, it'll then prepare for the ACK delimiter bit 
     * generation, and the controller will be in the ACK state.*/
    processedLastBit = 1;
    currentStateFunction = stateFunction[BS_ACK];
    delimiterACK = 1; 
}

void stateACK() {
    /* When this function is called : the controller is on the ACK state, and running before the next sample point of ACK delimiter
     * and there where the controller checks for CRC matching, and signal the Error frame if needed. */
    processedLastBit = 1;
    matchCRC = 1;
    /* change state to EOF ...*/
}

void stateEOF() {
    /* This state code might be moved to interruptBitTiming() in sync.c to get the right response time */
    static uint8_t bitCounter = 0;
    processedLastBit = 1;
    if(initializeState) {
        bitCounter = 0;
        initializeState = 0;
    }
    
    if(nxtBit != RECESSIVE) {
        SWITCH_ERROR_MODE(0, 6);
    }
    else
        bitCounter++;
    /* Seven recessive bits */
    if(bitCounter == 7) {
        /* Set next state. */
    }
}

void stateIntermission()        {
    /* to be moved to interruptBitTiming() in sync.c */
    static uint8_t bitCounter = 0;
    processedLastBit = 1;
    if(initializeState) {
        bitCounter = 0;
        initializeState = 0;
    }
    switch(bitCounter)  {
        case 0:
            if(nxtBit == DOMINANT)  {
                /* OVERLOAD FLAG, START TRANSMITTING OVERLOAD FRAME */
            }
            break;
        case 1:
            if(nxtBit == DOMINANT)  {
                /* Error */
            }
            break;
        case 2:
            if(nxtBit == DOMINANT)  {
                /* start of new frame */
            }
            else    {
                /* state transition to either suspend or idle. Here, we only implement the transition to idle,
                 * as 'suspend' is a valid transition only when we're transmitting. */
                currentStateFunction = stateFunction[BS_IDLE];
                initializeState = 1;
            }
            break;
        default:
            break;
    }
}

void stateSuspend()     {
    /* Only in transmission controller mode, and error state is passive */
}

void stateOverload()    {
    
}

void stateOverloadDelimiter()   {
    
}

void stateError()       {
    
}

void stateErrorDelimiter()      {
    
}