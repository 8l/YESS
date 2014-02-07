#include "bool.h"
#include "forwarding.h"
#include "bubbling.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "tools.h"
#include "instructions.h"
#include "registers.h"

//F register holds the input for the fetch stage. 
//It is only accessible from this file. (static)
static fregister F;

//prototypes
bool needs_regids(unsigned int icode);
bool needs_valc(unsigned int icode);
unsigned int getValC(unsigned int f_pc, bool *memError);
unsigned int checkIcode(unsigned int icode);
bool F_stall(bubbleType bubble);
bool D_stall(bubbleType bubble);
bool D_bubble(bubbleType bubble);
unsigned int predictPC(unsigned int f_pc, unsigned int icode);
unsigned int selectPC(forwardType forwarded);


/* Function Name: fetchStage
 * Purpose:       Handles the Fetch stage of the pipelined machine.  Fetches an instruction and
 *                sets initial values for later stages to use.  Also increments the PC.
 * Parameters:    forwarded - struct containing values used for forwarding
 *                bubble - struct containing values used for bubbling/stalling
 * Returns:       None
 * Modifies:      F.predPC
 */
void fetchStage(forwardType forwarded, bubbleType bubble)
{
	//F register only variables
	unsigned int icode;
	unsigned int ifun;
	unsigned int f_stat;
	unsigned int rA = RNONE;
	unsigned int rB = RNONE;
	unsigned int valC = 0;
	unsigned int valP;
	unsigned int opcode;
	unsigned int f_pc;
    bool memError = FALSE;

    //address of next instruction
    f_pc = selectPC(forwarded);

    //grabs the byte-long opcode
    opcode = getByte(f_pc, &memError);

    //gets the function code and instruction code
    ifun = getBits(0, 3, opcode);
    icode = getBits(4, 7, opcode);

    //checks for valid instructions
    f_stat = checkIcode(icode);

    //check for prior memory errors
    if(memError) f_stat = SADR;

    //Only update values in the F register if a valid instruction
    if(f_stat != SINS){
    
        //handles cases where instructions require register IDs or values
        if(needs_regids(icode)){
            //gets the byte containing register identifiers and checks for memory errors
            unsigned int regs = getByte(f_pc+1, &memError);
            if(memError) f_stat = SADR;

            //gets the invidivual register identifiers
            rA = getBits(4,7,regs);
            rB = getBits(0,3,regs);
        }

        //checks if a value is needed for valC, sets valC to the appropriate value if so
        if(needs_valc(icode)){
            switch(icode){
                case IIRMOVL:
                case IRMMOVL:
                case IMRMOVL:
                    valC = getValC(f_pc+2, &memError);
                    break;
                case IDUMP:
                case IJXX:
                case ICALL:
                    valC = getValC((f_pc+1), &memError);
                    break;
                default:
                    valC = getValC((f_pc+1), &memError);
                    break;
            }
            if(memError) f_stat = SADR;
        }
    }

    //checks if the F register should be stalled, if not the appropriate values are updated
    if(!F_stall(bubble)){
        valP = predictPC(f_pc, icode);

        if(icode == IJXX || icode == ICALL) F.predPC = valC;
        else F.predPC = valP;
    }

    //checks if the D register should be stalled or bubbled, and updates the D register as necessary
    if(!D_stall(bubble)){
        if(D_bubble(bubble)) updateDregister(SAOK, INOP, 0, RNONE, RNONE, 0, 0);
        else updateDregister(f_stat, icode, ifun, rA, rB, valC, valP);
    }
}
/* Function Name: getFregister
*  Purpose:       Returns a copy of the F register
*
*  Parameterss:   -
*  Returns:       fregister
*  Modifies:      -
*/
fregister getFregister()
{
    return F;
}

/* Function Name: clearFregister
*  Purpose:       Returns a copy of the F register
*
*  Parameterss:   -
*  Returns:       -
*  Modifies:      F
*/ 
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}

/*  Function Name: selectPC
 *  Purpose:       retrieving the address of the next function
 *
 *  Parameters:    forwarded - struct containing forwarded values
 *  Returns:       The address of the next instruction
 *  Modifies:      -
 */
unsigned int selectPC(forwardType forwarded)
{
    //checks icode in later stages for forwarding
    if(forwarded.M_icode == IJXX && !forwarded.M_Cnd) return forwarded.M_valA;
    else if(forwarded.W_icode == IRET) return forwarded.W_valM;

    //returns F.predPC if no forwarding is needed
    else return F.predPC;
}

/* Function Name: predictPC
 * Purpose:       Predicts the address of the next instruction based on the length of the
 *                current instruction
 *
 * Parameters:    pc - address of the current instruction
 *                icode - the current instruction code
 * Returns:       predicted address of the next instruction
 * Modifies:      -
 */
unsigned int predictPC(unsigned int pc, unsigned int icode){
    //Use switch statement to predict next address based on length of current instruction
    switch(icode){
        case IHALT:
            return pc + 1;
        case INOP:
            return pc + 1;
        case IIRMOVL:
            return pc + 6;
        case IRMMOVL:
            return pc + 6;
        case IMRMOVL:
            return pc + 6;
        case IOPL:
            return pc + 2;
        case IJXX:
            return pc + 5;
        case ICMOVXX:
            return pc + 2;
        case ICALL:
            return pc + 5;
        case IRET:
            return pc + 1;
        case IPUSHL:
            return pc + 2;
        case IPOPL:
            return pc + 2;
        case IDUMP:
            return pc + 5;
        default:
            return pc + 1;
    }
}

/* Function Name: needs_regids
 * Purpose:       Determines if the current instruction requires register IDs
 *
 * Parameters:    icode - the instruction code for the current instruction
 * Returns:       True if register IDs are needed, false otherwise
 * Modifies:      -
 */
bool needs_regids(unsigned int icode){
    switch(icode){
        case IIRMOVL:
        case IRMMOVL:
        case IMRMOVL:
        case IOPL:
        case ICMOVXX:
        case IPUSHL:
        case IPOPL:
            return TRUE;
        default:            //If not one of the above instructions, return FALSE
            return FALSE;
    }
}

/* Function Name: needs_valc
 * Purpose:       Determines if the current instruction requires a value provided by
 *                the instruction.
 *
 * Parameters:    icode - the current instruction code
 * Returns:       True if a value is required, false otherwise
 * Modifies:      -
 */
bool needs_valc(unsigned int icode){
    switch(icode){
        case IIRMOVL:
        case IRMMOVL:
        case IMRMOVL:
        case IJXX:
        case ICALL:
        case IDUMP:
            return TRUE;
        default:            //If not one of the above instructions, return False
            return FALSE;
    }
}

/* Function Name: getValC
 * Purpose:       Retrieves the value contained in the instruction
 *
 * Parameters:    f_pc - the starting address of value to return
 *                memError - a pointer to a boolean that indicates whether a memory error occurred
 * Returns:       the valC value
 * Modifies:      -
 */
unsigned int getValC(unsigned int f_pc, bool * memError){
    //obtains the 4 bytes needed
    unsigned char byte0 = getByte(f_pc, memError);
    unsigned char byte1 = getByte(f_pc+1, memError);
    unsigned char byte2 = getByte(f_pc+2, memError);
    unsigned char byte3 = getByte(f_pc+3, memError);

    //combines the four bytes into an unsigned int and returns
    return buildWord(byte0, byte1, byte2, byte3);
}

/* Function Name: checkIcode
 * Purpose:       Checks for valid instructions and returns the status
 *
 * Parameters:    icode - the current instruction code
 * Returns:       The status as determined by the icode
 * Modifies:      -
 */
unsigned int checkIcode(unsigned int icode){
    switch(icode){
        case INOP:
        case IRRMOVL:
        case IIRMOVL:
        case IRMMOVL:
        case IMRMOVL:
        case IOPL:
        case IJXX:
        case ICALL:
        case IRET:
        case IPUSHL:
        case IPOPL:
        case IDUMP:
            return SAOK;    //SAOK if a valid, non-halt instruction
        case IHALT:
            return SHLT;    //SHLT if instruction is a halt
        default:
            return SINS;    //SINS if the instruction is invalid
    }
}

/* Function Name: F_stall
 * Purpose:       Determines if a stall should be performed on the F register
 *
 * Parameters:    bubble - struct containing values for determining if the register should be stalled
 * Returns:       TRUE if register should be stalled, FALSE otherwise
 * Modifies:      -
 */
bool F_stall(bubbleType bubble){
    //makes code easier to read
    unsigned int E_icode = bubble.E_icode;
    unsigned int E_dstM = bubble.E_dstM;

    //register should be stalled if a RET instruction is anywhere in the pipeline except the writeback stage
    if(bubble.D_icode == IRET || E_icode == IRET || bubble.M_icode == IRET) return TRUE;

    //stall required if a load/data hazard is present
    else if((E_icode == IMRMOVL || E_icode == IPOPL) && (E_dstM == bubble.d_srcA || E_dstM == bubble.d_srcB)) return TRUE;
    return FALSE;
}

/* Function Name: D_stall
 * Purpose:       Determines if the D register should be stalled
 *
 * Parameters:    bubble - struct containing values for determining if the register should be stalled
 * Returns:       TRUE if register should be stalled, FALSE otherwise
 * Modifies:      -
 */
bool D_stall(bubbleType bubble){
    //makes code easier to read
    unsigned int E_icode = bubble.E_icode;
    unsigned int E_dstM = bubble.E_dstM;

    //stall required if a load/data hazard is present
    if((E_icode == IMRMOVL || E_icode == IPOPL) && (E_dstM == bubble.d_srcA || E_dstM == bubble.d_srcB)) return TRUE;
    return FALSE;
}

/* Function Name: D_bubble
 * Purpose:       Determines if the D register should be bubbled
 *
 * Parameters:    bubble - struct containing values for determining if the register should be bubbled
 * Returns:       TRUE if register should be bubbled, false otherwise
 * Modifies:      -
 */
bool D_bubble(bubbleType bubble){
    //makes code easier to read
    unsigned int E_icode = bubble.E_icode;
    unsigned int E_dstM = bubble.E_dstM;

    //checks for a mispredicted branch
    bool misPredBr = (E_icode == IJXX && !bubble.e_Cnd);

    //checks for a RET instruction later in the pipeline
    bool iret  = (bubble.D_icode == IRET || E_icode == IRET || bubble.M_icode == IRET);

    //checks if load/data hazard exists
    bool bub = (!((E_icode == IMRMOVL || E_icode == IPOPL) && (E_dstM == bubble.d_srcA || E_dstM == bubble.d_srcB)) && iret);

    if(misPredBr || bub) return TRUE;
    return FALSE;
}
