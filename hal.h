/* 
 * File:   uc.h
 * Author: yahia
 *
 * Created on April 9, 2013, 11:05 AM
 */

#ifndef HAL_H
#define	HAL_H

#include <stdint.h>


void initializeUC();

void getSample();

void putBit(uint8_t bit);

void updateTimer();     /* Are used to set the timer capture time. */

uint8_t getTimer();        /* Could use volatile variable or a macro instead */

void resetTimer();      /* zero the timer, and start over. */

#define TIMER (getTimer())

#endif	/* UC_H */

