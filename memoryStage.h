#ifndef MEMORYSTAGE_H
#define MEMORYSTAGE_H

//struct representing the M register
typedef struct
{
    unsigned int stat, icode, Cnd, valE, valA, dstE, dstM;
} mregister;

//prototypes for functions called from files other than memoryStage
mregister getMregister();
void mememoryStage(statusType *status, forwardType *forwarded, bubbleType *bubble);
void clearMregister();
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd,
    unsigned int valE, unsigned int valA, unsigned int dstE, unsigned int dstM);
#endif
