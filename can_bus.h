/* 
 * File:   can_bus.h
 * Author: yahia
 *
 * Created on April 9, 2013, 1:58 PM
 */

#ifndef CAN_BUS_H
#define	CAN_BUS_H

#include <stdint.h>

#define DOMINANT        0
#define RECESSIVE       1

/* List of Bus States */
#define NUM_STATES 14

#define BS_IDLE                 0
#define BS_SOF                  1
#define BS_ARBITRATION_CONTROL  2
#define BS_CONTROL              3
#define BS_DATA                 4
#define BS_CRC                  5
#define BS_ACK                  6
#define BS_EOF                  7
#define BS_INTERMISSION         8
#define BS_SUSPEND              9
#define BS_OVERLOAD             10
#define BS_OVERLOAD_DELIMITER   11
#define BS_ERROR                12
#define BS_ERROR_DELIMITER      13

#endif	/* CAN_BUS_H */

