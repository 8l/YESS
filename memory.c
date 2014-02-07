#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "registers.h"


/*
 * Memory.c - container for memory. 
 * Memory will be stored in memArray[].
 * Memory is organized as 1024 four-byte words.
 * Access to memArray[] will only be permitted
 * via the fetch and store functions.
 */

static unsigned int memArray[MEMSIZE]; //Memory

//MAKE THIS STATIC
/* Function Name: fetch
 * Purpose:       Used by other functions to retrieve data from memory
 *
 * Parameters:    address - address of data to retrieve
 *                memError - bool for memory errors
 * Returns:       unsigned int representing data
 * Modifies:      none
 */
static unsigned int fetch(int address, bool * memError)
{
    //If the requested address isn't within boundaries...
    if(address > 4095 || address < 0)
    {
        *memError = TRUE;
        return 0;
    }
    //if it is...
    else 
    {
        *memError = FALSE;
        return memArray[address];
    }
}

//MAKE THIS STATIC
/* Function Name: store
 * Purpose:       Used by other methods to store data in memory
 *
 * Parameters:    address - address in memory to store data
 *                value - data to be stored
 *                memError - bool for memory errors
 * Returns:       none
 * Modifies:      memArray
 */
static void store(int address, unsigned int value, bool * memError)
{
    //If the address isn't within boundaries
    if(address > 4095 || address < 0) *memError = TRUE;
    //If it is
    else 
    {
        *memError = FALSE;
        memArray[address] = value;
    }
}

/* Function Name: getByte
 * Purpose:       Returns a byte of information from memory
 *
 * Parameters:    address - address of byte to retrieve
 *                memError - bool for memory errors
 * Returns:       unsigned char representing byte of information
 * Modifies:      none
 */
unsigned char getByte(int address, bool * memError){
    //If the address isn't within boundaries
    if(address > 4095 || address < 0)
    {
        *memError = TRUE;
        return 0;
    }
    //If it is
    else
    {
        *memError = FALSE;
        //unsigned int byteAdd = (((unsigned int) &memArray) + address);
        unsigned int fromMem = fetch(address/4, memError);        //Get the word
        char toReturn = getByteNumber((address % 4), fromMem);  //Get the byte from the word
        return toReturn;
    }
}

/* Function Name: putByte
 * Purpose:       Store a byte of information in memory
 *
 * Parameters:    address - address in memory to store data
 *                value - data to be stored
 *                memError - bool for memory errors
 * Returns:       none
 * Modifies:      memArray
 */
void putByte(int address, unsigned char value, bool * memError){
    //If the address isn't within boundaries
    if(address >  4095|| address < 0) *memError = TRUE;
    //If it is
    else
    {
        *memError = FALSE;
        unsigned int word = fetch((address / 4), memError);
        word = putByteNumber((address % 4), value, word);
        store((address / 4), word, memError);
    }
}

/* Function Name: clearMemory
 * Purpose:       Clear the memory
 *
 * Parameters:    none
 * Returns:       none
 * Modifies:      memArray - sets everything to 0
 */
void clearMemory(){
    clearBuffer((char*) &memArray, (MEMSIZE * sizeof(unsigned int)));
}

/* Function Name: getWord
 * Purpose:       Retrieve a word(4 bytes) of data from memory
 *
 * Parameters:    address - first byte of data to retrieve
 *                memError - bool for memory errors
 * Returns:       unsigned int representing word of data
 * Modifies:      none
 */
unsigned int getWord(int address, bool * memError){
    //Make sure the address is a multiple of 4
    if((address % 4) != 0 || address < 0 || address > 4095) 
    {
        *memError = TRUE;
        return 0;
    }
    //If it is a multiple of four
    else
    {
        *memError = FALSE;
        //unsigned int word = fetch((address / 4), memError);
        unsigned int word = getByte(address, memError) | (getByte(address+1, memError) << 8) | (getByte(address+2, memError) << 16) |
                            getByte(address+3, memError) << 24;
        return word;
    }
}

/* Function Name: putWord
 * Purpose:       Stores a word(4 bytes) of information in memory
 *
 * Parameters:    address - beginning address in memory to store data
 *                value - data to be stored
 *                memError - bool for memory errors
 * Returns:       none
 * Modifies:      memArray
 */
void putWord(int address, unsigned int value, bool * memError){
    if(address % 4 != 0) *memError = TRUE;
    else
    {
        *memError = FALSE;
        store((address / 4), value, memError);
    }
}
