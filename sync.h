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
/*#include <inttypes.h>*/
/*#include <sys/types.h>*/
#include <stdint.h>

#define SYNC_SEG        0
#define PROP_SEG        1
#define PHS1_SEG        2
#define PHS2_SEG        3

/* Length in (Time Quantum) units. Total length should be between 8 - 25 Time Quanta */
#define SYNC_LN         1
#define PROP_LN         1
#define PHS1_LN         4
#define PHS2_LN         4

/* Length of (Time Quantum) in clock cycles */
#define TIME_QUANTUM    10

/* Used to match codes that require stuffing : 0b11111 (five ones) or 0b00000 (five zeros) */
#define ONES ((uint8_t)0b00011111)
#define ZEROS ((uint8_t)0b00000000)
/* Initial Code : helps detects matching without confusing with initial condition */
#define INITIAL_CODE ((uint8_t)0b00001000)

extern uint8_t segmentLenghts[4];
extern uint8_t nxtBit;                    /* Used to store next bit received from the bus */
extern uint8_t stuffingRegister;          /* Used to store bits to check for the code stuffing requirements */
extern uint8_t processedLastBit;          /* A flag used to indicate wether a state function needs to be called.*/

void interruptBitTiming();
void interruptOnChange();



#endif	/* SYNC_H */

