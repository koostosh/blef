#pragma once
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// Copyright (C) 2000 - 2003, Richard J. Wagner
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. The names of its contributors may not be used to endorse or promote
//      products derived from this software without specific prior written
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Defines.h"

#include <limits.h>
#include <time.h>
#include <math.h>

class MTRand {
// Data
public:
    enum { N = 624 };       // length of state vector
    enum { SAVE = N + 1 };  // length of array for save()

protected:
    enum { M = 397 };  // period parameter

    uint32 state[N];   // internal state
    uint32 *pNext;     // next value to get from state
    int left;          // number of values left before reload needed


//Methods
public:
    MTRand();                         // auto-initialize with /dev/urandom or time() and clock()
    uint32 randInt();                       // integer in [0,2^32-1]
    uint32 randInt( const uint32& n );      // integer in [0,n] for n < 2^32
protected:
    void initialize( const uint32 oneSeed );
    void reload();
    uint32 hiBit( const uint32& u ) const { return u & 0x80000000UL; }
    uint32 loBit( const uint32& u ) const { return u & 0x00000001UL; }
    uint32 loBits( const uint32& u ) const { return u & 0x7fffffffUL; }
    uint32 mixBits( const uint32& u, const uint32& v ) const
        { return hiBit(u) | loBits(v); }
    uint32 twist( const uint32& m, const uint32& s0, const uint32& s1 ) const
        { return m ^ (mixBits(s0,s1)>>1) ^ uint32(-(int32)(loBit(s1) & 0x9908b0dfUL)); }
    static uint32 hash( time_t t, clock_t c );
};


inline MTRand::MTRand()
{
    initialize(hash(time(NULL), 2135));
    reload();
}

inline uint32 MTRand::randInt()
{
    // Pull a 32-bit integer from the generator state
    // Every other access function simply transforms the numbers extracted here

    if( left == 0 ) reload();
    --left;

    register uint32 s1;
    s1 = *pNext++;
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9d2c5680UL;
    s1 ^= (s1 << 15) & 0xefc60000UL;
    return ( s1 ^ (s1 >> 18) );
}

inline uint32 MTRand::randInt( const uint32& n )
{
    // Find which bits are used in n
    // Optimized by Magnus Jonsson (magnus@smartelectronix.com)
    uint32 used = n;
    used |= used >> 1;
    used |= used >> 2;
    used |= used >> 4;
    used |= used >> 8;
    used |= used >> 16;

    // Draw numbers until one is found in [0,n]
    uint32 i;
    do
        i = randInt() & used;  // toss unused bits to shorten search
    while( i > n );
    return i;
}

inline void MTRand::initialize( const uint32 seed )
{
    // Initialize generator state with seed
    // See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
    // In previous versions, most significant bits (MSBs) of the seed affect
    // only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
    register uint32 *s = state;
    register uint32 *r = state;
    register int i = 1;
    *s++ = seed & 0xffffffffUL;
    for( ; i < N; ++i )
    {
        *s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
        r++;
    }
}


inline void MTRand::reload()
{
    // Generate N new values in state
    // Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
    register uint32 *p = state;
    register int i;
    for( i = N - M; i--; ++p )
        *p = twist( p[M], p[0], p[1] );
    for( i = M; --i; ++p )
        *p = twist( p[M-N], p[0], p[1] );
    *p = twist( p[M-N], p[0], state[0] );

    left = N, pNext = state;
}


inline uint32 MTRand::hash( time_t t, clock_t c )
{
    // Get a uint32 from t and c
    // Better than uint32(x) in case x is floating point in [0,1]
    // Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

    static uint32 differ = 0;  // guarantee time-based seeds will change

    uint32 h1 = 0;
    unsigned char *p = (unsigned char *) &t;
    for( size_t i = 0; i < sizeof(t); ++i )
    {
        h1 *= UCHAR_MAX + 2U;
        h1 += p[i];
    }
    uint32 h2 = 0;
    p = (unsigned char *) &c;
    for( size_t j = 0; j < sizeof(c); ++j )
    {
        h2 *= UCHAR_MAX + 2U;
        h2 += p[j];
    }
    return ( h1 + differ++ ) ^ h2;
}
