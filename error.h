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

#define SWITCH_ERROR_MODE(a,b)\
errorCounter += (b);\
errorBitCounter = (a);\
controllerMode = CM_ERROR;

extern uint8_t errorCounter;    /* Controller internal error counter. */
extern uint8_t errorBitCounter; /* How many bits to transmit ? */
extern uint8_t errorBitValue;   /* Flag : either recessive or dominant.*/

void codingError();

#endif	/* ERROR_H */

