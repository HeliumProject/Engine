#pragma once

/*
--------------------------------------------------------------------
lookup8.c, by Bob Jenkins, January 4 1997, Public Domain.
hash(), hash2(), hash3, and mix() are externally useful functions.
Routines to test the hash are included if SELF_TEST is defined.
You can use this free for any purpose.  It has no warranty.
--------------------------------------------------------------------
*/

#include "Platform/Types.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define hashsize(n) ((u64)1<<(n))
#define hashmask(n) (hashsize(n)-1)

/*
--------------------------------------------------------------------
mix -- mix 3 64-bit values reversibly.
mix() takes 48 machine instructions, but only 24 cycles on a superscalar
machine (like Intel's new MMX architecture).  It requires 4 64-bit
registers for 4::2 parallelism.
All 1-bit deltas, all 2-bit deltas, all deltas composed of top bits of
(a,b,c), and all deltas of bottom bits were tested.  All deltas were
tested both on random keys and on keys that were nearly all zero.
These deltas all cause every bit of c to change between 1/3 and 2/3
of the time (well, only 113/400 to 287/400 of the time for some
2-bit delta).  These deltas all cause at least 80 bits to change
among (a,b,c) when the mix is run either forward or backward (yes it
is reversible).
This implies that a hash using mix64 has no funnels.  There may be
characteristics with 3-bit deltas or bigger, I didn't test for
those.
--------------------------------------------------------------------
*/
#define mix64(a,b,c) \
{ \
    a -= b; a -= c; a ^= (c>>43); \
    b -= c; b -= a; b ^= (a<<9); \
    c -= a; c -= b; c ^= (b>>8); \
    a -= b; a -= c; a ^= (c>>38); \
    b -= c; b -= a; b ^= (a<<23); \
    c -= a; c -= b; c ^= (b>>5); \
    a -= b; a -= c; a ^= (c>>35); \
    b -= c; b -= a; b ^= (a<<49); \
    c -= a; c -= b; c ^= (b>>11); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<18); \
    c -= a; c -= b; c ^= (b>>22); \
}

/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 64-bit value
k     : the key (the unaligned variable-length array of bytes)
len   : the length of the key, counting by bytes
level : can be any 8-byte value
Returns a 64-bit value.  Every bit of the key affects every bit of
the return value.  No funnels.  Every 1-bit and 2-bit delta achieves
avalanche.  About 41+5len instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 64 bits,
use a bitmask.  For example, if you need only 10 bits, do
h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (u8 **)k, do it like this:
for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, Jan 4 1997.  bob_jenkins@burtleburtle.net.  You may
use this code any way you wish, private, educational, or commercial,
but I would appreciate if you give me credit.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^64
is acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------
This is identical to hash() on little-endian machines, and it is much
faster than hash(), but a little slower than hash2(), and it requires
-- that all your machines be little-endian, for example all Intel x86
chips or all VAXen.  It gives wrong results on big-endian machines.
--------------------------------------------------------------------
*/

// key = the stream of input bytes
// length = length of the stream of input bytes
// level = the previous hash or any arbitrary value
inline u64 hash64( u8* k, u32 length, u64 level)
{
    register u64 a,b,c,len;

    /* Set up the internal state */
    len = length;
    a = b = level;                         /* the previous hash value */
    c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

    /*---------------------------------------- handle most of the key */
    if (((size_t)k)&7)
    {
        while (len >= 24)
        {
            a += (k[0]        +((u64)k[ 1]<< 8)+((u64)k[ 2]<<16)+((u64)k[ 3]<<24)
                +((u64)k[4 ]<<32)+((u64)k[ 5]<<40)+((u64)k[ 6]<<48)+((u64)k[ 7]<<56));
            b += (k[8]        +((u64)k[ 9]<< 8)+((u64)k[10]<<16)+((u64)k[11]<<24)
                +((u64)k[12]<<32)+((u64)k[13]<<40)+((u64)k[14]<<48)+((u64)k[15]<<56));
            c += (k[16]       +((u64)k[17]<< 8)+((u64)k[18]<<16)+((u64)k[19]<<24)
                +((u64)k[20]<<32)+((u64)k[21]<<40)+((u64)k[22]<<48)+((u64)k[23]<<56));
            mix64(a,b,c);
            k += 24; len -= 24;
        }
    }
    else
    {
        while (len >= 24)    /* aligned */
        {
            a += *(u64 *)(k+0);
            b += *(u64 *)(k+8);
            c += *(u64 *)(k+16);
            mix64(a,b,c);
            k += 24; len -= 24;
        }
    }

    /*------------------------------------- handle the last 23 bytes */
    c += length;
    switch(len)              /* all the case statements fall through */
    {
    case 23: c+=((u64)k[22]<<56);
    case 22: c+=((u64)k[21]<<48);
    case 21: c+=((u64)k[20]<<40);
    case 20: c+=((u64)k[19]<<32);
    case 19: c+=((u64)k[18]<<24);
    case 18: c+=((u64)k[17]<<16);
    case 17: c+=((u64)k[16]<<8);
        /* the first byte of c is reserved for the length */
    case 16: b+=((u64)k[15]<<56);
    case 15: b+=((u64)k[14]<<48);
    case 14: b+=((u64)k[13]<<40);
    case 13: b+=((u64)k[12]<<32);
    case 12: b+=((u64)k[11]<<24);
    case 11: b+=((u64)k[10]<<16);
    case 10: b+=((u64)k[ 9]<<8);
    case  9: b+=((u64)k[ 8]);
    case  8: a+=((u64)k[ 7]<<56);
    case  7: a+=((u64)k[ 6]<<48);
    case  6: a+=((u64)k[ 5]<<40);
    case  5: a+=((u64)k[ 4]<<32);
    case  4: a+=((u64)k[ 3]<<24);
    case  3: a+=((u64)k[ 2]<<16);
    case  2: a+=((u64)k[ 1]<<8);
    case  1: a+=((u64)k[ 0]);
        /* case 0: nothing left to add */
    }
    mix64(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  hash64()
//
//  written by: afh 
//
//  - returns a 64-bit hash value as a 16-character string in hex format
//
////////////////////////////////////////////////////////////////////////////////////////////////

inline tstring hash64( tstring in )
{
    u8* bytes = (u8*)&(in[0]);

    u64 hash_val = hash64( bytes, (u32)in.size(), 0x123456789ABCDEFL );

    tstring ret;
    for (u32 i = 0; i < 16; i++)
    {
        u64 digit = (hash_val >> (i * 4)) & 0xF;

        tchar buf[2];
        _stprintf( buf, "%x", digit);
        ret += buf;
    }

    return ret;
}