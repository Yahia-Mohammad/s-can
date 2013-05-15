#include "sync.h"
#include "hal.h"
#include "can_bus.h"
#include "controller.h"
#include "crc.h"
#include "error.h"

globalSync_t globalSync = {
.jumpWidth = MIN(4,PHS1_LN),
.relativeJumpWidth = 0,
.segmentLengths  = {SYNC_PROP_LN, PHS1_LN, PHS2_LN},
.processedLastBit = 1,
.lstBit = RECESSIVE,
.bitRepetitionCount = 0,        
.nxtBit = RECESSIVE        
};


static uint8_t bitState = 0; 
#if 0
/* In which bit time segment the bus is */
uint8_t jumpWidth = MIN(4,PHS1_LN);      /* Synchronization Jump Width */
int8_t relativeJumpWidth; // = jumpWidth - SYNC_PROP_LN;

/*we might need to multiply by TIME_QUANTUM*/


uint8_t segmentLengths[3] = {SYNC_PROP_LN, PHS1_LN, PHS2_LN};
uint8_t processedLastBit = 1;
uint8_t lstBit = RECESSIVE;
uint8_t bitRepetitionCount = 0;
uint8_t nxtBit = RECESSIVE;
#endif

/*Function: void interruptBitTiming
 This function gets called when the timer reaches the value in the capture (or match) register.*/
void interruptBitTiming()       {
    
    if(bitState == PHS2_SEG) 
        bitState = SYNC_PROP_SEG;
    else 
        bitState++;
    
    updateCapture(0);
    
    /*********************************/
    /*** Start of Segment PHS2_SEG ***/
    /*********************************/
    
    /*
     * We update the state and the timer as soon as possible so that we keep synchronized 
     * as close as possible. NOTE : The following condition should have been (bitState == PHS1_SEG) if
     * the state update would be delayed after it.
     */
    
    if(bitState == PHS2_SEG)    {
        getSample();
        globalSync.processedLastBit = 0;
        /*====================*/
        /*== Receiving Mode ==*/
        /*====================*/
        if(globalController.controllerMode == CM_RECEIVE)        {
            /* Note : This code is written and optimized for ARM7 CPUs, that perform equally well for both 32 bit and 8 bit. So,
             * we don't have any problems checking 32-bit address pointers, otherwise, we could have used 8-bit indeces. */
 
            /* VERY IMPORTANT : DELIMITER CRC SHOULD BE EXCLUDED FROM BIT STUFFING */
            if((currentStateFunction == stateFunction[BS_ARBITRATION_CONTROL]) || (currentStateFunction == stateFunction[BS_DATA]) || (currentStateFunction == stateFunction[BS_CRC]))
            {
                if(globalSync.bitRepetitionCount != 0)
                {
                    if(globalSync.lstBit == globalSync.nxtBit)   
                            globalSync.bitRepetitionCount++;
                    else
                        if(globalSync.bitRepetitionCount == 5)     {
                            globalSync.bitRepetitionCount = 0;
                            return; /* check if it should be replaced with code to neglect this bit */
                        }    
                        else
                            globalSync.bitRepetitionCount = 1;

                    if(globalSync.bitRepetitionCount == 6)
                        SWITCH_ERROR_MODE(CM_RECEIVE); /*Value 0 should be replaced*/
                        return; /* Error Detected : will be expanded later ...*/
                }

                globalSync.lstBit = globalSync.nxtBit;
                globalSync.bitRepetitionCount++; /* double check this */
    #if 0    /* We may need to put it here */
                if(currentStateFunction != stateFunction[BS_CRC])   {
                    /* update the CRC sequence */
                }
    #endif

            } /*** End of stuffing update/checking ***/

            if(globalController.matchCRC)       {
                UPDATE_REGISTER(receivedCRC, globalSync.nxtBit);
                localCRC &= ~(1 << 15);
                if(localCRC != receivedCRC) {
                    /* Error*/
                    SWITCH_ERROR_MODE(CM_RECEIVE);
                }
                globalController.matchCRC = 0;
                globalController.delimiterCRC = 1;
                return;
            }

            if(globalController.delimiterCRC)    {/* The ACK flag should be set here*/
                    /* check for recessive bit in nxtBit, otherwise : Error */
                if(globalSync.nxtBit != RECESSIVE)     {
                    SWITCH_ERROR_MODE(CM_RECEIVE); 
                }
                globalController.delimiterCRC = 0;
                globalController.generateACK = 1;
                return;
            }
            if(globalController.delimiterACK)    {
                if(globalSync.nxtBit != RECESSIVE) {
                    SWITCH_ERROR_MODE(CM_RECEIVE); 
                }
                globalController.delimiterACK = 0;
                return;
            }
        } /* controllerMode == CM_RECEIVE */
        
        /*************************/
        /*****  Error Mode   *****/
        /*************************/
        
        if(globalController.controllerMode == CM_ERROR)  {
            if(globalError.bitCounter == 0) {
                if(globalError.busSubState == BS_SUB_ERROR) {
                    if(globalSync.nxtBit == RECESSIVE) {
                        globalError.busSubState = BS_SUB_ERROR_DELIMITER;
                        globalError.bitCounter = 8;
                    }
                }
                else    {
                    /* globalError.busSubState == BS_SUB_ERROR_DELIMITER */
                    
                    /*||||| update error counter, and error state **/
                    /* The following needs to be expanded to implement other error checks */
                    if(globalError.lastControllerState == CM_RECEIVE)   
                        globalError.receiveCounter++;
                    else
                        globalError.transmitCounter++;
                    
                    /* Transition to next state */
                    globalController.controllerMode = CM_RECEIVE;
                    currentStateFunction = stateFunction[BS_INTERMISSION];
                    globalController.initializeState = 1;
                }
            } /* global.Error.bitCounter == 0 */
        } /* controllerMode == CM_ERROR */
    } /* bitState == PHS2_SEG */
    
    /******************************/
    /*** Start of SYNC_PROP_SEG ***/
    /******************************/
       
    if(bitState == SYNC_PROP_SEG)   {
        
        uint8_t outputBit = RECESSIVE;
        /*==================*/
        /*== RECEIVE MODE ==*/
        /*==================*/
            
        if(globalController.controllerMode == CM_RECEIVE)    {
            if(globalController.generateACK == 0)    {
                putBit(DOMINANT);
                globalController.generateACK = 0;
            }
        }
            
            
        /*================*/
        /*== Error Mode ==*/
        /*================*/
            
        if(globalController.controllerMode == CM_ERROR)  {
            if(globalError.bitCounter) {
                outputBit = (globalError.busSubState) ? globalError.errorState : RECESSIVE;
                globalError.bitCounter--;
            }
        }
        
        /* Next, update the bus state */
        putBit(outputBit);
    } /* bitState == SYNC_PROP_SEG */
}

void interruptOnChange()        {
    
    switch(bitState)    {
        if(currentStateFunction == stateFunction[BS_IDLE])  {
            /* Hard Synchronization */
            bitState = SYNC_PROP_SEG;
            updateCapture(0);
            resetTimer();
        }
        else    
            switch(bitState)    {
                case SYNC_PROP_SEG:
                    if(TIMER <= globalSync.jumpWidth)  /* in SYNC_SEG */   {
                        updateCapture(0);
                        resetTimer();
                    }
                break;
            
                case PHS1_SEG:
                    if(globalSync.relativeJumpWidth > 0)
                        if(TIMER < globalSync.relativeJumpWidth)   {
                            /* Same as Hard Synchronization */
                            bitState = SYNC_PROP_SEG;
                            updateCapture(0);
                            resetTimer();
                        }
                        else    
                            updateCapture(globalSync.jumpWidth);
                break;
            
                case PHS2_SEG:
                    updateCapture((int8_t)(-globalSync.jumpWidth));
                break;
                
                default:
                /* redundant */
                break;
            }
            
        

    }
}