#pragma once 

#include "Platform/Assert.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

/* -*- mode: c; c-file-style: "k&r" -*-

strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

// NOTE: This file has been modified from its original version.  The separate 
// source file has been combined into this header.  Dowloaded from:
// http://sourcefrog.net/projects/natsort/

/*
* CUSTOMIZATION SECTION
*
* You can change this typedef, but must then also change the inline
* functions in strnatcmp.c
*
*/

typedef tchar nat_char;


static inline int
nat_isdigit(nat_char a)
{
    return isdigit((tchar) a);
}


static inline int
nat_isspace(nat_char a)
{
    return isspace((tchar) a);
}


static inline nat_char
nat_toupper(nat_char a)
{
    return toupper((tchar) a);
}



static int
compare_right(nat_char const *a, nat_char const *b)
{
    int bias = 0;

    /* The longest run of digits wins.  That aside, the greatest
    value wins, but we can't know that it will until we've scanned
    both numbers to know that they have the same magnitude, so we
    remember it in BIAS. */
    for (;; a++, b++) {
        if (!nat_isdigit(*a)  &&  !nat_isdigit(*b))
            return bias;
        else if (!nat_isdigit(*a))
            return -1;
        else if (!nat_isdigit(*b))
            return +1;
        else if (*a < *b) {
            if (!bias)
                bias = -1;
        } else if (*a > *b) {
            if (!bias)
                bias = +1;
        } else if (!*a  &&  !*b)
            return bias;
    }

    return 0;
}


static int
compare_left(nat_char const *a, nat_char const *b)
{
    /* Compare two left-aligned numbers: the first to have a
    different value wins. */
    for (;; a++, b++) {
        if (!nat_isdigit(*a)  &&  !nat_isdigit(*b))
            return 0;
        else if (!nat_isdigit(*a))
            return -1;
        else if (!nat_isdigit(*b))
            return +1;
        else if (*a < *b)
            return -1;
        else if (*a > *b)
            return +1;
    }

    return 0;
}


static int strnatcmp0(nat_char const *a, nat_char const *b, int fold_case)
{
    int ai, bi;
    nat_char ca, cb;
    int fractional, result;

    NOC_ASSERT(a && b);
    ai = bi = 0;
    while (1) {
        ca = a[ai]; cb = b[bi];

        /* skip over leading spaces or zeros */
        while (nat_isspace(ca))
            ca = a[++ai];

        while (nat_isspace(cb))
            cb = b[++bi];

        /* process run of digits */
        if (nat_isdigit(ca)  &&  nat_isdigit(cb)) {
            fractional = (ca == '0' || cb == '0');

            if (fractional) {
                if ((result = compare_left(a+ai, b+bi)) != 0)
                    return result;
            } else {
                if ((result = compare_right(a+ai, b+bi)) != 0)
                    return result;
            }
        }

        if (!ca && !cb) {
            /* The strings compare the same.  Perhaps the caller
            will want to call strcmp to break the tie. */
            return 0;
        }

        if (fold_case) {
            ca = nat_toupper(ca);
            cb = nat_toupper(cb);
        }

        if (ca < cb)
            return -1;
        else if (ca > cb)
            return +1;

        ++ai; ++bi;
    }
}

/* Compare, recognizing numeric string and regarding case. */
inline int strnatcmp(nat_char const *a, nat_char const *b)
{
    return strnatcmp0(a, b, 0);
}

/* Compare, recognizing numeric string and ignoring case. */
inline int strinatcmp(nat_char const *a, nat_char const *b)
{
    return strnatcmp0(a, b, 1);
}

#ifdef __cplusplus

#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Comparison algorithm for strings sorted using case insensitive natural 
// ordering.  See strnatcmp.h for more information.
// 
struct CaseInsensitiveNatStrCmp : public std::binary_function< tstring, tstring, bool >
{
    bool operator( )( const tstring& str1, const tstring& str2 ) const
    {
        return ( strinatcmp( str1.c_str(), str2.c_str() ) < 0 );
    }
};


///////////////////////////////////////////////////////////////////////////////
// Comparison algorithm for strings sorted using case sensitive natural 
// ordering.  See strnatcmp.h for more information.
// 
struct CaseSensitiveNatStrCmp : public std::binary_function< tstring, tstring, bool >
{
    bool operator( )( const tstring& str1, const tstring& str2 ) const
    {
        return ( strnatcmp( str1.c_str(), str2.c_str() ) < 0 );
    }
};

#endif