#include "error.h"

uint8_t errorCounter;    /* Controller internal error counter. */
uint8_t errorBitCounter; /* How many bits to transmit ? */
uint8_t errorBitValue;   /* Flag : either recessive or dominant.*/

void codingError()      {
    return;
}
