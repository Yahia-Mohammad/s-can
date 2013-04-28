/* 
 * File:   controller.h
 * Author: yahia
 *
 * Created on April 9, 2013, 2:11 PM
 */

#ifndef CONTROLLER_H
#define	CONTROLLER_H

#include <stdint.h>
#include "can_bus.h"
#define CAN_STANDARD 0
#define CAN_EXTENDED 1

#define TRASNMIT_BUFFER_SIZE    8
#define RECEIVE_BUFFER_SIZE     8

/* List of Controller States */

#define CS_RECEIVING            0
#define CS_TRANSMITTING         1

#define FILTERS_NUM             1
#define MAX_FILTERS_NUM         16

#define INCOMING_BUFFER_SIZE    8

#define UPDATE_REGISTER(a,b) \
(a)<<=1; \
(a)+=(b);

#if 1
#define CHECK_STUFFING()\
if(stuffingRegister == ZEROS)       {\
        if(nxtBit == DOMINANT)\
            codingError();  /* VERY IMPORTANT : WE ALSO NEED TO INITIALIZE VARIABLES FOR NEXT RUN, INCLUDING stuffingRegister */ \
        else    {\
            stuffingRegister = INITIAL_CODE; \
            UPDATE_REGISTER(stuffingRegister, nxtBit); \
        }\
        /* VERY IMPORTANT : BEFORE RETURNING, WE NEED TO SET NEXT STATE */ \
        /* OR ADD IT IN THE END OF ERROR FUNCTIONS */ \
        return;\
    }\
    \
    if(stuffingRegister == ONES)        {\
        if(nxtBit == RECESSIVE)\
            codingError(); /* VERY IMPORTANT : WE ALSO NEED TO INITIALIZR VARIABLES FOR NEXT RUN */ \
        else    {\
            stuffingRegister = INITIAL_CODE;\
            UPDATE_REGISTER(stuffingRegister, nxtBit);\
        }\
        /* VERY IMPORTANT : AS ABOVE */ \
        return;\
    }
#endif

extern uint32_t nodeFilters [FILTERS_NUM];      /* Filters */
extern uint8_t incomingBuffer[INCOMING_BUFFER_SIZE];    /* Receiving Filter*/
extern uint8_t dataLength; /* Expected size of incoming data in bytes */
extern uint8_t RTR; /* Remote Transmission Request */
extern uint8_t matchedFilterIndex; /* The index of the nodeFilters entry that mantches the incoming identifier */

extern void (*stateFunction[NUM_STATES])();
extern void (*currentStateFunction)();

void stateIdle();
void stateSOF();
void stateArbitration();
void stateControl();
void stateData();
void stateCRC();
void stateACK();
void stateEOF();
void stateIntermission();
void stateSuspend();
void stateOverload();
void stateOverloadDelimiter();
void stateError();
void stateErrorDelimiter();


#endif	/* CONTROLLER_H */

