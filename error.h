/* 
 * File:   error.h
 * Author: yahia
 *
 * Created on April 9, 2013, 2:14 PM
 */

#ifndef ERROR_H
#define	ERROR_H

#include <stdint.h>
/* List of Error States */

#define ES_ACTIVE       0
#define ES_PASSIVE      1

#define BS_SUB_ERROR            0
#define BS_SUB_ERROR_DELIMITER  1

typedef struct  {
    uint16_t transmitCounter;   /* Controller internal error counter. */
    uint16_t receiveCounter;    /* Controller internal receive error counter. */
    uint8_t bitCounter;         /* How many bits to transmit ? */
    uint8_t errorState;         /* Flag : either recessive or dominant.*/
    uint8_t lastControllerState;/* Either Transmit or Receive. */
    uint8_t busSubState;        /* Either Error (or) Error Delimiter */
} globalError_t;

extern globalError_t globalError;

#define SWITCH_ERROR_MODE(a)\
globalError.lastControllerState = (a);\
globalError.bitCounter = 6;\
controllerMode = CM_ERROR;

#define DECREASE_RECEIVE_ERROR_COUNTER      globalError.receiveCounter--
#define DECREASE_TRANSMIT_ERROR_COUNTER     globalError.transmitCounter--

#endif	/* ERROR_H */

