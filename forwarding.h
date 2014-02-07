#ifndef FORWARDING_H
#define FORWARDING_H

//struct used to forward values from later stages in the pipeline
typedef struct
{
    unsigned int M_Cnd, M_icode, M_valA, W_icode, W_valE, W_dstE, W_valM, W_dstM, M_valE, M_dstE, m_valM, M_dstM, e_valE, e_dstE;
} forwardType;

#endif
