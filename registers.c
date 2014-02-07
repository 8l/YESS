#include "bool.h"
#include <stdio.h>
#include "tools.h"
#include "registers.h"
#include "memory.h"

//'registers' are only accessible through get/setRegister() in registers.c
static unsigned int registers[REGSIZE];

//unsigned int representing the Condition Code register
static unsigned int CC;

/* Function Name: getRegister
 * Purpose:       Returns a desired register
 *
 * Parameters:    regNum - signed int indicating register to return
 * Returns:       Desired register, represented as an unsigned int
 * Modifies:      none
 */
unsigned int getRegister(int regNum)
{
    if(regNum > 7 || regNum < 0) return 0; //Error checking
    else return registers[regNum]; //return register
}

/* Function Name: setRegister
 * Purpose:       Sets a register to a desired value
 *
 * Parameters:    regNum - signed int indicating register to modify
 *                regValue - value to place in register
 * Returns:       none
 * Modifies:      Register indicated by regNum
 */
void setRegister(int regNum, unsigned int regValue)
{
    if(regNum > 7 || regNum < 0) {} //Error checking, do nothing
    else registers[regNum] = regValue; //Set register to value
}

/* Function Name: clearRegisters
 * Purpose:       Clears F, D, W, E and M registers
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      F, D, W, E and M registers
 */
void clearRegisters()
{
   //clears the registers
   clearFregister();
   clearDregister();
   clearWregister();
   clearEregister();
   clearMregister();
}

/* Function Name: setCC
 * Purpose:       Sets/clears a flag in the condition code register
 *
 * Parameters:    bitNumber - flag to set/clear
 *                value - 1(set) or 0(clear)
 * Returns:       none
 * Modifies:      CC
 */
void setCC(unsigned int bitNumber, unsigned int value)
{
    //error checking of bitNumber
    if(bitNumber == ZF || bitNumber == SF || bitNumber == OF){
        //error checking of value
        if(value == 0 || value == 1){
           CC =  assignOneBit(bitNumber, value, CC); //set or clear flag
        }
        else printf("Invalid value passed to setCC"); //for value errors
    }
    else printf("Invalid bitNumber passed to setCC"); //for bitNumber errors
}

/* Function Name: getCC
 * Purpose:       Used for checking a flag in the condition code register
 *
 * Parameters:    bitNumber - flag to check
 * Returns:       unsigned int with value of flag, -1 if invalid bitNumber passed
 * Modifies:      none
 */
unsigned int getCC(unsigned int bitNumber)
{
    //error checking
    if(bitNumber == ZF || bitNumber == SF || bitNumber == OF){
        return getBits(bitNumber, bitNumber, CC); //returns value of desired flag
    }
    printf("Invalid bitNumber passed to getCC"); //for errors
    return -1; //invalid return option
}
