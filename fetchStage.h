#ifndef FETCHSTAGE_H
#define FETCHSTAGE_H

//struct to represent the F register
typedef struct 
{
    unsigned int predPC;
} fregister;

//prototypes for functions called from files other than fetchStage
fregister getFregister();
void clearFregister();
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA,
    unsigned int rB, unsigned int valC, unsigned int valP);
void fetchStage(forwardType forwarded, bubbleType bubble);
#endif
