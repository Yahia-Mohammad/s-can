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

/* List of Controller Modes of Operation */

#define CM_RECEIVE          0
#define CM_TRANSMIT         1
#define CM_ERROR            2
#define CM_OVERLOAD         3
#define CM_OFFLINE          4

#define FILTERS_NUM             1
#define MAX_FILTERS_NUM         16

#define INCOMING_BUFFER_SIZE    8

#define UPDATE_REGISTER(a,b) \
(a)<<=1; \
(a)+=(b);

typedef struct  {  
    uint8_t dataLength; /* Expected size of incoming data in bytes */
    uint8_t RTR; /* Flag : Remote Transmission Request */
    uint8_t matchedFilterIndex; /* The index of the nodeFilters entry that matches the incoming identifier */

/* intializeState : Used to indicate whether a function internal variables needs to be initialized or not. 
 * In our implementation, functions use static variables to keep track of their progress for every bit. However, There's a state
 * transition, the next state needs to initialize its static variables, and here is where we set this flag.  */
    uint8_t initializeState; /* Flag : used to mark the first run of a state function. */ 

    uint8_t generateACK;     /* Flag : used to generate ACK bit the very next bit. */
    uint8_t matchCRC;        /* Flag : used to complete the computation of CRC sequence, and compare it.*/
    uint8_t delimiterCRC;    /* Flag : used to check for recessive bit in CRC delimiter field, otherwise : Error */
    uint8_t delimiterACK;    /* Flag : used to check for recessive bit in ACK delimiter field, otherwise : Error */
    uint8_t controllerMode;  /* Controller mode of operation : (Receive, Transmit, Error, Offline,...). */
} globalController_t;

extern globalController_t globalController;

extern uint32_t nodeFilters [FILTERS_NUM];      /* Filters */
extern uint8_t incomingBuffer[INCOMING_BUFFER_SIZE];    /* Receiving Filter*/

extern void (*stateFunction[NUM_STATES])();
extern void (*currentStateFunction)();

void stateIdle();
void stateArbitrationControl();
void stateData();
void stateCRC();   
void stateCRC_Delimiter();
void stateACK();            
void stateEOF();
void stateIntermission();
void stateSuspend();

#if 0
void stateOverload();
void stateOverloadDelimiter();
void stateError();
void stateErrorDelimiter();
#endif


void Initialize();

#endif	/* CONTROLLER_H */

