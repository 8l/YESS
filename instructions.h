#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/*Defines the different constants used by the program
 * for instructions and function codes, as well as the
 * status codes.
 */

//instruction codes
#define IHALT 0
#define INOP 1
#define IRRMOVL 2
#define IIRMOVL 3
#define IRMMOVL 4
#define IMRMOVL 5
#define IOPL 6
#define IJXX 7
#define ICMOVXX 2
#define ICALL 8
#define IRET 9
#define IPUSHL 0xA
#define IPOPL 0xB
#define IDUMP 0xC

//IOPL function codes
#define ADDL 0
#define SUBL 1
#define ANDL 2
#define XORL 3

//IJXX function codes
#define JMP 0
#define JLE 1
#define JL 2
#define JE 3
#define JNE 4
#define JGE 5
#define JG 6

//IRRMOVL and CMOVL function codes
#define RRMOVL 0
#define CMOVLE 1
#define CMOVL 2
#define CMOVE 3
#define CMOVNE 4
#define CMOVGE 5
#define CMOVG 6

//status codes
#define SAOK 1
#define SHLT 2
#define SADR 3
#define SINS 4

#endif
