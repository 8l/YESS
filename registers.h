#ifndef REGISTERS_H
#define REGISTERS_H

//register identifiers
#define EAX    0x0
#define ECX    0x1
#define EDX    0x2
#define EBX    0x3
#define ESP    0x4
#define EBP    0x5
#define ESI    0x6
#define EDI    0x7
#define REGSIZE 8         //number of registers
#define ZF     0x2        //zero flag is bit 2 of CC
#define SF     0x1        //sign flag is bit 1 of CC
#define OF     0x0        //overflow flag is bit 0 of CC
#define RNONE  0xf        //no register needed


//prototypes
unsigned int getRegister(int regNum);
void setRegister(int regNum, unsigned int regValue);
void clearRegisters();
void setCC(unsigned int bitNumber, unsigned int value);
unsigned int getCC(unsigned int bitNumber);
#endif

