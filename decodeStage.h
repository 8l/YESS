#ifndef DECODESTAGE_H
#define DECODESTAGE_H

//struct representing the D pipeline register
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;

//prototypes for functions called from files other than decodeStage
dregister getDregister();
void clearDregiser();
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);
void decodeStage(forwardType forward, bubbleType *bubble);
#endif
