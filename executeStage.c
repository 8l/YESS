#include "bool.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "executeStage.h"
#include "tools.h"
#include "instructions.h"
#include "memoryStage.h"
#include "registers.h"

//prototypes
bool calcECnd();
bool M_bubble(statusType status);
bool calcMCnd();
bool set_cc(statusType status);

//prototypes for functions involving the function pointer array
unsigned int (*functions[16])(bool status);
unsigned int performOpl(bool status);
unsigned int performIrmovl(bool status);
unsigned int performCmovl(bool status);
unsigned int performRmmovl(bool status);
unsigned int performMrmovl(bool status);
unsigned int performPushl(bool status);
unsigned int performPopl(bool status);
unsigned int seteDstE(bool e_cnd);
unsigned int performCall(bool);
unsigned int performRet(bool status);
unsigned int returnZero(bool status);
unsigned int dump(bool status);

/*-----------------NOTE-----------------------
 * If the execute stage doesn't do anything
 * with an instruction, there isn't a function
 * for that instruction.  Those indices in the
 * function pointer array point to the 
 * returnZero function.
 * ------------------------------------------*/



//E register holds the input for the execute stage
//It is only accessible from this file. (static)
static eregister E;

/* Function Name: executeStage
 * Purpose:       performs execution of instruction
 *                and detects mispredicted branches
 *
 * Parameters:    status    - status of memory stage and writeback register
 *                forwarded - pointer to a struct used for forwarding
 *                bubble    - pointer to a struct used for bubbling/stalling
 * Returns:       -
 * Modifies:      M Register
 *                forwarded->e_valE
 *                forwarded->e_dstE
 *                bubble->e_Cnd
 *                bubble->E_icode
 *                bubble->E_dstM
 */
void executeStage(statusType status, forwardType *forwarded, bubbleType *bubble)
{
    //determines whether the condition codes should be set or not
    bool setcc = set_cc(status);

    //calculates the condition code used by the execute stage that determines whether or not
    //a conditional move should be taken
    bool e_cnd = calcECnd();

    //calculates the value of e_valE based on the icode
    unsigned int e_valE = (*functions[E.icode])(setcc);

    //calculates the e_dstE register
    unsigned int e_dstE = seteDstE(e_cnd);

    //update forwarded struct
    forwarded->e_valE = e_valE;
    forwarded->e_dstE = e_dstE;

    //calculates the condition code used by the memory stage that determines whether or not
    //a conditional jump should be taken
    bool M_cnd = calcMCnd();

    //update bubble struct
    bubble->e_Cnd = M_cnd;
    bubble->E_icode = E.icode;
    bubble->E_dstM = E.dstM;
    
    //check if bubbling is needed and update the M register accordingly
    if(M_bubble(status)) updateMregister(SAOK, INOP, 0, 0, 0, RNONE, RNONE);
    else updateMregister(E.stat, E.icode, M_cnd, e_valE, E.valA, e_dstE, E.dstM);
}

/* Function Name: set_cc
 * Purpose:       Determines if an IOPL instruction should set condition codes
 *
 * Parameters:    status - status of memory stage and writeback stage
 * Returns:       TRUE if the instruction is an OPL and the status in the memory and writeback
 *                stages are SAOK
 * Modifies:      -
 */
bool set_cc(statusType status){
    if(E.icode == IOPL && status.m_stat == SAOK && status.W_stat == SAOK) return TRUE;
    else return FALSE;
}

/* Function Name: calcMCnd
 * Purpose:       sets the conditional flags
 *
 * Parameters:    -
 * Returns:       whether or not any of the condition codes
 * 				  were set to true
 * Modifies:      -
 */
bool calcMCnd(){
    if(E.icode != IJXX) return 0;

    //grab the condition codes
    unsigned int oF = getCC(OF);
    unsigned int sF = getCC(SF);
    unsigned int zF = getCC(ZF);

    //determine the kind of jump
    switch(E.ifun){
        case JMP: //unconditional jump
            return 1;

        case JLE: //jump if less than or equal to
            if((sF ^ oF) || zF) return 1;
            else return 0;

        case JL: //jump if less than
            if(sF^oF) return 1;
            else return 0;

        case JE: //jump if equal to
            if(zF) return 1;
            else return 0;

        case JNE: //jump if not equal to
            if(!zF) return 1;
            else return 0;

        case JGE: //jump if greater than or equal to
            if(!(sF ^ oF)) return 1;
            else return 0;

        case JG: //jump if greater than
            if(!(sF^oF) && !zF) return 1;
            else return 0;

        default: //otherwise
            return 0;
    }
}

/* Function Name: calcECnd
 * Purpose:       Calculates whether or not a conditional move should be taken
 *
 * Parameters:    -
 * Returns:       TRUE if the conditional move should be taken, FALSE otherwise
 * Modifies:      -
 */
bool calcECnd()
{
	if(E.icode != ICMOVXX || E.icode != IRRMOVL){
        return 0;
    }

	//grab the condition codes
    unsigned int oF = getCC(OF);
    unsigned int sF = getCC(SF);
    unsigned int zF = getCC(ZF);

    //determine the kind of move
    switch(E.ifun){
        case RRMOVL: //"unconditional" move
            return 1;

        case CMOVLE: //move if less than or equal to
            if((sF ^ oF) || zF) return 1;
            else return 0;

        case CMOVL: //move if less than
            if(sF ^ oF) return 1;
            else return 0;
        case CMOVE: //move if equal to
            if(zF) return 1;
            else return 0;

        case CMOVNE: //move if not equal to
            if(!zF) return 1;
            else return 0;

        case CMOVGE: //move if greater than or equal to
            if(!(sF ^ oF)) return 1;
            else return 0;

        case CMOVG: //move if greater than
            if((!(sF^oF))&&(!zF)) return 1;
            else return 0;
    }
}

/* Function Name: M_bubble
 * Purpose:       Determines whether or not the M pipeline register should be bubbled
 *
 * Parameters:    status - struct with status information from the memory and writeback stages
 * Returns:       bool to determine whether the M register should be bubbled
 * Modifies       -
 */
bool M_bubble(statusType status){
    if(status.m_stat != SAOK || status.W_stat != SAOK) return TRUE;
    return FALSE;
}

/* Function Name: getEregister
 * Purpose:       Returns a copy of the E register
 * 
 * Parameters:    none
 * Returns:       e register
 * Modifies:      none
 */
eregister getEregister(){
    return E;
}

/* Function Name: clearEregister
 * Purpose:       Clears the E register
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      E
 */
void clearEregister(){
    clearBuffer((char *) &E, sizeof(E));
    E.stat = SAOK;
    E.icode = INOP;
}

/* Function Name: seteDstE
 * Purpose:       sets the destination register
 *
 * Parameters:    condition codes set in this stage
 * Returns:       the destination register
 * Modifies:      -
 */
unsigned int seteDstE(bool e_cnd)
{   
    if((E.icode == IRRMOVL) && !e_cnd) return RNONE;
	else return E.dstE;
}

/* Function Name: initializeFuncPtrArray()
 * Purpose:       initializes the array of function pointers
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      E
 */
void initializeFuncPtrArray()
{
	//fill the function pointer array with returnZero()
    int i = 0;
    for(i; i < 16; i++) functions[i] = returnZero;

    //put the functions in their respective places
    functions[IDUMP] = dump;
    functions[IOPL] = performOpl;
    functions[IIRMOVL] = performIrmovl;
    functions[ICMOVXX] = performCmovl;
    functions[IRMMOVL] = performRmmovl;
    functions[IMRMOVL] = performMrmovl;
    functions[IPUSHL] = performPushl;
    functions[IPOPL] = performPopl;
    functions[ICALL] = performCall;
    functions[IRET] = performRet;
}

/* Function Name: updateEregister
 * Purpose:       Passes values from d stage to E register
 *
 * Returns:       -
 * Parameters:    All parameters are the corresponding fields to the E register
 * Modifies:      ALl of the E register
 */
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE,
    unsigned int dstM, unsigned int srcA, unsigned int srcB)
{
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

//****************************************
//*** Function Pointer Array Functions ***
//****************************************

/* Function Name: performOpl()
 * Purpose:       executes the designated operation if it's an an opl code
 *
 * Returns:       result of add, subtract, and, or xor of E.valA and E.valB
 * Parameters:    None
 * Modifies:      Condition codes when appropriate
 */
unsigned int performOpl(bool status)
{
    int aluA = E.valA;
    int aluB = E.valB;
    int ret;

    if(!status) return aluA+aluB;

	//Perform calculations and check for overflow.
	//Check for zero result and negative result later.
	switch(E.ifun)
	{
		case ADDL: //addition
			ret = aluA + aluB;

			//positive overflow
			if(ret <= 0 && aluA > 0 && aluB > 0) setCC(OF, 1);

			//negative overflow
			else if(ret >= 0 && aluA < 0 && aluB < 0) setCC(OF, 1);

			//no overflow
			else setCC(OF, 0);
            break;

		case SUBL: //subtraction
			ret = aluB - aluA;

			//positive overflow
			if(ret <= 0 && aluA < 0 && aluB > 0) setCC(OF, 1);

			//negative overflow
			else if(ret >= 0 && aluA > 0 && aluB < 0) setCC(OF, 1);

			//no overflow
			else setCC(OF, 0);
            break;

		case ANDL: //bitwise and
			ret = aluA & aluB;
            break;

		case XORL: //bitwise exclusive or
			ret = aluA ^ aluB;
            break;
	}

	//Check for a result of 0.
	if(ret == 0) setCC(ZF, 1);
	else setCC(ZF, 0);

	//Check for a negative result.
	if(ret < 0) setCC(SF, 1);
	else setCC(SF, 0);

	return ret;
}

//returns E.valC to move an immediate into a register
unsigned int performIrmovl(bool status){
	return E.valC;
}

//returns E.valA to conditionally move based on registers
unsigned int performCmovl(bool status){
	return E.valA;
}
//returns E.valB + E.valC to move from a register to memory
unsigned int performRmmovl(bool status){
    return E.valB + E.valC;
}

//returns E.valB + E.valC to move from memory to register
unsigned int performMrmovl(bool status){
    return E.valB + E.valC;
}

//decrements the stack pointer by 4 to push a value
unsigned int performPushl(bool status){
	return E.valB - 4;
}

//increments the stack pointer by 4 to push a value
unsigned int performPopl(bool status){
	return E.valB + 4;
}

//filler function
unsigned int returnZero(bool status){
	return 0;
}

//performs a memory dump
unsigned int dump(bool status){
	return E.valC;
}

//performs a call to a function in the assembly
unsigned int performCall(bool status){
    return E.valB - 4;
}

//performs a return from a call
unsigned int performRet(bool status){
    return E.valB + 4;
}

