#include <stdio.h>
#include "bool.h"
#include "tools.h"

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).
*/

/* Function Name:   getBits
 * Purpose:         Return a bit values between indices low and high
 *
 * Parameters:      low - LSB of range to return
 *                  high - MSB of range to return
 *                  source - unsigned int with desired bits
 * Returns:         unsigned int made up of desired bit values
 */
unsigned int getBits(int low, int high, unsigned int source)
{
    if(low < 0 || high > 31 || low > high) return 0; //error checking
    int mask = (1 << 31); //create mask 0x80000000
    mask = mask >> (31-(high-low+1)); //arithmetic right shift of mask to desired position
    source = source >> low; //right shifts source so LSB of desired range is at index 0

    return ~mask & source; //complements mask and ands it with source to get desired bits
} 

/* Function Name:   setBits
 * Purpose:         Sets the bits between low and high to 1
 *
 * Parameters:      low - LSB of range of bits to set
 *                  high - MSB of range of bits to set
 *                  source - unsigned int to alter
 * Returns:         unsigned int containing altered source
 */
unsigned int setBits(int low, int high, unsigned int source)
{
    if(low < 0 || high > 31 || low > high) return source; //error checking
    int mask = 0x0; //blank mask
    int numOfBits = high-low+1; //number of bits to set to 1
    
    unsigned int i;
    for(i = 0; i < numOfBits; i++){
        mask = mask << 1; //create a mask of 1s of length numOfBits with LSB at index 0
        mask++;
    }

    mask = mask << low; //shifts mask out to desired position
    return mask | source; //set desired bits in source to 1 and return
}

/* Function Name:   clearBits
 * Purpose:         Clears the bits between low and high to 0
 *
 * Parameters:      low - LSB of range of bits to clear
 *                  high - MSB of range of bits to clear
 *                  source - unsigned int to alter
 * Returns:         unsigned int containing altered source
 */
unsigned int clearBits(int low, int high, unsigned int source)
{
       if(low < 0 || high > 31 || low > high) return source; //error checking
       int mask = 0x0; //blank mask
       int numOfBits = high-low+1; //number of bits to clear

       unsigned int i;
       for(i = 0; i < numOfBits; i++){
             mask = mask << 1; //mask of 1s with length equal to numOfBits
             mask++;
        }

        mask = ~(mask << low); //shifts mask to the desired position and complements it
        return mask & source; //clears desired bits and returns

}

/* Function Name:   assignOneBit
 * Purpose:         Set a desired bit to either a 0 or 1
 *
 * Parameters:      bitNumber - index of desired bit to be set
 *                  bitValue - value to set bit to (0 or 1)
 *                  source - unsigned int to be altered
 * Returns:         unsigned int containing altered source
 */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
    if(bitNumber > 31 || bitNumber < 0) return source; //error checking
    if(bitValue == 1){
        int mask = (1 << bitNumber); //creates a mask and sets the desired bit to 1
        return mask | source;
    }
    if(bitValue == 0){
        int mask = ~(1 << bitNumber); //creates a mask and sets the desired bit to 0
        return mask & source;
    }
    return source; //returns if bitValue was an incorrect value
}

/* Function Name:   getByteNumber
 * Purpose:         Returns a specific byte in an unsigned int
 *
 * Parameters:      byteNo - desired byte to be returned (3[MSB] - 0[LSB])
 *                  source - unsigned int containing desired byte
 * Returns:         unsigned 8-bit char with desired byte value
 */
unsigned char getByteNumber(int byteNo, unsigned int source)
{
    if(byteNo > 3 || byteNo < 0) return 0; //error checking
    int i;
    for(i = 0; i < byteNo; i++){
        source = source >> 8; //shifts source so desired byte is between indices 0 and 7
    }
    return (unsigned char)(0xFF & source); //pulls byte out and casts it as an unsigned char before returning
}

/* Function Name:   putByteNumber
 * Purpose:         Sets an 8-bit range of indices to a specified byte value
 *
 * Parameters:      byteNo - desired 8-bit range of indices to be altered (3[MSB] - 0[LSB])
 *                  byteValue - 8-bit value to place
 *                  source - unsigned int to be altered
 * Returns:         unsigned int with new byte value replacing the old byte value
 */
unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
    if(byteNo < 0 || byteNo > 3) return source; //error checking
    unsigned int bV = (unsigned int)byteValue; //casts 8-bit byteValue to 32-bit unsigned int
    bV <<= byteNo*8; //shifts byte value out to the correct spot
    int lowbit = byteNo*8; //determines LSB of range of bits to replace
    int highbit = (byteNo+1)*8-1; //determines MSB of range of bits to replace
    source = clearBits(lowbit, highbit, source); //clears bits at the byteNo to be replaced
    return source | bV; //sets bits to the new byte value
}

/* Function Name:   buildWord
 * Purpose:         Takes 4 8-bit unsigned char values and combines them into a 32-bit word
 *
 * Parameters:      byte0 - least significant byte of new word
 *                  byte1 - 2nd byte of new word
 *                  byte2 - 3rd byte of new word
 *                  byte3 - most significant byte of new word
 * Returns:         32-bit unsigned int with combined bytes
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
    //casts all 8-bit char values to 32-bit unsigned ints
    unsigned int b0 = (unsigned int)byte0;
    unsigned int b1 = (unsigned int)byte1;
    unsigned int b2 = (unsigned int)byte2;
    unsigned int b3 = (unsigned int)byte3;

    unsigned int word = b0+(b1<<8)+(b2<<16)+(b3<<24); //builds word using left shifts
    return word; //return word
}

/* Function Name:   isNegative
 * Purpose:         Checks if MSB of an unsigned int is a 1
 *
 * Parameters:      source - unsigned int to check
 * Returns:         0 if source would not be negative
 *                  1 if source would be negative
 */
bool isNegative(unsigned int source)
{
    return (source>>31) & 0x1; //shifts bit 31 to index 0 and checks if 1 or 0
}

/* Function Name:   expandBits
 * Purpose:         Creates a "string" representation of a 32-bit unsigned int, with spaces between bytes
 *
 * Parameters:      source - unsigned int to be represented
 *                  bits[] - array of characters to represent string
 * Returns:         void
 */
void expandBits(unsigned int source, char bits[36])
{
    int i;
    for(i = 34; i >= 0; i--){
        if(i == 8 || i == 17 || i == 26){ //Checks for bit positions in between bytes
           bits[i] = ' '; //places a space character
        }
        else {
           if(source & 0x01) bits[i] = '1'; //if LSB of source is 1, place char 1 in array index
           else bits[i] = '0'; //otherwise place a char 0

           source >>= 1; //right shift source
        }
    }
    bits[35] = '\0'; //add a null terminator
}

/* Function Name:   clearBuffer
 * Purpose:         Clear a certain number of bits in a buffer to 0
 *
 * Parameters:      *buff - beginning address of bits to clear
 *                  size - number of bits to clear
 * Returns:         void
 */
void clearBuffer(char * buff, int size)
{
    int i;
    for(i = 0; i < size; i++){
        buff[i] = 0; //iterates through buff and clears desired number of bits
    }
}

