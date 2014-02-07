#ifndef BUBBLING_H
#define BUBBLING_H
//struct used to determine if bubbling/stalling of a pipeline register should occur
typedef struct{
    unsigned int d_srcA, d_srcB, E_dstM, e_Cnd, E_icode, D_icode, M_icode;
}bubbleType;
#endif
