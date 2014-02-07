#ifndef MEMORY_H
#define MEMORY_H

//used simulate the memory used by the program, 1024 indices of 4 bytes
#define MEMSIZE 1024

//prototypes
static unsigned int fetch(int address, bool * memError);
static void store(int address, unsigned int value, bool * memError);
unsigned char getByte(int address, bool * memError);
void putByte(int address, unsigned char value, bool * memError);
void clearMemory();
unsigned int getWord(int address, bool * memError);
void putWord(int address, unsigned int value, bool * memError);
#endif 

