#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "loader.h"
#include "memory.h"

/* Function Name: load
 * Purpose:       Driver function. Opens an input file, loads the machine code,
 *                performs a memory dump, then exits.
 *
 * Parameters:    argc - amount of arguments passed
 *                args[] - input file name
 * Returns:       TRUE if program loaded successfully
 *                FALSE if program failed to load
 */
bool load(int argc, char * args[])
{
    if((argc > 1) && (validFileName(args[1]))){ //check for valid file name and # of arguments

        FILE* f = fopen(args[1], "r"); //open the file
        
        unsigned char addrCounter = 0; //counter used to check for overlapping addresses
        char line[80];
        unsigned int addr;
        int i;              //Address parser count
        unsigned int lineCount = 0;
        int len;            //Length of instruction
        unsigned char dataByte;      //Temporary storage of info
        bool loadErr = FALSE;
        int check;
        while(fgets(line,80,f))     //While we haven't reached the end of the file...
        {           
            lineCount++;
            check = checkLine(line);                    //Check the line

            if(check == 1)                              //If the line is valid...
            {                                           
                addr = grabAddress(line);               //Grab the address
                if(addr < addrCounter){                 //Check for errors with the address
                    printError(lineCount, line);
                    loadErr = TRUE;
                    break;
                }
                len = lenInst(line);                    //Find the length of the instruction
                int i = 0;
                unsigned int byteNo = 0;
                while(byteNo < len)                          //And load the data byte by byte
                {
                    dataByte = grabDataByte(line, 9+i);
                    putByte(addr, dataByte, &loadErr);
                    addr++;
                    byteNo++;
                    addrCounter = addr;
                    i += 2;
                }
              
            }                 
            else if(check == 0)                         //If the line is invalid...
            {                                           //Halt loading and give an error message
                printError(lineCount, line);
                loadErr = TRUE;
                break;
            }    
            discardRest(line, f);                                 //If the line or instructions are just spaces, do nothing
        }

        return loadErr;                                 //return TRUE if program was successfully loaded
    }
    printf("file opening failed\n usage: yess <filename>.yo\n"); //error message

    return TRUE; //return if invalid filename
}

void printError(unsigned int lineNum, char* line){
    printf("Error on line%d\n", lineNum);
    printf("%s\n", line);
}

/* Function Name: lenInst
 * Purpose:       Determine length of instruction in bytes on a line
 *
 * Parameters:    line - line of YAS
 * Returns:       Length of instruction in bytes
 *                (No instruction means 0 bytes long)
 * Modifies:      -
 */
int lenInst(char * line)
{
    int bytes = 0;
    int count = 9;

    while(count < 20)
    {
        if(checkHex(line, count, count + 1)) bytes++;
        else break;

        count += 2;
    }
    return bytes;
}

/* Function Name: validFileName
 * Purpose:       Determines validity of input file name
 *
 * Parameters:    name: pointer to a character representing the beginning of a string
 * Returns:       Whether or not the input file name ends in ".yo"
 * Modifies:      -
 */
bool validFileName(char*  name)
{
    char * pch;

    if((pch = strrchr(name, '.')) != NULL) //find the last occurrence of a '.'
    if((*(pch+1) == 'y') && (*(pch+2) == 'o')) return TRUE;    //check if 2 characters after . are 'y' and 'o'

    return FALSE;
}

/* Function Name: discardRest
 * Purpose:       Looks for '\n' in the line and discards characters until '\n'
 *
 * Parameters:    A line file from the input file
 * Returns:       Flag for EOF  
 *                    - FALSE : not EOF (specifically, reached '\n')
 *                    - TRUE  : reached EOF
 * Modifies:      -
 */
void discardRest(char * line, FILE * file)
{
    int length = strlen(line);
    if(!(line[length - 1] == '\n')){ //If the line does NOT already contain a '\n' character
        char c = (char)fgetc(file);
        while(c != '\n' &&  !feof(file)){     //Parse through file 
            c = (char)fgetc(file);
        }
    }
    //If the we already have a '\n' character in the buffer...
   // else if(buffer[length - 1] == '\n') return FALSE;
   // else return TRUE; //The remaining case: there is an EOF already in the buffer
}

/* Function Name: grabAddress
 * Purpose:       Grabs the address of a data record in that line
 *
 * Parameters:    A record
 * Returns:       Address of the record in that line
 * Modifies:      -
 */
unsigned int grabAddress(char* data)
{
    long str = strtol((data+4), NULL, 16); //converts address to a long int
    return (unsigned int)str;
}

/* Function Name: isAddress
 * Purpose:       Determines if a record has an address
 *
 * Parameters:    data - a record to check
 * Returns:       bool of whether the record contains an address
 * Modifies:      -
 */
bool isAddress(char* data)
{
    bool xCheck = checkHex(data, 4, 6); //checks that the address is in hex

    if((data[2] == '0') && (data[3] == 'x') && (xCheck == TRUE)) return TRUE; //returns if there is an address
    else return FALSE;
}

/* Function Name: checkHex
 * Purpose:       Determines if the characters between 'start' and 
 *                'end' are hex characters, inclusively
 *
 * Parameters:    data - Data record to check
 *                start - location to start checking
 *                end - loaction to stop checking
 * Returns:       bool of whether the data record has hex inbetween 'start' and 'end'
 * Modifies:      -
 */
bool checkHex(char * data, unsigned int start, unsigned int end)
{
    //Assume all are hex until it runs into the first non-hex
    bool isHex = TRUE;
    int i;

    for(i = start; i <= end; i++) //Check each character
    { 
        int c = (int) data[i];    //Explicit type cast. Prevents warnings.
        if(isxdigit(c) == 0)      //If the character is a non-hex digit...
        {
            isHex = FALSE;
            break;                //Simply stop searching 
        }
    }

    return isHex;
}

/* Function name: isSpaces
 * Purpose:       Determines if 'data' contains whitespace (space characters
 *                specifically) between 'start' and 'end', inclusively.
 *
 * Parameters:    data - input string to check
 *                start - location to start at in the string
 *                end - location to terminate search
 * Returns:       bool of whether the input string contains spaces between 
 *                'start' and 'end'.
 */
bool isSpaces(char * data, unsigned int start, unsigned int end)
{
    bool isSpace = TRUE;
    int i;

    for(i = start; i <= end; i++)
    {
        int c = (int) data[i]; //check a character at a time
        if(isspace(c) == 0) //checks whether or no character is a space
        {
            isSpace = FALSE; //if it wasn't a space, return false
            break;
        }
    }

    return isSpace; //return true if all checked characters are spaces
}

/* Function Name: isData
 * Purpose:       Determines if there is data in the line after the address
 *
 * Parameters:    line - input record to check
 * Returns:       bool indicating whether the line contains data or not
 * Modifies:      none
 */
bool isData(char* line){
    
    if(!isSpaces(line, 9, 10)){ //checks for spaces where data should be
        int i;
        int count = 0;
        for(i = 9; i <= 20; i++){
            if(line[i] == ' ') break;
            else count++;    
        }
        bool check = checkHex(line, 9, 8+count);
        count = count%2;
        bool a = (check && !count);                
        return (a);
    }

    return FALSE; //return false if only spaces in record
}

/* Function Name: grabDataByte
 * Purpose:       Returns a byte of data from a data record
 *
 * Parameters:    line - input record to pull data from
 *                start - beginning index of byte of data to grab
 * Returns:       unsigned char holding the byte of data
 * Modifies:      none
 */
unsigned char grabDataByte(char* line, unsigned int start){
    char data[3];

    data[0] = line[start];
    data[1] = line[start+1];
    data[2] = '\0';

    long holder = strtol(data, NULL, 16);
    return (unsigned char)holder;
}

/* Function Name: checkLine
 * Purpose:       Determines validity of a line of YAS
 *
 * Parameters:    char * to a line of YAS
 * Returns:       -1 : all spaces in line or instruction.
 *                0 : invalid line
 *                1 : valid line
 * Modifies:      -
 */
int checkLine(char * line)
{   
    if(isSpaces(line, 0, 21) && line[22] == '|') 
            return -1;            //The case of an empty line
    else{
        if(!isSpaces(line, 0, 1)) //Check initial spaces
            return 0;
        if(!isAddress(line))      //Check for address
            return 0;
        if(!(line[7] == ':' && line[8] == ' ' && line[21] == ' ' && line[22] == '|')) //Check particular spots
            return 0;
        if(!(isData(line)))       //Check for data
        {                         //If no data...
            if(!isSpaces(line, 9, 20))//Check if only spaces
                return 0;         //If not spaces OR valid data
            else
                return -1;        //If just spaces
        }
        return 1;
    }
}
