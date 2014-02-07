#include <stdio.h>
#include "bool.h"
#include "forwarding.h"
#include "status.h"
#include "writebackStage.h"
#include "tools.h"
#include "instructions.h"
#include "dump.h"
#include "registers.h"

//W register holds the input for the writeback stage
//It is only accessible from this file. (static)
static wregister W;

/* Function Name: writebackStage
 * Purpose:       To simulate the writeback stage in a pipeline machine
 *
 * Parameters:    forward - struct containing values used for forwarding
 *                status - struct containing values for the status of the memory and writeback stages
 * Returns:       TRUE if status is anything other than SAOK, FALSE otherwise
 * Modifies:      Appropriate values in forward and status structs
 */
bool writebackStage(forwardType *forward, statusType *status){

    //set appropriate forwarding values
    forward->W_valE = W.valE;
    forward->W_dstE = W.dstE;
    forward->W_dstM = W.dstM;
    forward->W_valM = W.valM;
    forward->W_icode = W.icode;

    //set appropriate status value
    status->W_stat = W.stat;

    //check if instruction is a dump
    if(W.icode == IDUMP){
        if(W.valE & 0x1) dumpProgramRegisters();
        if(W.valE & 0x2) dumpProcessorRegisters();
        if(W.valE & 0x4) dumpMemory();
    }

    //only update registers if status is SAOK
    if(W.stat == SAOK){
    setRegister(W.dstE, W.valE);
    setRegister(W.dstM, W.valM);
    }
    
    //determines next operation based on status
    switch(W.stat){
        case SAOK:
            return FALSE; //continue program operation
        case SHLT:
            return TRUE; //program terminated due to halt
        case SADR:
            printf("Invalid memory address\n"); 
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
            return TRUE; //invalid memory address, dump everything and terminate
        case SINS:
            printf("Invalid instruction\n");
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
            return TRUE; //invalid instruction, dump everything and terminate
    }
}

/* Function Name: getWregister
 * Purpose:       Returns a copy of the W register
 *
 * Parameters:    -
 * Returns:       wregister
 * Modifies:      -
 */
wregister getWregister(){
    return W;
}

/* Function Name: clearWregister
 * Purpose:       Clears the W register
 * 
 * Parameters:    -
 * Returns:       -
 * Modifies:      W
 */
void clearWregister(){
    clearBuffer((char *) &W, sizeof(W));
    W.stat = SAOK;
    W.icode = INOP;
}

/* Function Name: updateWregister
 * Purpose:       Passes the values from the m stage to the W register
 *
 * Parameters:    All the values that correspond to the W register fields
 * Returns:       -
 * Modifies:      W register
 */
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE,
                    unsigned int valM, unsigned int dstE, unsigned int dstM){
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}
