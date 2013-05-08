/* 
 * File:   crc.h
 * Author: yahia
 *
 * Created on April 30, 2013, 4:27 PM
 */

#ifndef CRC_H
#define	CRC_H

#include <stdint.h>
#define CRC_MSB ((uint16_t) (0x1 << 14))

extern uint16_t localCRC, receivedCRC; 

#define UPDATE_CRC(a,b)\
if(((a)&CRC_MSB)^((b) << 14)) {\
    (a)<<=1;\
    (a)^=(uint16_t)0x4599;\
    }\
else\
    (a)<<=1;

#endif	/* CRC_H */

