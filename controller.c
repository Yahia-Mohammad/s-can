#include "controller.h"
#include "can_bus.h"
#include "sync.h"
#include "error.h"

void (*stateFunction[NUM_STATES])() = {stateIdle, stateSOF,
stateArbitration, stateControl, stateData, stateCRC,
stateACK, stateEOF, stateIntermission, stateSuspend,
stateOverload, stateOverloadDelimiter, stateError, stateErrorDelimiter};

uint8_t nxtBit = RECESSIVE;
uint8_t stuffingRegister = 0;

uint8_t incomingBuffer[INCOMING_BUFFER_SIZE] = {0};
uint32_t nodeFilters [FILTERS_NUM] = {0};
uint8_t dataLength = 0;
uint8_t RTR = 0;
uint8_t matchedFilterIndex = MAX_FILTERS_NUM;

/* IMPORTANT : Should be initialized to  stateFunction[BS_IDLE]*/
void (*currentStateFunction)() = 0;

void stateIdle()        {
    if(nxtBit == DOMINANT)
      currentStateFunction = stateFunction[BS_ARBITRATION];  
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
    uint8_t matched = 0;
    uint8_t i; /* Used as an iterator, a precaution in case the compiler used only support ISO C */
    
    /* First : Checking for stuffing condition */
#if 0
    if(stuffingRegister == ZEROS)       {
        if(nxtBit == DOMINANT)
            codingError(); /* VERY IMPORTANT : WE ALSO NEED TO INITIALIZE VARIABLES FOR NEXT RUN, INCLUDING stuffingRegister */
        else    {
            stuffingRegister = INITIAL_CODE;
            UPDATE_REGISTER(stuffingRegister, nxtBit);
        }
        /* VERY IMPORTANT : BEFORE RETURNING, WE NEED TO SET NEXT STATE */
        /* OR ADD IT IN THE END OF ERROR FUNCTIONS */
        return;
    }
    
    if(stuffingRegister == ONES)        {
        if(nxtBit == RECESSIVE)
            codingError(); /* VERY IMPORTANT : WE ALSO NEED TO INITIALIZR VARIABLES FOR NEXT RUN */
        else    {
            stuffingRegister = INITIAL_CODE;
            UPDATE_REGISTER(stuffingRegister, nxtBit);
        }
        /* VERY IMPORTANT : AS ABOVE */
        return;
    }
#endif
    
    CHECK_STUFFING();
    
    /* we should check for other errors as well */
    
    /* Increase the bit counter */
    bitCounter++;
    
    /* place holder : update CRC sequence here */
    
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
        
        /* All the clean up goes here, all static and related global variables, then set the next state */
        /* WE DO NOT CLEAR stuffingRegister here */
        /* When setting the next state, we might set it to CRC state directly if it's 0 data length (like RTR) */
    }
    
}

void stateControl()     {
    /* Redundant : should be removed */
}

void stateData()        {
    /*
     * The trick here is to assign a value that can not be a valid value, so that we can distinguish between three conditions : 
     * 1) The initial state : where we have non-zero, and non-valid values.
     * 2) The final state : where we have zero values.
     * 3) Valid data bit state : where we have non-zero, but valid states. 
     * ********************************************************************
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
    static uint8_t bitCounter = 10;  /* Non-Zero initial condition */
    static uint8_t byteCounter = 10; /* Non-Zero initial condition */
    
    CHECK_STUFFING();
    
    if(bitCounter == 10)        { 
        /* Checking for Initial Condition.
         * Checking only one of the counters is sufficient */
        bitCounter = 8;
        byteCounter = dataLength - 1;
    }
    
    if(bitCounter == 0) {
        byteCounter--;
        bitCounter = 7;
    }
    else
        bitCounter--;
    
    UPDATE_REGISTER(incomingBuffer[byteCounter], nxtBit);
    /* we should update the CRC sequence here as well */
    
    if((bitCounter == 0) && (byteCounter == 0)) {
        /* do the clean up and prepare for the next state */
        /* clear corresponding global and static variables, clear buffers and set next state */
    }
    
}

void stateCRC() {
    
}

void stateACK() {
    
}

void stateEOF() {
    
}

void stateIntermission()        {
    
}

void stateSuspend()     {
    
}

void stateOverload()    {
    
}

void stateOverloadDelimiter()   {
    
}

void stateError()       {
    
}

void stateErrorDelimiter()      {
    
}