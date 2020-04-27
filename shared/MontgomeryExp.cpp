#include "MontgomeryExp.h"
#include <cstring>

void MontgomeryExp::init(const uint8* mod)
{
    memcpy(N, mod, 32);
    // calculate stuff... it could be simply set cause N is always same, but anyway!
    memset(Ni, 0x00, 32);
    uint32 x = 0;
    uint8 i, j;
    for (j = 0; j < 32; j++)
    {
        for (i = 0; i < j; i++)
            x += Ni[i] * N[j - i];

        for (i = 0; i < 8; i++)
            if (!(x & (1 << i)))
            {
            x += N[0] << i;
            Ni[j] |= (1 << i);
            }
        x = x >> 8;
    }
    x++;
    for (j = 1; j < 32; j++)
    {
        for (i = j; i < 32; i++)
            x += Ni[i] * N[31 - i + j];
        Ri[j - 1] = x % (1 << 8);
        x = x >> 8;
    }
    Ri[31] = x;
}

// S = G^P mod N
void MontgomeryExp::ModExpSimple(uint8* S, uint8 G, uint8* P)
{
    uint8 A[65];
    memset(A, 0x00, 65);
    A[32] = G;

    internal_ModExp(S, A, P, 20);
}

// S = (B-3V)^(A+U*X) mod N
void MontgomeryExp::ModExpFunny(uint8* S, uint8* B, uint8* V, uint8* A, uint8* U, uint8* X)
{
    uint8 p[41], d[65], i, j;
    uint16 x;

    {
        // p = u*x +a
        memset(p, 0x00, 41);

        for (i = 0; i < 20; i++)
        {
            x = 0;
            for (j = 0; j < 20; j++)
            {
                x += U[i] * X[j];
                x += p[i + j];
                p[i + j] = x % 256;
                x = x >> 8;
            }
            p[i + 20] += x;
        }

        x = 0;
        i = 0;
        while ((i < 20) || x)
        {
            x += p[i];
            if (i < 20)
                x += A[i];
            p[i] = x % 256;
            x = x >> 8;
            i++;
        }
    }

    {
        // d = b - 3*v mod n
        bool cut;
        memset(d, 0x00, 65);
        memcpy(d, B, 32);
        d[32] = 0x00;
        
        for (j = 0; j < 3; j++)
        {
            cut = false;
            for (i = 0; i < 32; i++)
            {
                if (cut && d[i] == 0)
                {
                    d[i]--;
                    d[i] -= V[i];
                    continue;
                }
                else if(cut)
                    d[i]--;

                cut = (V[i] > d[i]);
                d[i] -= V[i];
            }
            if (cut)
                d[32]--;
        }
        
        while (d[32] != 0) // mod n
        {
            x = 0;
            for (i = 0; i < 32; i++)
            {
                x += N[i];
                x += d[i];
                d[i] = x % 256;
                x = x >> 8;
            }
            d[32] += x;
        }
    }

    for (i = 0; i < 32; i++)
    {
        d[32 + i] = d[i];
        d[i] = 0;
    }

    internal_ModExp(S, d, p, 41);
}

// S = (A*V^U)^B mod N
void MontgomeryExp::ModExpAnother(uint8* S, uint8* A, uint8* V, uint8* U, uint8* B)
{
    uint8 vr[65], ar[65], VM[32], AM[32];
    bool started = false;

    // convert v to montgomery
    memset(vr, 0x00, 65);
    memcpy(vr + 32, V, 32);
    modN(VM, vr);
    // S = v^u
    for (uint16 i = 0; i<(20 * 8); i++)
    {
        if ((U[(20 - 1) - i / 8] >> (7 - i % 8)) & 0x01)
        {
            if (!started)
            {
                memcpy(S, VM, 32);
                started = true;
                continue;
            }
            step(S, S, S);
            step(S, S, VM);
            continue;
        }
        else if (started)
            step(S, S, S);
    }

    // convert A to montgomery
    memset(ar, 0x00, 65);
    memcpy(ar + 32, A, 32);
    modN(AM, ar);
    
    // VM = multiply
    step(VM, S, AM);

    started = false;
    // S = VM^B
    for (uint16 i = 0; i<(20 * 8); i++)
    {
        if ((B[(20 - 1) - i / 8] >> (7 - i % 8)) & 0x01)
        {
            if (!started)
            {
                memcpy(S, VM, 32);
                started = true;
                continue;
            }
            step(S, S, S);
            step(S, S, VM);
            continue;
        }
        else if (started)
            step(S, S, S);
    }
    
    // de-montgomerize
    memset(ar, 0x00, 65);
    for (uint8 i = 0; i < 32; i++)
    {
        uint16 x = 0;
        for (uint8 j = 0; j < 32; j++)
        {
            x += S[i] * Ri[j];
            x += ar[i + j];
            ar[i + j] = x % 256;
            x = x >> 8;
        }
        ar[i + 32] += x;
    }
    modN(S, ar);
}

void MontgomeryExp::internal_ModExp(uint8* S, uint8* A, uint8* P,uint8 size)
{
    uint8 I[32];
    bool started = false;

    modN(I, A);
    for (uint16 i = 0; i<(size * 8); i++)
    {
        if ((P[(size-1) - i / 8] >> (7 - i % 8)) & 0x01)
        {
            if (!started)
            {
                memcpy(S, I, 32);
                started = true;
                continue;
            }
            step(S, S, S);
            step(S, S, I);
            continue;
        }
        else if (started)
            step(S, S, S);
    }

    memset(A, 0x00, 65);
    for (uint8 i = 0; i < 32; i++)
    {
        uint16 x = 0;
        for (uint8 j = 0; j < 32; j++)
        {
            x += S[i] * Ri[j];
            x += A[i + j];
            A[i + j] = x % 256;
            x = x >> 8;
        }
        A[i + 32] += x;
    }
    modN(S, A);
}

void MontgomeryExp::step(uint8* S, uint8* A,uint8* B)
{
    uint8 T[65],L[32];
    uint8 i, j;
    uint32 x;
    bool cut = false;

    // T=AB
    memset(T, 0x00, 65);
    for (i = 0; i < 32; i++)
    {
        x = 0;
        for (j = 0; j < 32; j++)
        {
            x += A[i] * B[j];
            x += T[i + j];
            T[i + j] = x % 256;
            x = x >> 8;
        }
        T[i + 32] += x;
    }
    
    // L=TNi mod R
    memset(L, 0x00, 32);
    for (i = 0; i < 32; i++)
    {
        x = 0;
        for (j = 0; j < 32-i; j++)
        {
            x += T[i] * Ni[j];
            x += L[i + j];
            L[i + j] = x % 256;
            x = x >> 8;
        }
    }

    // T= T+LN
    for (i = 0; i < 32; i++)
    {
        x = 0;
        for (j = 0; j < 32; j++)
        {
            x += L[i] * N[j];
            x += T[i + j];
            T[i + j] = x % 256;
            x = x >> 8;
        }
        while (x)
        {
            x += T[i + j];
            T[i + j] = x % 256;
            x = x >> 8;
            j++;
        }
    }

    // T = T/r +-N
    if (T[64])
        cut = true;
    else
    {
        cut = true;
        for (i = 0; i < 32; i++)
        {
            if (T[63 - i] < N[31 - i])
                cut = false;
            if (T[63 - i] != N[31 - i])
                break;
        }
    }

    for (i = 0; i < 32; i++)
        S[i] = T[i + 32];
    if (!cut)
        return;
    cut = false;
    for (i = 0; i < 32; i++)
    {
        if (cut && S[i] == 0)
        {
            S[i]--;
            S[i] -= N[i];
            continue;
        }
        else if (cut)
            S[i]--;

        cut = (N[i] > S[i]);
        S[i] -= N[i];
    }
    
}

void MontgomeryExp::modN(uint8* S, uint8* B)
{
    // a is 65 bytes, last set to 0
    volatile uint8 i, j, cut; uint8 A[65];
    uint16 x,y;
    bool repeat = false,forward = false;
    memcpy(A, B, 65);

    for (i = 0; i < 33;)
    {
        repeat = false;
        x = A[64-i] << 8;
        x += A[63 - i];
        if (x < N[31])
        {
            i++;
            continue;
        }
        if (x == N[31])
        {
            cut = 1;
            for (j = 1; j < 32; j++)
            {
                if (A[63 - i - j] < N[31 - j])
                    cut = 0;
                if (A[63 - i - j] != N[31 - j])
                    break;
            }
        }
        else // x > N[31]
        {
            cut = x / (N[31] + 1);
            if (cut * N[31] < x)
                repeat = true;
        }

        for (j = 0; j < 32; j++)
        {
            x = A[32 - i + j];
            x += A[33 - i + j] << 8;
            y = N[j] * cut;
            if (forward) y+= 256;

            forward = (x < y);
            x -= y;
            
            A[32 - i + j] = x % (1 << 8);
            A[33 - i + j] = x >> 8;
        }

        if (!repeat)
            i++;
    }
    memcpy(S, A, 32);
}
