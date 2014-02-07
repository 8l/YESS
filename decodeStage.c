#include "bool.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "decodeStage.h"
#include "tools.h"
#include "instructions.h"
#include "executeStage.h"
#include "registers.h"

/*D register holds the input for the decode stage.
 * It is only accessible from this file. (static)*/
static dregister D;

//prototypes
unsigned int selectFwdA(unsigned int d_srcA, forwardType forwarded);
unsigned int forwardB(unsigned int d_srcB, forwardType forwarded);
unsigned int getDstE();
unsigned int getDstM();
unsigned int getSrcA();
unsigned int getSrcB();
bool E_bubble(bubbleType *bubble);


/* Function Name: decodeStage
 * Purpose:       Perform the execution of the Decode Stage in the pipelines machine.
 *                Sort out source and destination registers and set up values for the
 *                Execute Stage to operate with.
 *
 * Returns:       -
 * Parameters:    forward - a struct used for forwarding of values from later stages
 *                bubble - pointer to a struct used for bubbling/stalling
 * Modifies:      -
 */
void decodeStage(forwardType forward, bubbleType *bubble){
    //set variables
    unsigned int d_srcA = getSrcA();
    unsigned int d_srcB = getSrcB();
    unsigned int d_dstM = getDstM();
    unsigned int d_dstE = getDstE();

    unsigned int d_valA, d_valB;

    //select forwarding sources
    d_valA = selectFwdA(d_srcA, forward);
    d_valB = forwardB(d_srcB, forward);

    //set the bubble conditions
    bubble->D_icode = D.icode;
    bubble->d_srcA = d_srcA;
    bubble->d_srcB = d_srcB;

    //update the E register for the Execute Stage
    if(E_bubble(bubble)) updateEregister(SAOK, INOP, 0, 0, 0, 0, RNONE, RNONE, RNONE, RNONE);
    else updateEregister(D.stat, D.icode, D.ifun, D.valC, d_valA, d_valB, d_dstE, d_dstM, d_srcA, d_srcB);
}

/* Function Name: getDregister
 * Purpose:       Returns a copy of the D register
 *
 * Parameters:    none
 * Returns:       D register
 * Modifies:      none
 */
dregister getDregister(){
    return D;
}

/* Function Name: clearDregister
 * Purpose:       Clears the D register
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      D
 */
void clearDregister(){
    clearBuffer((char *) &D, sizeof(D));
    D.stat = SAOK;
    D.icode = INOP;
}

/* Funcion Name: updateDregister
 * Purpose:      Used by the Fetch Stage to update values in the D register
 *
 * Returns:      None
 * Parameters:   Values used by the Decode Stage
 * Modifies:     None
 */
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP)
{
    D.stat = stat;
    D.icode = icode;
    D.ifun = ifun;
    D.rA = rA;
    D.rB = rB;
    D.valC = valC;
    D.valP = valP;
}

/* Function Name: getSrcA
 * Purpose:       Get the sourceA register if it's needed
 *
 * Returns:       D.rA if the register is needed, RNONE otherwise
 * Parameters:    None
 * Modifies:      None
 */
unsigned int getSrcA(){
    switch(D.icode){
        case IRRMOVL:
        case IRMMOVL:
        case IOPL:
        case IPUSHL:
            return D.rA;

        case IRET:
        case IPOPL:
        	return ESP;

        default:
            return RNONE;
    }
}

/* Function Name: getSrcB()
 * Purpose:       Get the sourceB register if it's needed
 *
 * Returns:       D.rB if the register is needed, RNONE otherwise
 * Parameters:    None
 * Modifies:      None
 */
unsigned int getSrcB(){
    switch(D.icode){
        case IMRMOVL:
        case IRMMOVL:
        case IOPL:
            return D.rB;

        case IPUSHL:
        case IPOPL:
        case ICALL:
        case IRET:
        	return ESP;

        default:
            return RNONE;
    }
}

/* Function Name: selectFwdA()
 * Purpose:       Selects the value of valA based on the icode or srcA registers in the
 *                decode stage.
 *
 * Returns:       The required valA value
 * Parameters:    -
 * Modifies:      -
 */
unsigned int selectFwdA(unsigned int d_srcA, forwardType forwarded){
    //returns D.valP(an address) if a jump or call instruction
    if(D.icode == IJXX || D.icode == ICALL) return D.valP;

    //if valA is not needed in the execute stage
    else if(d_srcA == RNONE) return 0;

    //checks if forwarding of a value in a later stage is needed
    else if(d_srcA == forwarded.e_dstE) return forwarded.e_valE; 
    else if(d_srcA == forwarded.M_dstM) return forwarded.m_valM;
    else if(d_srcA == forwarded.M_dstE) return forwarded.M_valE;
    else if(d_srcA == forwarded.W_dstM) return forwarded.W_valM;
    else if(d_srcA == forwarded.W_dstE) return forwarded.W_valE;

    //returns the value in d_srcA
    else return getRegister(d_srcA);
}

/* Function Name: forwardB()
 * Purpose:       Selects the value of valB based on the icode or srcB registers in the
 *                decode stage.
 *
 * Returns:       The required valB value
 * Parameters:    -
 * Modifies:      -
 */
unsigned int forwardB(unsigned int d_srcB, forwardType forwarded){

	//if valB is not needed in the execute stage
    if(d_srcB == RNONE) return 0;

    //checks if forwarding of a value in a later stage is needed
    else if(d_srcB == forwarded.e_dstE) return forwarded.e_valE;
    else if(d_srcB == forwarded.M_dstM) return forwarded.m_valM;
    else if(d_srcB == forwarded.M_dstE) return forwarded.M_valE;
    else if(d_srcB == forwarded.W_dstM) return forwarded.W_valM;
    else if(d_srcB == forwarded.W_dstE) return forwarded.W_valE;
    
    //returns the value in d_srcB
    else return getRegister(d_srcB);
}

/* Function Name: getDstM()
 * Purpose:       determines the destination register for
 *                the IMRMOVL and IPOPL operands
 *
 * Returns:       D.rA if the register is needed, RNONE otherwise
 * Parameters:    -
 * Modifies:      -
 */
unsigned int getDstM(){
    if(D.icode == IMRMOVL || D.icode == IPOPL) return D.rA;
    else return RNONE;
}

/* Function Name: getDstE()
 * Purpose:       determines the destination register for certain operands
 *
 * Returns:       D.rA, ESP, or RNONE
 * Parameters:    -
 * Modifies:      -
 */
unsigned int getDstE(){
    switch(D.icode){
        case IRRMOVL:
        case IIRMOVL:
        case IOPL:
            return D.rB;

        case IPUSHL:
        case IPOPL:
        case ICALL:
        case IRET:
            return ESP;

        default:
            return RNONE;
   }    
}

/* Function Name: E_bubble()
 * Purpose:       determines whether or not the E Register is bubbled
 * Returns:       TRUE if Y86 performed a mispredicted branch or
 *                or if there is a load/use hazard. Otherwise, FALSE
 *
 * Parameters:    bubble - the values to load into the registers if bubbled
 * Modifies:      -
 */
bool E_bubble(bubbleType *bubble){
    //local variables to make code easier to read
    unsigned int E_icode = bubble->E_icode;
    unsigned int E_dstM = bubble->E_dstM;

    //mispredicted branch detection
    bool misPredBr = (E_icode == IJXX && !bubble->e_Cnd);
    //load/use hazard detection
    bool loadUseHaz = ((E_icode == IMRMOVL || E_icode == IPOPL) && (E_dstM == bubble->d_srcA || E_dstM == bubble->d_srcB));

    if(misPredBr || loadUseHaz) return TRUE;
    return FALSE;
}
