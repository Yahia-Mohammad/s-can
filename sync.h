/* 
 * File:   sync.h
 * Author: yahia
 *
 * Created on April 9, 2013, 10:27 AM
 */

/* How does the clock work in both receiving and transmitting modes ?
 * 
 * Receving : 
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

/* NOTE : Here, we merged the SYNC_SEG with PROP_SEG into one segment, this should work ok as both 
 * segments are of constant durations.we can simply check for SYNC_SEG by examining the timer.
 * This should work for most hardware configurations, unless reading the timer needs special of cost relatively high
 * CPU cycles, in this case we can separate these states back. */

/*#define SYNC_SEG        0*/
#define SYNC_PROP_SEG        0
#define PHS1_SEG        1
#define PHS2_SEG        2

/* Length in (Time Quantum) units. Total length should be between 8 - 25 Time Quanta */

/*#define SYNC_LN         1*/
#define SYNC_PROP_LN         2
#define PHS1_LN         4
#define PHS2_LN         4

/* Length of (Time Quantum) in clock cycles */
#define TIME_QUANTUM    10

extern uint8_t segmentLenghts[3];
extern uint8_t nxtBit;                  /* Used to store next bit received from the bus */
extern uint8_t lstBit;                  /* Used for stuffing check */
extern uint8_t bitRepetitionCount;      /* Used to count the number of repetition of last bit */

extern uint8_t processedLastBit;        /* A flag used to indicate weather a state function needs to be called.*/

void interruptBitTiming();
void interruptOnChange();



#endif	/* SYNC_H */

