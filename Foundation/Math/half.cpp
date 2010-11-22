#include "half.h"

using namespace Helium;

//-------------------------------------------------------------
// Lookup tables for half-to-float32_t and float32_t-to-half conversion
//-------------------------------------------------------------
const half::uif half::_toFloat[1 << 16] =
#include "Halftofloat.h"

const uint16_t half::_eLut[1 << 9] =
{
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,  1024,  2048,  3072,  4096,  5120,  6144,  7168, 
    8192,  9216, 10240, 11264, 12288, 13312, 14336, 15360, 
    16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552, 
    24576, 25600, 26624, 27648, 28672, 29696,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0, 33792, 34816, 35840, 36864, 37888, 38912, 39936, 
    40960, 41984, 43008, 44032, 45056, 46080, 47104, 48128, 
    49152, 50176, 51200, 52224, 53248, 54272, 55296, 56320, 
    57344, 58368, 59392, 60416, 61440, 62464,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0,     0,     0,     0, 
};

//-----------------------------------------------
// Overflow handler for float32_t-to-half conversion;
// generates a hardware floating-point overflow,
// which may be trapped by the operating system.
//-----------------------------------------------
float32_t half::overflow()
{
    volatile float32_t f = 1e10;

    for (int32_t i = 0; i < 10; i++)	
        f *= f;				// this will overflow before the for­loop terminates
    return f;
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------
int16_t half::convert(int32_t i)
{
    //
    // Our floating point number, f, is represented by the bit
    // pattern in integer i.  Disassemble that bit pattern into
    // the sign, s, the exponent, e, and the significand, m.
    // Shift s into the position where it will go in in the
    // resulting half number.
    // Adjust e, accounting for the different exponent bias
    // of float32_t and half (127 versus 15).
    //

    int32_t s =  (i >> 16) & 0x00008000;
    int32_t e = ((i >> 23) & 0x000000ff) - (127 - 15);
    int32_t m =   i        & 0x007fffff;

    //
    // Now reassemble s, e and m into a half:
    //

    if (e <= 0)
    {
        if (e < -10)
        {
            //
            // E is less than -10.  The absolute value of f is
            // less than HALF_MIN (f may be a small normalized
            // float32_t, a denormalized float32_t or a zero).
            //
            // We convert f to a half zero.
            //

            return 0;
        }

        //
        // E is between -10 and 0.  F is a normalized float32_t,
        // whose magnitude is less than HALF_NRM_MIN.
        //
        // We convert f to a denormalized half.
        // 

        m = (m | 0x00800000) >> (1 - e);

        //
        // Round to nearest, round "0.5" up.
        //
        // Rounding may cause the significand to overflow and make
        // our number normalized.  Because of the way a half's bits
        // are laid out, we don't have to treat this case separately;
        // the code below will handle it correctly.
        // 

        if (m &  0x00001000)
            m += 0x00002000;

        //
        // Assemble the half from s, e (zero) and m.
        //

        return s | (m >> 13);
    }
    else if (e == 0xff - (127 - 15))
    {
        if (m == 0)
        {
            //
            // F is an infinity; convert f to a half
            // infinity with the same sign as f.
            //

            return s | 0x7c00;
        }
        else
        {
            //
            // F is a NAN; produce a half NAN that preserves
            // the sign bit and the 10 leftmost bits of the
            // significand of f.
            //

            return s | 0x7c00 | (m >> 13);
        }
    }
    else
    {
        //
        // E is greater than zero.  F is a normalized float32_t.
        // We try to convert f to a normalized half.
        //

        //
        // Round to nearest, round "0.5" up
        //

        if (m &  0x00001000)
        {
            m += 0x00002000;

            if (m & 0x00800000)
            {
                m =  0;		// overflow in significand,
                e += 1;		// adjust exponent
            }
        }

        //
        // Handle exponent overflow
        //

        if (e > 30)
        {
            overflow ();	// Cause a hardware floating point overflow;
            return s | 0x7c00;	// if this returns, the half becomes an
        }   			// infinity with the same sign as f.

        //
        // Assemble the half from s, e and m.
        //

        return s | (e << 10) | (m >> 13);
    }
}

//---------------------------------------
// Functions to print the bit-layout of
// floats and Halfs, mostly for debugging
//---------------------------------------
void half::createBitString(tchar_t c[19], half h)
{
    uint16_t b = h.bits();

    for (int32_t i = 15, j = 0; i >= 0; i--, j++)
    {
        c[j] = (((b >> i) & 1)? '1': '0');
        if (i == 15 || i == 10)
            c[++j] = ' ';
    }    
    c[18] = 0;
}


void half::createBitString(tchar_t c[35], float32_t f)
{
    half::uif x;
    x.f = f;
    for (int32_t i = 31, j = 0; i >= 0; i--, j++)
    {
        c[j] = (((x.i >> i) & 1)? '1': '0');

        if (i == 31 || i == 23)
            c[++j] = ' ';
    }
    c[34] = 0;
}
