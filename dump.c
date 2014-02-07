#include "bool.h"
#include <stdio.h>
#include "dump.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "registers.h"
#include "memory.h"

#define WORDSPERLINE 8
#define LINELENGTH

//prototypes of Functions only called within this file
static void dumpLine(int line[WORDSPERLINE], int address);
static void buildLine(int line[WORDSPERLINE], int address);
static int isEqual(int prevLine[WORDSPERLINE], int currLine[WORDSPERLINE]);
static void copy(int *, int *);
//end prototypes

// Function: dumpMemory
// Description: Outputs the contents of the YESS little-endian memory 
//              WORDSPERLINE four-byte words per line.  A * is displayed 
//              at the end of a line if each line in memory after that 
//              up to the next ine displayed is identical to the * line.
// Params: none
// Returns: none
// Modifies: none
void dumpMemory()
{
    int address = 0;
    int prevLine[WORDSPERLINE];
    int currLine[WORDSPERLINE];
    int star = 0;
    buildLine(prevLine, address);
    dumpLine(prevLine, address);
    for (address=WORDSPERLINE; address < MEMSIZE; address+=WORDSPERLINE)
    {
       buildLine(currLine, address);    
       if (isEqual(prevLine, currLine))
       {
          if (!star)
          {
              printf("*\n");
              star = 1;
          }
       } else
       { 
          printf("\n");
          dumpLine(currLine, address);
          star = 0;
       }
       copy(prevLine, currLine);
    }
    printf("\n");
}

// Function: copy
// Description: This Function copies the contents of the cLine array into the
//              pLine array.
// Params:  cline - array of ints (current line)
// Returns: pline - initialized to values in cline (previous line)
// Modifies: none
void copy(int pLine[WORDSPERLINE], int cLine[WORDSPERLINE])
{
    int i;
    for (i = 0; i < WORDSPERLINE; i++) pLine[i] = cLine[i];
}

// Function: dumpLine
// Description: This Function outputs the starting address in the variable
//              address and the contents of the line array.
// Params: line - array of ints to output
//        address - row header
// Returns: none
// Modifies: none
void dumpLine(int line[WORDSPERLINE], int address)
{
    int i;
    printf("%03x: ", address*4);
    for (i = 0; i < WORDSPERLINE; i++) printf("%08x ", line[i]);
}

// Function: buildLine
// Descripton: This Function accesses memory for WORDSPERLINE words and 
//             sets the line array to the WORDSPERLINE
//             words from memory.
// Params: address - starting index to access memory
// Returns: none
// Modifies: line - array initialized to values in memory
void buildLine(int line[WORDSPERLINE], int address)
{
    int i;
    char byte0, byte1, byte2, byte3;
    bool memError;
    for (i = 0; i < WORDSPERLINE; i++, address++)
        line[i] = getWord((address * 4), &memError);
}

// Function: isEqual
// Description: This Function compares the contents of prevLine and currLine
//              and returns TRUE if they are equal.
// Params: prevLine - array of ints
//        currLine - array of ints
// Returns: returns TRUE if prevLine and currLine are identical
// Modifies: none
int isEqual(int prevLine[WORDSPERLINE], int currLine[WORDSPERLINE])
{
    int i;
    for (i = 0; i < WORDSPERLINE; i++)
        if (prevLine[i] != currLine[i]) return FALSE;
    return TRUE;
}

// Function: dumpProgramRegisters
// Description: This Function outputs the contents of the YESS program registers
//              to standard out.
// Params: none
// Returns: none
// Modifies: none
void dumpProgramRegisters()
{
    printf("%%eax: %08x %%ecx: %08x %%edx: %08x %%ebx: %08x\n",
           getRegister(EAX), getRegister(ECX), getRegister(EDX), 
           getRegister(EBX));
    printf("%%esp: %08x %%ebp: %08x %%esi: %08x %%edi: %08x\n\n",
           getRegister(ESP), getRegister(EBP), getRegister(ESI), 
           getRegister(EDI));
}

// Function: dumpProcessorRegisters
// Description: This Function outputs the contents of the YESS 
//              processor registers to standard out.
// Params: none
// Returns: none
// Modifies: none
void dumpProcessorRegisters()
{
    fregister F = getFregister();
    dregister D = getDregister();
    eregister E = getEregister();
    mregister M = getMregister();
    wregister W = getWregister();

    printf("CC - ZF: %01x SF: %01x OF: %01x\n", getCC(ZF), getCC(SF), getCC(OF)); 
    printf("F - predPC: %08x\n", F.predPC);
    printf("D - stat: %01x icode: %01x ifun: %01x rA: %01x rB: %01x valC: %08x  valP: %08x\n",
            D.stat, D.icode, D.ifun, D.rA, D.rB, D.valC, D.valP);
    printf("E - stat: %01x icode: %01x  ifun: %01x  valC: %08x valA: %08x valB: %08x\n",
            E.stat, E.icode, E.ifun, E.valC, E.valA, E.valB);
    printf("    dstE: %01x dstM: %01x srcA: %01x srcB: %01x\n",
            E.dstE, E.dstM, E.srcA, E.srcB);
    printf("M - stat: %01x icode: %01x Cnd: %01x valE: %08x valA: %08x dstE: %01x dstM: %01x\n",
            M.stat, M.icode, M.Cnd, M.valE, M.valA, M.dstE, M.dstM);
    printf("W - stat: %01x icode: %01x valE: %08x valM: %08x dstE: %01x dstM: %01x\n\n",
            W.stat, W.icode, W.valE, W.valM, W.dstE, W.dstM);
}
