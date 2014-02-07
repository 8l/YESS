#ifndef LOADER_H
#define LOADER_H

//prototypes
bool load(int argc, char * args[]);
int lenInst(char * line);
bool validFileName(char * name);
void discardRest(char * line, FILE * file);
unsigned int grabAddress(char* data);
bool isAddress(char * data);
bool isData(char * data);
bool isSpaces(char * data, unsigned int start, unsigned int end);
bool checkHex(char * data, unsigned int start, unsigned int end);
bool isData(char* line);
unsigned char grabDataByte(char * data, unsigned int start);
int checkLine(char * data);
void printError(unsigned int lineNum, char* line);

#endif
