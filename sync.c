#include "sync.h"
#include "uc.h"
#include "can_bus.h"
#include "controller.h"
#include "crc.h"
#include "error.h"

static uint8_t bitState = 0;
/*we might need to multiply by TIME_QUANTUM*/
uint8_t segmentLengths[3] = {SYNC_PROP_LN, PHS1_LN, PHS2_LN};
uint8_t processedLastBit = 1;
uint8_t lstBit = RECESSIVE;
uint8_t bitRepetitionCount = 0;


/*Function: void interruptBitTiming
 This function gets called when the timer reaches the value in the capture (or match) register.*/
void interruptBitTiming()       {
    
    if(bitState == PHS2_SEG) 
        bitState = SYNC_PROP_SEG;
    else 
        bitState++;
    updateTimer();
    
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
        processedLastBit = 0;
        /*====================*/
        /*== Receiving Mode ==*/
        /*====================*/
        if(controllerMode == CM_RECEIVE)        {
            /* Note : This code is written and optimized for ARM7 CPUs, that perform equally well for both 32 bit and 8 bit. So,
             * we don't have any problems checking 32-bit address pointers, otherwise, we could have used 8-bit indeces. */
 
            /* VERY IMPORTANT : DELIMITER CRC SHOULD BE EXCLUDED FROM BIT STUFFING */
            if((currentStateFunction == stateFunction[BS_ARBITRATION_CONTROL]) || (currentStateFunction == stateFunction[BS_DATA]) || (currentStateFunction == stateFunction[BS_CRC]))
            {
                if(bitRepetitionCount != 0)
                {
                    if(lstBit == nxtBit)   
                            bitRepetitionCount++;
                    else
                        if(bitRepetitionCount == 5)     {
                            bitRepetitionCount = 0;
                            return; /* check if it should be replaced with code to neglect this bit */
                        }    
                        else
                            bitRepetitionCount = 1;

                    if(bitRepetitionCount == 6)
                        SWITCH_ERROR_MODE(6,0); /*Value 0 should be replaced*/
                        return; /* Error Detected : will be expanded later ...*/
                }

                lstBit = nxtBit;

    #if 0    /* We may need to put it here */
                if(currentStateFunction != stateFunction[BS_CRC])   {
                    /* update the CRC sequence */
                }
    #endif

            }

            if(matchCRC)       {/* This is WRONG: CRC mismatch doesn't signal error after crc delimiter, rather, after the ACK delimiter.*/
                UPDATE_REGISTER(receivedCRC, nxtBit);
                localCRC &= ~(1 << 15);
                if(localCRC != receivedCRC) {
                    /* Error*/
                    SWITCH_ERROR_MODE(6,0);/* Value 0 will be replaced */
                }
                matchCRC = 0;
                delimiterCRC = 1;
                return;
            }

            if(delimiterCRC)    {/* The ACK flag should be set here*/
                    /* check for recessive bit in nxtBit, otherwise : Error */
                if(nxtBit != RECESSIVE)     {
                    SWITCH_ERROR_MODE(6,0); /* Value 0 will be replaced */
                }
                delimiterCRC = 0;
                generateACK = 1;
                return;
            }
            if(delimiterACK)    {
                if(nxtBit != RECESSIVE) {
                    SWITCH_ERROR_MODE(6,0); /* Value 0 will be replaced */
                }
                delimiterACK = 0;
            }
        }
    }
    
    /******************************/
    /*** Start of SYNC_PROP_SEG ***/
    /******************************/
    
    if(bitState == SYNC_PROP_SEG)   {
            
        /*==================*/
        /*== RECEIVE MODE ==*/
        /*==================*/
            
        if(controllerMode == CM_RECEIVE)    {
            if(generateACK == 0)
                putBit(RECESSIVE);
            else    {
                putBit(DOMINANT);
                generateACK = 0;
            }
        }
            
            
        /*================*/
        /*== Error Mode ==*/
        /*================*/
            
        if(controllerMode == CM_ERROR)  {
            if(errorBitCounter) {
                putBit(errorBitValue);
                errorBitCounter--;
            }
            else
                putBit(RECESSIVE);
        }
    }
}

void interruptOnChange()        {
    /*
     1)if in SYNC_SEG or PROP_SEG := reset counter
     2)if in PHS1_SEG :=increase PHS1 length.
     3)if in PHS2_SEG :=decrease PHS2 length.
     */
}