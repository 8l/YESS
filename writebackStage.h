#ifndef WRITEBACKSTAGE_H
#define WRITEBACKSTAGE_H

//struct representing the W register
typedef struct {
    unsigned int stat, icode, valE, valM, dstE, dstM;
} wregister;

//prototypes for functions called from files other than writebackStage
wregister getWregister();
void clearWregister();
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE, 
    unsigned int valM, unsigned int dstE, unsigned int dstM);
bool writebackStage(forwardType *forward, statusType* status);

#endif
