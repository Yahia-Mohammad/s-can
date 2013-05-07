/* 
 * File:   uc.h
 * Author: yahia
 *
 * Created on April 9, 2013, 11:05 AM
 */

#ifndef UC_H
#define	UC_H

#include <stdint.h>

void initializeUC();

void getSample();

void putBit(uint8_t bit);

void updateTimer();     /* Are used to set the timer capture time. */


#endif	/* UC_H */

