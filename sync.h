/* 
 * File:   sync.h
 * Author: yahia
 *
 * Created on April 9, 2013, 10:27 AM
 */

/* How does the clock work in both receiving and transmitting modes ?
 * 
 * Receiving : 
 * The Timer and Capture/Match unit are utilized to get the bit's segments right.
 * Synchronization and Resynchronization are timed by interrupt OnChange.
 * 
 * Transmitting :
 * 
 */

#ifndef SYNC_H
#define	SYNC_H

#include <sys/types.h>
#include <stdint.h>

#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) < (b) ? (a) : (b)

/* NOTE : Here, we merged the SYNC_SEG with PROP_SEG into one segment, this should work ok as both 
 * segments are of constant durations.we can simply check for SYNC_SEG by examining the timer.
 * This should work for most hardware configurations, unless reading the timer needs special of cost relatively high
 * CPU cycles, in this case we can separate these states back. */

#define SYNC_PROP_SEG        0
#define PHS1_SEG        1
#define PHS2_SEG        2

/* Length in (Time Quantum) units. Total length should be between 8 - 25 Time Quanta */


#define SYNC_PROP_LN    2
#define PHS1_LN         4
#define PHS2_LN         4

/* Length of (Time Quantum) in clock cycles */
#define TIME_QUANTUM    10

typedef struct   {
    uint8_t segmentLengths[3];
    uint8_t jumpWidth;
    int8_t relativeJumpWidth; 

    uint8_t nxtBit;                  /* Used to store next bit received from the bus */
    uint8_t lstBit;                  /* Used for stuffing check */
    uint8_t bitRepetitionCount;      /* Used to count the number of repetition of last bit */

    uint8_t processedLastBit;        /* Flag : used to indicate weather a state function needs to be called.*/
} globalSync_t;

extern globalSync_t globalSync;

void interruptBitTiming();
void interruptOnChange();



#endif	/* SYNC_H */

