/* 
 * File:   can_api.h
 * Author: yahia
 *
 * Created on April 13, 2013, 11:18 AM
 */

#ifndef CAN_API_H
#define	CAN_API_H

unsigned int Initialize(unsigned int can_mode); /* Should add more control over the clock and frequency */
unsigned int setIdentifier(char* id);
unsigned int transmitDATA(char* data); /* Blocking Version*/
unsigned int receiveDATA(); /* Reads and returns the receiving buffer, and blocks if the buffer is empty */

#endif	/* CAN_API_H */

