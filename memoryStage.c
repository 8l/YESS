#include "bool.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "memoryStage.h"
#include "tools.h"
#include "instructions.h"
#include "writebackStage.h"
#include "registers.h"

//M register holds the input for the memory stage
//It is only accessible from this file. (static)
static mregister M;

//prototypes
bool W_stall(statusType status);
bool setReadSig();
bool setWriteSig();
unsigned int selectMemAddress();

/* Function Name: memoryStage
 * Purpose:       Simulate the memory stage of pipeline execution
 *
 * Parameters:    status - pointer to a struct with status information for the memory and writeback stages
 *                forwarded - pointer to a struct containing values used for forwarding
 *                bubble - pointer to a struct containing values used for bubbling/stalling
 * Returns:       -
 * Modifies:      Values in the status, forwarded, and bubble structs
 */
void memoryStage(statusType *status, forwardType *forwarded, bubbleType *bubble){

    //gets the needed address in memory
	unsigned int memAddress = selectMemAddress();

	unsigned int m_stat = M.stat;
	bool memError = FALSE;
	unsigned int m_valM = M.valA;

    //checks if memory will be read from
	if(setReadSig())
	{
        //reads the value from memory and checks for a memory address error
		m_valM = getWord(memAddress, &memError);
		if(memError) m_stat = SADR;
	}
    //checks if memory will be written to
	else if(setWriteSig())
	{
        //writes the value of m_valM to memory and checks for a memory address error
		putWord(memAddress, m_valM, &memError);
		if(memError) m_stat = SADR;
	}

    //sets the appropriate value in the status struct
    status->m_stat = m_stat;

    //sets the appropriate values in the forwarded struct
    forwarded->M_valE = M.valE;
    forwarded->m_valM = m_valM;
    forwarded->M_dstM = M.dstM;
    forwarded->M_dstE = M.dstE;
    forwarded->M_Cnd = M.Cnd;
    forwarded->M_icode = M.icode;
    forwarded->M_valA = M.valA;

    //sets the appropriate value in the bubble struct
    bubble->M_icode = M.icode;

    //checks if the W register should be stalled and updates the W register accordingly
	if(!W_stall(*status)) updateWregister(m_stat, M.icode, M.valE, m_valM, M.dstE, M.dstM);
}

/* Function Name: selectMemAddress
 * Purpose:       Returns the the appropriate value representing the needed address in memory
 *
 * Parameters:    -
 * Returns:       The address in memory required
 * Modifies:      -
 */
unsigned int selectMemAddress()
{
    //returns the appropriate address based on the icode
	switch(M.icode)
	{
		case IRMMOVL:
		case IMRMOVL:
		case IPUSHL:
		case ICALL:
		    return M.valE;
		case IPOPL:
		case IRET:
		  	return  M.valA;
    }

    //if a memory address is not needed, return 0
	return 0;
}

/* Function Name: setReadSig
 * Purpose:       Determines if memory will be read from in this stage
 *
 * Parameters:    -
 * Returns:       TRUE if memory is read from, FALSE otherwise
 * Modifies:      -
 */
bool setReadSig()
{
    //readsig is based on icode
	if(M.icode == IMRMOVL || M.icode == IPOPL || M.icode == IRET) return TRUE;
	else return FALSE;
}

/* Function Name: setWriteSig
 * Purpose:       Determines if memory will be written to in this stage
 *
 * Parameters:    -
 * Returns:       TRUE if memory will be written to, FALSE otherwise
 * Modifies:      -
 */
bool setWriteSig()
{
    //writesig is based on icode
	if(M.icode == IRMMOVL || M.icode == IPUSHL || M.icode == ICALL) return TRUE;
	else return FALSE;
}

/* Function Name: getMregister
 * Purpose:       Returns a copy of the M register
 *
 * Parameters:    none
 * Returns:       mregister
 * Modifies:      none
 */
mregister getMregister(){
    return M;
}

/* Function Name: clearMregister
 * Purpose:       Clears the M register
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      M
 */
void clearMregister(){
    clearBuffer((char *) &M, sizeof(M));
    M.stat = SAOK;
    M.icode = INOP;
}

/* Function Name: updateMregister
 * Purpose:       Passes the values from the e stage to the Memory register
 *
 * Parameters:    All the values that correspond to the fields in the M register
 * Returns:       -
 * Modifies:      M register
 */
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd, unsigned int valE,
                    unsigned int valA, unsigned int dstE, unsigned int dstM){
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

/* Function Name: W_stall
 * Purpose:       Determines if the W register should be stalled
 *
 * Parameters:    status - struct containing value for W status
 * Returns:       TRUE if register should be stalled, FALSE otherwise
 * Modifies:      -
 */
bool W_stall(statusType status){
    //stall register if W_stat is SINS, SADR, or SHLT
    if(status.W_stat != SAOK) return TRUE;
    return FALSE;
}
