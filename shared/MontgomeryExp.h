#pragma once
#include "Defines.h"

class MontgomeryExp
{
public:
    void init(const uint8* mod);
    
    void ModExpSimple(uint8* S, uint8 G, uint8* P);
    void ModExpFunny(uint8* S, uint8* B, uint8* V, uint8* A, uint8* U, uint8* X);
    void ModExpAnother(uint8* S, uint8* A, uint8* V, uint8* U, uint8* B);
    void modN(uint8* S, uint8* A);
private:
    void internal_ModExp(uint8* S, uint8* A, uint8* P,uint8 size);
    void step(uint8*S, uint8* A,uint8* B);
    uint8 N[32], Ni[32], Ri[32];
};
