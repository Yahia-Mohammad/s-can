/* 
 * File:   main.c
 * Author: yahia
 *
 * Created on April 8, 2013, 9:43 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "controller.h"
/*
 * 
 */
int main(int argc, char** argv) {

    while(1)    {
        switch(controllerMode)  {
            case CM_RECEIVE:
                (*currentStateFunction)();
                break;
            case CM_TRANSMIT:
                break;
            case CM_ERROR:
                break;
            case CM_OVERLOAD:
                break;
            case CM_OFFLINE:
                break;
        }
    }
    
    return (EXIT_SUCCESS);
}

