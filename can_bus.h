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
#define NUM_STATES 9

#define BS_IDLE                 0

#define BS_ARBITRATION_CONTROL  1

#define BS_DATA                 2
#define BS_CRC                  3
/* we need to re-number the states and create a function for CRC delimiter */
#define BS_CRC_DELIMITER        4

#define BS_ACK                  5
#define BS_EOF                  6
#define BS_INTERMISSION         7
#define BS_SUSPEND              8

#if 0
#define BS_OVERLOAD             10
#define BS_OVERLOAD_DELIMITER   11
#define BS_ERROR                12
#define BS_ERROR_DELIMITER      13
#endif

#endif	/* CAN_BUS_H */

