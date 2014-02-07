#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"

//prototypes
void initialize();
forwardType forwarded; 
statusType status;
bubbleType bubble;

/* The main driver for the program.  Initializes the registers and function
 * pointer array for the execute stage.  Loads the program into the simulated
 * memory.  Simultes execution of the pipeline by calling the different stages
 * in the appropriate order and passing the required structs.
 */ 
int main(int argc, char * args[])
{
    //Initializes the registers and function pointer array
    initialize();
    initializeFuncPtrArray();

    //loads the program into the simulated memory
    bool loaded = !load(argc, args);

    //If the load was unsuccessfull, dump the memory and exit
    if(!loaded){
        dumpMemory();     
        exit(0);
    }
    
    //clock cycle counter
    int clockCount = 0;
    bool stop = FALSE;

    //simulate execution of the program through the pipeline
    while(!stop){
        stop = writebackStage(&forwarded, &status);
        memoryStage(&status, &forwarded, &bubble);
        executeStage(status, &forwarded, &bubble);
        decodeStage(forwarded, &bubble);
        fetchStage(forwarded, bubble);
        clockCount++;
    }

    printf("\nTotal clock cycles = %d\n", clockCount);

}

/* Function Name: initialize
 * Purpose:       Clear the memory and registers in preparation for
 *                running a new program.
 *
 * Parameters:    -
 * Returns:       -
 * Modifies:      -
 */
void initialize()
{
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}


