/**
*** Copyright (C) 2009 Intel Corporation.  All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
**/
#ifndef _LRB_PROTOTYPE_PRIMITIVES_INL
#define _LRB_PROTOTYPE_PRIMITIVES_INL

#include <xmmintrin.h>
#include <emmintrin.h>
#include <float.h>
#include <math.h>
#include <assert.h>


#ifndef USE_C_PROTOTYPE_PRIMITIVES
   #define USE_C_PROTOTYPE_PRIMITIVES 1
#endif

#if defined (__GNUC__)
#define _hypot hypot
#define _hypotf hypotf
#endif


#if defined(__INTEL_COMPILER) || defined(__GNUC__)

/* Contains definitions for uint64_t */
#include <stdint.h>

#else

typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;

#endif

#if defined(_WIN64)

#if defined(__QMSPP_)

typedef struct __declspec(align(64)) {
    float v[16];
} __m512;

typedef struct __declspec(align(64)) {
    double v[8];
} __m512d;

typedef struct __declspec(align(64)) {
    int v[16];
} __m512i;

#else

typedef __declspec(align(16)) struct {
    float v[16];
} __m512;

typedef __declspec(align(16)) struct {
    double v[8];
} __m512d;

typedef __declspec(align(16)) struct {
    int v[16];
} __m512i;

#endif

#elif defined(__GNUC__)

typedef struct {
float v[16];
} __attribute__((__may_alias__)) __m512;

typedef struct {
double v[8];
} __attribute__((__may_alias__)) __m512d;

typedef struct {
int v[16];
} __attribute__((__may_alias__)) __m512i;

#else

typedef struct {
    float v[16];
} __m512;

typedef struct {
    double v[8];
} __m512d;

typedef struct {
    int v[16];
} __m512i;

#endif

#define _M512I __m512i
#define _M512D __m512d
#define _M512  __m512

typedef unsigned short __mmask;


/* Constant for special read-only mask register 'k0' */
#define _MM_K0_REG (0xffff)

typedef enum {
    _MM_SWIZ_REG_NONE = 0x00,   /* hgfe dcba - Nop */
    _MM_SWIZ_REG_DCBA = 0x00,   /* hgfe dcba - Nop */
    _MM_SWIZ_REG_CDAB = 0x01,   /* ghef cdab - Swap pairs */
    _MM_SWIZ_REG_BADC = 0x02,   /* fehg badc - Swap with two-away */
    _MM_SWIZ_REG_AAAA = 0x03,   /* eeee aaaa - broadcast a element */
    _MM_SWIZ_REG_BBBB = 0x04,   /* ffff bbbb - broadcast b element */
    _MM_SWIZ_REG_CCCC = 0x05,   /* gggg cccc - broadcast c element */
    _MM_SWIZ_REG_DDDD = 0x06,   /* hhhh dddd - broadcast d element */
    _MM_SWIZ_REG_DACB = 0x07    /* hegf dacb - cross-product */
} _MM_SWIZZLE_ENUM;

/* Constants for 32-bit broadcast swizzle primitives */
typedef enum {
    _MM_BROADCAST_16X16    = 0x00,  /* identity swizzle/convert */
    _MM_BROADCAST_1X16     = 0x01,  /* broadcast x 16 ( aaaa aaaa aaaa aaaa ) */
    _MM_BROADCAST_4X16     = 0x02   /* broadcast x 4  ( dcba dcba dcba dcba ) */
} _MM_BROADCAST32_ENUM;

/* Constants for 64-bit broadcast swizzle primitives */
typedef enum {
    _MM_BROADCAST_8X8     = 0x00,   /* identity swizzle/convert */
    _MM_BROADCAST_1X8     = 0x01,   /* broadcast x 8 ( aaaa aaaa ) */
    _MM_BROADCAST_4X8     = 0x02    /* broadcast x 2 ( dcba dcba ) */
} _MM_BROADCAST64_ENUM;

/* Constants for float32 SwizUpConv conversion primitives */
typedef enum {
    _MM_16X16_F32   = 0x00,   /* identity swizzle/convert */
    _MM_1X16_F32    = 0x01,   /* broadcast x 16 ( aaaa aaaa aaaa aaaa ) */
    _MM_4X16_F32    = 0x02,   /* broadcast x 4  ( dcba dcba dcba dcba ) */
    _MM_UI8_TO_F32  = 0x04,   /* 16 x uint8   => 16 x fp32 */
    _MM_UN8_TO_F32  = 0x05,   /* 16 x unorm8  => 16 x fp32 */
    _MM_F16_TO_F32  = 0x06,   /* 16 x float16 => 16 x fp32 */
    _MM_SI16_TO_F32 = 0x07    /* 16 x sint16  => 16 x fp32 */
} _MM_UPCONV_F32_ENUM;

/* Constants for int32 SwizUpConv conversion primitives */
typedef enum {
    _MM_16X16_I32   = 0x00,   /* identity swizzle/convert */
    _MM_1X16_I32    = 0x01,   /* broadcast x 16 ( aaaa aaaa aaaa aaaa ) */
    _MM_4X16_I32    = 0x02,   /* broadcast x 4  ( dcba dcba dcba dcba ) */
    _MM_UI8_TO_I32  = 0x04,   /* 16 x uint8   => 16 x uint32 */
    _MM_SI8_TO_I32  = 0x05,   /* 16 x sint8   => 16 x sint32 */
    _MM_UI16_TO_I32 = 0x06,   /* 16 x uint16  => 16 x uint32 */
    _MM_SI16_TO_I32 = 0x07    /* 16 x sint16  => 16 x sint32 */
} _MM_UPCONV_I32_ENUM;

/* Constants for float64 SwizUpConv conversion primitives */
typedef enum {
    _MM_8X8_F64     = 0x00,   /* identity swizzle/convert */
    _MM_1X8_F64     = 0x01,   /* broadcast x 8 ( aaaa aaaa ) */
    _MM_4X8_F64     = 0x02    /* broadcast x 4  ( dcba dcba ) */
} _MM_UPCONV_F64_ENUM;

/* Constants for int64 SwizUpConv conversion primitives */
typedef enum {
    _MM_8X8_I64     = 0x00,   /* identity swizzle/convert */
    _MM_1X8_I64     = 0x01,   /* broadcast x 8 ( aaaa aaaa ) */
    _MM_4X8_I64     = 0x02    /* broadcast x 4  ( dcba dcba ) */
} _MM_UPCONV_I64_ENUM;

/* Constants for 32-bit FullUpConv conversion vload primitives */
typedef enum {
    _MM_FULLUPC_NONE     = 0x00,   /* no conversion      */
    _MM_FULLUPC_FLOAT16  = 0x01,   /* float16 => float32 */
    _MM_FULLUPC_SRGB8    = 0x03,   /* srgb8   => float32 */
    _MM_FULLUPC_UINT8    = 0x04,   /* uint8   => float32 */
    _MM_FULLUPC_SINT8    = 0x05,   /* sint8   => float32 */
    _MM_FULLUPC_UNORM8   = 0x06,   /* unorm8  => float32 */
    _MM_FULLUPC_SNORM8   = 0x07,   /* snorm8  => float32 */
    _MM_FULLUPC_UINT16   = 0x08,   /* uint16  => float32 */
    _MM_FULLUPC_SINT16   = 0x09,   /* sint16  => float32 */
    _MM_FULLUPC_UNORM16  = 0x0A,   /* unorm16 => float32 */
    _MM_FULLUPC_SNORM16  = 0x0B,   /* snorm16 => float32 */
    _MM_FULLUPC_UINT8I   = 0x0C,   /* uint8   => uint32  */
    _MM_FULLUPC_SINT8I   = 0x0D,   /* sint8   => sint32  */
    _MM_FULLUPC_UINT16I  = 0x0E,   /* uint16  => uint32  */
    _MM_FULLUPC_SINT16I  = 0x0F,   /* sint16  => sint32  */
    _MM_FULLUPC_UNORM10A = 0x10,   /* unorm10A10B10C2D field A => float32 */
    _MM_FULLUPC_UNORM10B = 0x11,   /* unorm10A10B10C2D field B => float32 */
    _MM_FULLUPC_UNORM10C = 0x12,   /* unorm10A10B10C2D field C => float32 */
    _MM_FULLUPC_UNORM2D  = 0x13,   /* unorm10A10B10C2D field D => float32 */
    _MM_FULLUPC_FLOAT11A = 0x14,   /* float11A11B10C field A   => float32 */
    _MM_FULLUPC_FLOAT11B = 0x15,   /* float11A11B10C field B   => float32 */
    _MM_FULLUPC_FLOAT10C = 0x16    /* float11A11B10C field C   => float32 */
} _MM_FULLUP32_ENUM;

/* Constants for 64-bit FullUpConv conversion vload primitives */
typedef enum {
    _MM_FULLUPC64_NONE   = 0x00   /* no conversion      */
} _MM_FULLUP64_ENUM;

/* Constants for 32-bit DownConv conversion vstore primitives */
typedef enum {
    _MM_DOWNC_NONE       = 0x00,   /* no conversion      */
    _MM_DOWNC_FLOAT16    = 0x01,   /* float32 => float16 */
    _MM_DOWNC_FLOAT16RZ  = 0x02,   /* float32 => float16 (round to zero) */
    _MM_DOWNC_UINT8      = 0x04,   /* float32 => uint8   */
    _MM_DOWNC_SINT8      = 0x05,   /* float32 => sint8   */
    _MM_DOWNC_UNORM8     = 0x06,   /* float32 => unorm8  */
    _MM_DOWNC_SNORM8     = 0x07,   /* float32 => snorm8  */
    _MM_DOWNC_UINT16     = 0x08,   /* float32 => uint16  */
    _MM_DOWNC_SINT16     = 0x09,   /* float32 => sint16  */
    _MM_DOWNC_UNORM16    = 0x0A,   /* float32 => unorm16 */
    _MM_DOWNC_SNORM16    = 0x0B,   /* float32 => snorm16 */
    _MM_DOWNC_UINT8I     = 0x0C,   /* uint32  => uint8   */
    _MM_DOWNC_SINT8I     = 0x0D,   /* sint32  => sint8   */
    _MM_DOWNC_UINT16I    = 0x0E,   /* uint32  => uint16  */
    _MM_DOWNC_SINT16I    = 0x0F,   /* sint32  => sint16  */
} _MM_DOWNCONV32_ENUM;

/* Constants for 64-bit DownConv conversion vstore primitives */
typedef enum {
    _MM_DOWNC64_NONE     = 0x00    /* no conversion      */
} _MM_DOWNCONV64_ENUM;

/* Constants for vstore Subset primitives */
typedef enum {
    _MM_SUBSET32_16      = 0x0,    /* store 16 elements (ponm lkji hgfe dcba) */
    _MM_SUBSET32_1       = 0x1,    /* store  1 element  (a)    */
    _MM_SUBSET32_4       = 0x2     /* store  4 elements (dcba) */
} _MM_STORE_SUBSET32_ENUM;

typedef enum {
    _MM_SUBSET64_8       = 0x0,    /* store 8 64-bit elements  (hgfe dcba) */
    _MM_SUBSET64_1       = 0x1,    /* store  1 element  (a)    */
    _MM_SUBSET64_4       = 0x2     /* store  4 elements (dcba) */
} _MM_STORE_SUBSET64_ENUM;

/* Constants for rounding mode */
typedef enum {
    _MM_ROUND_MODE_NEAREST     = 0x0,    /* round to nearest (even) */
    _MM_ROUND_MODE_DOWN        = 0x1,    /* round toward negative infinity */
    _MM_ROUND_MODE_UP          = 0x2,    /* round toward positive infinity */
    _MM_ROUND_MODE_TOWARD_ZERO = 0x3     /* round toward zero */
} _MM_ROUND_MODE_ENUM;

/* Constants for exponent adjustment */
typedef enum {
    _MM_EXPADJ_NONE      = 0x0,    /* 2**0  (32.0 - no exp adjustment) */
    _MM_EXPADJ_4         = 0x1,    /* 2**4  (28.4)  */
    _MM_EXPADJ_5         = 0x2,    /* 2**5  (27.5)  */
    _MM_EXPADJ_8         = 0x3,    /* 2**8  (24.8)  */
    _MM_EXPADJ_16        = 0x4,    /* 2**16 (16.16) */
    _MM_EXPADJ_24        = 0x5,    /* 2**24 (8.24)  */
    _MM_EXPADJ_31        = 0x6,    /* 2**31 (1.31)  */
    _MM_EXPADJ_32        = 0x7     /* 2**32 (0.32)  */
} _MM_EXP_ADJ_ENUM;

/* Constants for index scaling for gather and scatter */
typedef enum {
    _MM_SCALE_1 = 1,
    _MM_SCALE_2 = 2,
    _MM_SCALE_4 = 4
} _MM_INDEX_SCALE_ENUM;

/* Constants for cvtins_ps2u10 */
typedef enum {
    _MM_UNORM10A = 0,
    _MM_UNORM10B = 1,
    _MM_UNORM10C = 2,
    _MM_UNORM2D  = 3
} _MM_UNORM10_FIELD_ENUM;

/* Constants for cvtins_ps2f11 */
typedef enum {
    _MM_FLOAT11A    = 0,
    _MM_FLOAT11B    = 1,
    _MM_FLOAT10C    = 2,
    _MM_FLOAT11NONE = 3
} _MM_FLOAT11_FIELD_ENUM;

/* Constants for the bytemasked compares */
typedef enum {
    _MM_BMSK_00FFFFFF = 0,
    _MM_BMSK_000000FF = 1,
    _MM_BMSK_FFFFFF00 = 2,
    _MM_BMSK_FF000000 = 3
} _MM_BMSK_FIELD_ENUM;

/* Constants for load/store hints */
typedef enum {
    _MM_HINT_NONE = 0,
    _MM_HINT_NT   = 1
} _MM_MEM_HINT_ENUM;

/* Constants for use with _mm_vprefetch */
typedef enum {
    _MM_PFHINT_NONE        = 0x0,
    _MM_PFHINT_EX          = 0x1,
    _MM_PFHINT_NT          = 0x2,
    _MM_PFHINT_EX_NT       = 0x3,
    _MM_PFHINT_MISS        = 0x4,
    _MM_PFHINT_EX_MISS     = 0x5,
    _MM_PFHINT_NT_MISS     = 0x6,
    _MM_PFHINT_EX_NT_MISS  = 0x7
} _MM_PREFETCH_HINT_ENUM;

/* Constants for 16-bit insertfield and rotatefield */
typedef enum {
    _MM_BIT16_0  = 0,
    _MM_BIT16_1  = 1,
    _MM_BIT16_2  = 2,
    _MM_BIT16_3  = 3,
    _MM_BIT16_4  = 4,
    _MM_BIT16_5  = 5,
    _MM_BIT16_6  = 6,
    _MM_BIT16_7  = 7,
    _MM_BIT16_8  = 8,
    _MM_BIT16_9  = 9,
    _MM_BIT16_10 = 10,
    _MM_BIT16_11 = 11,
    _MM_BIT16_12 = 12,
    _MM_BIT16_13 = 13,
    _MM_BIT16_14 = 14,
    _MM_BIT16_15 = 15
} _MM_BITPOSITION16_ENUM;

/* Constants for 32-bit insertfield and rotatefield */
typedef enum {
    _MM_BIT32_0  = 0,
    _MM_BIT32_1  = 1,
    _MM_BIT32_2  = 2,
    _MM_BIT32_3  = 3,
    _MM_BIT32_4  = 4,
    _MM_BIT32_5  = 5,
    _MM_BIT32_6  = 6,
    _MM_BIT32_7  = 7,
    _MM_BIT32_8  = 8,
    _MM_BIT32_9  = 9,
    _MM_BIT32_10 = 10,
    _MM_BIT32_11 = 11,
    _MM_BIT32_12 = 12,
    _MM_BIT32_13 = 13,
    _MM_BIT32_14 = 14,
    _MM_BIT32_15 = 15,
    _MM_BIT32_16 = 16,
    _MM_BIT32_17 = 17,
    _MM_BIT32_18 = 18,
    _MM_BIT32_19 = 19,
    _MM_BIT32_20 = 20,
    _MM_BIT32_21 = 21,
    _MM_BIT32_22 = 22,
    _MM_BIT32_23 = 23,
    _MM_BIT32_24 = 24,
    _MM_BIT32_25 = 25,
    _MM_BIT32_26 = 26,
    _MM_BIT32_27 = 27,
    _MM_BIT32_28 = 28,
    _MM_BIT32_29 = 29,
    _MM_BIT32_30 = 30,
    _MM_BIT32_31 = 31
} _MM_BITPOSITION32_ENUM;

/* Constants for 64-bit insertfield and rotatefield */
typedef enum {
    _MM_BIT64_0  = 0,
    _MM_BIT64_1  = 1,
    _MM_BIT64_2  = 2,
    _MM_BIT64_3  = 3,
    _MM_BIT64_4  = 4,
    _MM_BIT64_5  = 5,
    _MM_BIT64_6  = 6,
    _MM_BIT64_7  = 7,
    _MM_BIT64_8  = 8,
    _MM_BIT64_9  = 9,
    _MM_BIT64_10 = 10,
    _MM_BIT64_11 = 11,
    _MM_BIT64_12 = 12,
    _MM_BIT64_13 = 13,
    _MM_BIT64_14 = 14,
    _MM_BIT64_15 = 15,
    _MM_BIT64_16 = 16,
    _MM_BIT64_17 = 17,
    _MM_BIT64_18 = 18,
    _MM_BIT64_19 = 19,
    _MM_BIT64_20 = 20,
    _MM_BIT64_21 = 21,
    _MM_BIT64_22 = 22,
    _MM_BIT64_23 = 23,
    _MM_BIT64_24 = 24,
    _MM_BIT64_25 = 25,
    _MM_BIT64_26 = 26,
    _MM_BIT64_27 = 27,
    _MM_BIT64_28 = 28,
    _MM_BIT64_29 = 29,
    _MM_BIT64_30 = 30,
    _MM_BIT64_31 = 31,
    _MM_BIT64_32 = 32,
    _MM_BIT64_33 = 33,
    _MM_BIT64_34 = 34,
    _MM_BIT64_35 = 35,
    _MM_BIT64_36 = 36,
    _MM_BIT64_37 = 37,
    _MM_BIT64_38 = 38,
    _MM_BIT64_39 = 39,
    _MM_BIT64_40 = 40,
    _MM_BIT64_41 = 41,
    _MM_BIT64_42 = 42,
    _MM_BIT64_43 = 43,
    _MM_BIT64_44 = 44,
    _MM_BIT64_45 = 45,
    _MM_BIT64_46 = 46,
    _MM_BIT64_47 = 47,
    _MM_BIT64_48 = 48,
    _MM_BIT64_49 = 49,
    _MM_BIT64_50 = 50,
    _MM_BIT64_51 = 51,
    _MM_BIT64_52 = 52,
    _MM_BIT64_53 = 53,
    _MM_BIT64_54 = 54,
    _MM_BIT64_55 = 55,
    _MM_BIT64_56 = 56,
    _MM_BIT64_57 = 57,
    _MM_BIT64_58 = 58,
    _MM_BIT64_59 = 59,
    _MM_BIT64_60 = 60,
    _MM_BIT64_61 = 61,
    _MM_BIT64_62 = 62,
    _MM_BIT64_63 = 63
} _MM_BITPOSITION64_ENUM;

/* Constants for permutation with shuf128x32 */
typedef enum {
    _MM_PERM_AAAA = 0x00, _MM_PERM_AAAB = 0x01, _MM_PERM_AAAC = 0x02,
    _MM_PERM_AAAD = 0x03, _MM_PERM_AABA = 0x04, _MM_PERM_AABB = 0x05,
    _MM_PERM_AABC = 0x06, _MM_PERM_AABD = 0x07, _MM_PERM_AACA = 0x08,
    _MM_PERM_AACB = 0x09, _MM_PERM_AACC = 0x0A, _MM_PERM_AACD = 0x0B,
    _MM_PERM_AADA = 0x0C, _MM_PERM_AADB = 0x0D, _MM_PERM_AADC = 0x0E,
    _MM_PERM_AADD = 0x0F, _MM_PERM_ABAA = 0x10, _MM_PERM_ABAB = 0x11,
    _MM_PERM_ABAC = 0x12, _MM_PERM_ABAD = 0x13, _MM_PERM_ABBA = 0x14,
    _MM_PERM_ABBB = 0x15, _MM_PERM_ABBC = 0x16, _MM_PERM_ABBD = 0x17,
    _MM_PERM_ABCA = 0x18, _MM_PERM_ABCB = 0x19, _MM_PERM_ABCC = 0x1A,
    _MM_PERM_ABCD = 0x1B, _MM_PERM_ABDA = 0x1C, _MM_PERM_ABDB = 0x1D,
    _MM_PERM_ABDC = 0x1E, _MM_PERM_ABDD = 0x1F, _MM_PERM_ACAA = 0x20,
    _MM_PERM_ACAB = 0x21, _MM_PERM_ACAC = 0x22, _MM_PERM_ACAD = 0x23,
    _MM_PERM_ACBA = 0x24, _MM_PERM_ACBB = 0x25, _MM_PERM_ACBC = 0x26,
    _MM_PERM_ACBD = 0x27, _MM_PERM_ACCA = 0x28, _MM_PERM_ACCB = 0x29,
    _MM_PERM_ACCC = 0x2A, _MM_PERM_ACCD = 0x2B, _MM_PERM_ACDA = 0x2C,
    _MM_PERM_ACDB = 0x2D, _MM_PERM_ACDC = 0x2E, _MM_PERM_ACDD = 0x2F,
    _MM_PERM_ADAA = 0x30, _MM_PERM_ADAB = 0x31, _MM_PERM_ADAC = 0x32,
    _MM_PERM_ADAD = 0x33, _MM_PERM_ADBA = 0x34, _MM_PERM_ADBB = 0x35,
    _MM_PERM_ADBC = 0x36, _MM_PERM_ADBD = 0x37, _MM_PERM_ADCA = 0x38,
    _MM_PERM_ADCB = 0x39, _MM_PERM_ADCC = 0x3A, _MM_PERM_ADCD = 0x3B,
    _MM_PERM_ADDA = 0x3C, _MM_PERM_ADDB = 0x3D, _MM_PERM_ADDC = 0x3E,
    _MM_PERM_ADDD = 0x3F, _MM_PERM_BAAA = 0x40, _MM_PERM_BAAB = 0x41,
    _MM_PERM_BAAC = 0x42, _MM_PERM_BAAD = 0x43, _MM_PERM_BABA = 0x44,
    _MM_PERM_BABB = 0x45, _MM_PERM_BABC = 0x46, _MM_PERM_BABD = 0x47,
    _MM_PERM_BACA = 0x48, _MM_PERM_BACB = 0x49, _MM_PERM_BACC = 0x4A,
    _MM_PERM_BACD = 0x4B, _MM_PERM_BADA = 0x4C, _MM_PERM_BADB = 0x4D,
    _MM_PERM_BADC = 0x4E, _MM_PERM_BADD = 0x4F, _MM_PERM_BBAA = 0x50,
    _MM_PERM_BBAB = 0x51, _MM_PERM_BBAC = 0x52, _MM_PERM_BBAD = 0x53,
    _MM_PERM_BBBA = 0x54, _MM_PERM_BBBB = 0x55, _MM_PERM_BBBC = 0x56,
    _MM_PERM_BBBD = 0x57, _MM_PERM_BBCA = 0x58, _MM_PERM_BBCB = 0x59,
    _MM_PERM_BBCC = 0x5A, _MM_PERM_BBCD = 0x5B, _MM_PERM_BBDA = 0x5C,
    _MM_PERM_BBDB = 0x5D, _MM_PERM_BBDC = 0x5E, _MM_PERM_BBDD = 0x5F,
    _MM_PERM_BCAA = 0x60, _MM_PERM_BCAB = 0x61, _MM_PERM_BCAC = 0x62,
    _MM_PERM_BCAD = 0x63, _MM_PERM_BCBA = 0x64, _MM_PERM_BCBB = 0x65,
    _MM_PERM_BCBC = 0x66, _MM_PERM_BCBD = 0x67, _MM_PERM_BCCA = 0x68,
    _MM_PERM_BCCB = 0x69, _MM_PERM_BCCC = 0x6A, _MM_PERM_BCCD = 0x6B,
    _MM_PERM_BCDA = 0x6C, _MM_PERM_BCDB = 0x6D, _MM_PERM_BCDC = 0x6E,
    _MM_PERM_BCDD = 0x6F, _MM_PERM_BDAA = 0x70, _MM_PERM_BDAB = 0x71,
    _MM_PERM_BDAC = 0x72, _MM_PERM_BDAD = 0x73, _MM_PERM_BDBA = 0x74,
    _MM_PERM_BDBB = 0x75, _MM_PERM_BDBC = 0x76, _MM_PERM_BDBD = 0x77,
    _MM_PERM_BDCA = 0x78, _MM_PERM_BDCB = 0x79, _MM_PERM_BDCC = 0x7A,
    _MM_PERM_BDCD = 0x7B, _MM_PERM_BDDA = 0x7C, _MM_PERM_BDDB = 0x7D,
    _MM_PERM_BDDC = 0x7E, _MM_PERM_BDDD = 0x7F, _MM_PERM_CAAA = 0x80,
    _MM_PERM_CAAB = 0x81, _MM_PERM_CAAC = 0x82, _MM_PERM_CAAD = 0x83,
    _MM_PERM_CABA = 0x84, _MM_PERM_CABB = 0x85, _MM_PERM_CABC = 0x86,
    _MM_PERM_CABD = 0x87, _MM_PERM_CACA = 0x88, _MM_PERM_CACB = 0x89,
    _MM_PERM_CACC = 0x8A, _MM_PERM_CACD = 0x8B, _MM_PERM_CADA = 0x8C,
    _MM_PERM_CADB = 0x8D, _MM_PERM_CADC = 0x8E, _MM_PERM_CADD = 0x8F,
    _MM_PERM_CBAA = 0x90, _MM_PERM_CBAB = 0x91, _MM_PERM_CBAC = 0x92,
    _MM_PERM_CBAD = 0x93, _MM_PERM_CBBA = 0x94, _MM_PERM_CBBB = 0x95,
    _MM_PERM_CBBC = 0x96, _MM_PERM_CBBD = 0x97, _MM_PERM_CBCA = 0x98,
    _MM_PERM_CBCB = 0x99, _MM_PERM_CBCC = 0x9A, _MM_PERM_CBCD = 0x9B,
    _MM_PERM_CBDA = 0x9C, _MM_PERM_CBDB = 0x9D, _MM_PERM_CBDC = 0x9E,
    _MM_PERM_CBDD = 0x9F, _MM_PERM_CCAA = 0xA0, _MM_PERM_CCAB = 0xA1,
    _MM_PERM_CCAC = 0xA2, _MM_PERM_CCAD = 0xA3, _MM_PERM_CCBA = 0xA4,
    _MM_PERM_CCBB = 0xA5, _MM_PERM_CCBC = 0xA6, _MM_PERM_CCBD = 0xA7,
    _MM_PERM_CCCA = 0xA8, _MM_PERM_CCCB = 0xA9, _MM_PERM_CCCC = 0xAA,
    _MM_PERM_CCCD = 0xAB, _MM_PERM_CCDA = 0xAC, _MM_PERM_CCDB = 0xAD,
    _MM_PERM_CCDC = 0xAE, _MM_PERM_CCDD = 0xAF, _MM_PERM_CDAA = 0xB0,
    _MM_PERM_CDAB = 0xB1, _MM_PERM_CDAC = 0xB2, _MM_PERM_CDAD = 0xB3,
    _MM_PERM_CDBA = 0xB4, _MM_PERM_CDBB = 0xB5, _MM_PERM_CDBC = 0xB6,
    _MM_PERM_CDBD = 0xB7, _MM_PERM_CDCA = 0xB8, _MM_PERM_CDCB = 0xB9,
    _MM_PERM_CDCC = 0xBA, _MM_PERM_CDCD = 0xBB, _MM_PERM_CDDA = 0xBC,
    _MM_PERM_CDDB = 0xBD, _MM_PERM_CDDC = 0xBE, _MM_PERM_CDDD = 0xBF,
    _MM_PERM_DAAA = 0xC0, _MM_PERM_DAAB = 0xC1, _MM_PERM_DAAC = 0xC2,
    _MM_PERM_DAAD = 0xC3, _MM_PERM_DABA = 0xC4, _MM_PERM_DABB = 0xC5,
    _MM_PERM_DABC = 0xC6, _MM_PERM_DABD = 0xC7, _MM_PERM_DACA = 0xC8,
    _MM_PERM_DACB = 0xC9, _MM_PERM_DACC = 0xCA, _MM_PERM_DACD = 0xCB,
    _MM_PERM_DADA = 0xCC, _MM_PERM_DADB = 0xCD, _MM_PERM_DADC = 0xCE,
    _MM_PERM_DADD = 0xCF, _MM_PERM_DBAA = 0xD0, _MM_PERM_DBAB = 0xD1,
    _MM_PERM_DBAC = 0xD2, _MM_PERM_DBAD = 0xD3, _MM_PERM_DBBA = 0xD4,
    _MM_PERM_DBBB = 0xD5, _MM_PERM_DBBC = 0xD6, _MM_PERM_DBBD = 0xD7,
    _MM_PERM_DBCA = 0xD8, _MM_PERM_DBCB = 0xD9, _MM_PERM_DBCC = 0xDA,
    _MM_PERM_DBCD = 0xDB, _MM_PERM_DBDA = 0xDC, _MM_PERM_DBDB = 0xDD,
    _MM_PERM_DBDC = 0xDE, _MM_PERM_DBDD = 0xDF, _MM_PERM_DCAA = 0xE0,
    _MM_PERM_DCAB = 0xE1, _MM_PERM_DCAC = 0xE2, _MM_PERM_DCAD = 0xE3,
    _MM_PERM_DCBA = 0xE4, _MM_PERM_DCBB = 0xE5, _MM_PERM_DCBC = 0xE6,
    _MM_PERM_DCBD = 0xE7, _MM_PERM_DCCA = 0xE8, _MM_PERM_DCCB = 0xE9,
    _MM_PERM_DCCC = 0xEA, _MM_PERM_DCCD = 0xEB, _MM_PERM_DCDA = 0xEC,
    _MM_PERM_DCDB = 0xED, _MM_PERM_DCDC = 0xEE, _MM_PERM_DCDD = 0xEF,
    _MM_PERM_DDAA = 0xF0, _MM_PERM_DDAB = 0xF1, _MM_PERM_DDAC = 0xF2,
    _MM_PERM_DDAD = 0xF3, _MM_PERM_DDBA = 0xF4, _MM_PERM_DDBB = 0xF5,
    _MM_PERM_DDBC = 0xF6, _MM_PERM_DDBD = 0xF7, _MM_PERM_DDCA = 0xF8,
    _MM_PERM_DDCB = 0xF9, _MM_PERM_DDCC = 0xFA, _MM_PERM_DDCD = 0xFB,
    _MM_PERM_DDDA = 0xFC, _MM_PERM_DDDB = 0xFD, _MM_PERM_DDDC = 0xFE,
    _MM_PERM_DDDD = 0xFF
} _MM_PERM_ENUM;

/* Constants for _MM_FIXUP macro for generating _FixupTable enum */
typedef enum {
    _MM_FIXUP_NO_CHANGE = 0x00,
    _MM_FIXUP_NEG_INF   = 0x01,
    _MM_FIXUP_NEG_ZERO  = 0x02,
    _MM_FIXUP_POS_ZERO  = 0x03,
    _MM_FIXUP_POS_INF   = 0x04,
    _MM_FIXUP_NAN       = 0x05,
    _MM_FIXUP_MAX_FLOAT = 0x06,
    _MM_FIXUP_MIN_FLOAT = 0x07
} _MM_FIXUPRESULT_ENUM;

#define _MM_FIXUP(_NegInf, _Neg, _NegZero, _PosZero, _Pos, _PosInf, _Nan) \
   (_MM_FIXUPTABLE_ENUM) \
   ((int) _NegInf | \
   ((int) _Neg << 3) | \
   ((int) _NegZero << 6) | \
   ((int) _PosZero << 9) | \
   ((int) _Pos << 12) | \
   ((int) _PosInf << 15) | \
   ((int) _Nan << 18))

/* Constants for Fixup function - must be generated by _MM_FIXUP macro */
typedef enum {
    _MM_PLACEHOLDER = 0xffffffff
} _MM_FIXUPTABLE_ENUM;


#define _LPP_max(a, b) ((a) > (b) ? (a) : (b))
#define _LPP_min(a, b) ((a) < (b) ? (a) : (b))

#if !USE_C_PROTOTYPE_PRIMITIVES
static struct
{
    unsigned int v0;
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} _LPP_maskTable32[] =
{
    {0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000},
    {0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000},
    {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000},
    {0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000},
    {0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000},
    {0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF},
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF},
    {0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF},
    {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF},
    {0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
    {0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
    {0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
};

static struct
{
    unsigned int v0;
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} _LPP_maskTable64[] =
{
    {0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000},
    {0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
    {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
};

static int _LPP_SSERoundMode[4] = {
    _MM_ROUND_NEAREST,
    _MM_ROUND_DOWN,
    _MM_ROUND_UP,
    _MM_ROUND_TOWARD_ZERO
};

inline void _LPP_SetSSERoundingMode(_MM_ROUND_MODE_ENUM rc)
{
    _MM_SET_ROUNDING_MODE(_LPP_SSERoundMode[rc]);
}

#endif

static unsigned int _LPP_byteMaskTable[] =
{
    0x00FFFFFF, 0x000000FF, 0xFFFFFF00, 0xFF000000
};

static float _LPP_expAdjIntToFloatTable[] =
{
    1.0, (1.0 / 16.0), (1.0 / 32.0), (1.0 / 256.0), (1.0 / 65536.0), (1.0 / 16777216.0), (1.0 / 2147483648.0), (1.0 / 4294967296.0)
};

static float _LPP_expAdjFloatToIntTable[] =
{
    1.0, 16.0, 32.0, 256.0, 65536.0, 16777216.0, 2147483648.0, 4294967296.0
};

inline bool _LPP_isNaN(float src)
{
    static const unsigned int exponentMask = 0x7f800000;
    static const unsigned int mantissaMask = 0x007fffff;
    unsigned int u = *(unsigned int *) &src;

    return (((u & exponentMask) == exponentMask) && (u & mantissaMask));
}

inline bool _LPP_isINF(float src)
{
    static const unsigned int exponentMask = 0x7f800000;
    static const unsigned int mantissaMask = 0x007fffff;
    unsigned int u = *(unsigned int *) &src;

    return (((u & exponentMask) == exponentMask) && ((u & mantissaMask) == 0));
}

//
// Conversion from float32
//

inline unsigned short _LPP_ConvertFloat32ToFloat16(const float *pfvalue) // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
{
    static const unsigned int m_cFracBits = 10;           // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const unsigned int m_eMax =  m_cExpBias;         // Max exponent
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_wMaxNormal = ((m_eMax+127) << 23) | 0x7FE000;
    static const unsigned int m_wMinNormal = (m_eMin+127) << 23;
    static const unsigned int m_BiasDiffo = (unsigned int) ((m_cExpBias-127) << 23);
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    unsigned int u = *(unsigned int *) pfvalue;
    unsigned short v;
    unsigned int Sign = (u & 0x80000000) >> 16;
    unsigned int MagU = u & 0x7FFFFFFF;     // Absolute value
    if( (u & (0xff<<23)) == (0xff<<23) )
    {
        // INF or NaN
        v = (unsigned short)(Sign | (((1 << m_cExpBits) - 1))<<m_cFracBits);
        if( (u & (~0xff800000)) != 0 )
        {
            // NaN - smash together the fraction bits to
            //       keep the random 1's (in a way that keeps float16->float->float16
            //       conversion invertible down to the bit level, even with NaN).
            v = (unsigned short)(v| (((u>>13)|(u>>3)|(u))&0x000003ff));
        }
    }
    else if (MagU > m_wMaxNormal)
    {
        // Not representable by 16 bit float -> use flt16_max (due to round to zero)
        v = (unsigned short)(Sign | ((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
    }
    else if (MagU < m_wMinNormal)
    {
        // Denormalized value

        // Make implicit 1 explicit
        unsigned int Frac = (MagU & ((1<<23)-1)) | (1<<23);
        int nshift = (m_eMin + 127 - (MagU >> 23));

        if (nshift < 24)
        {
            MagU = Frac >> nshift;
        }
        else
        {
            MagU = 0;
        }

        // Round to zero
        v = (unsigned short)(Sign | (MagU>>m_cFracBitsDiff));
    }
    else
    {
        // Normalize value with Round to zero
        v = (unsigned short)(Sign | ((MagU + m_BiasDiffo)>>m_cFracBitsDiff));
    }
    return v;
}

inline unsigned short _LPP_ConvertFloat32ToFloat11(const float *pfvalue)  // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
{
    static const unsigned int m_cFracBits = 6;            // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const unsigned int m_eMax =  m_cExpBias;         // Max exponent
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_wMaxNormal = ((m_eMax+127) << 23) | 0x7E0000;
    static const unsigned int m_wMinNormal = (m_eMin+127) << 23;
    static const unsigned int m_BiasDiffo = (unsigned int) ((m_cExpBias-127) << 23);
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    unsigned int u = *(unsigned int *) pfvalue;
    unsigned short v;
    unsigned int Sign = (u & 0x80000000);
    unsigned int MagU = u & 0x7FFFFFFF;     // Absolute value
    if( (u & (0xff<<23)) == (0xff<<23) )
    {
        // INF or NaN
        v = (unsigned short)(((1 << m_cExpBits) - 1)<<m_cFracBits);
        if( (u & (~0xff800000)) != 0 )
        {
            // NaN - smash together the fraction bits to
            //       keep the random 1's (in a way that keeps float16->float->float16
            //       conversion invertible down to the bit level, even with NaN).
            v = (unsigned short)(v| (((u>>17)|(u>>11)|(u>>6)|(u))&0x0000003f));
        }
        else if( Sign ) // - INF
        {
            v = 0;
        }
    }
    else if( Sign )
    {
        v = 0; // negative (float11 has no sign)
    }
    else if (MagU > m_wMaxNormal)
    {
        // Not representable by 11 bit float -> use flt11_max (due to round to zero)
        v = (unsigned short)(((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
    }
    else if (MagU < m_wMinNormal)
    {
        // Denormalized value

        // Make implicit 1 explicit
        unsigned int Frac = (MagU & ((1<<23)-1)) | (1<<23);
        int nshift = (m_eMin + 127 - (MagU >> 23));

        if (nshift < 24)
        {
            MagU = Frac >> nshift;
        }
        else
        {
            MagU = 0;
        }

        // Round to zero
        v = (unsigned short)(MagU>>m_cFracBitsDiff);
    }
    else
    {
        // Normalize value with Round to zero
        v = (unsigned short)((MagU + m_BiasDiffo)>>m_cFracBitsDiff);
    }
    return v;
}

inline unsigned short _LPP_ConvertFloat32ToFloat10(const float *pfvalue)  // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
{
    static const unsigned int m_cFracBits = 5;            // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const unsigned int m_eMax =  m_cExpBias;         // Max exponent
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_wMaxNormal = ((m_eMax+127) << 23) | 0x7C0000;
    static const unsigned int m_wMinNormal = (m_eMin+127) << 23;
    static const unsigned int m_BiasDiffo = (unsigned int) ((m_cExpBias-127) << 23);
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    unsigned int u = *(unsigned int *) pfvalue;
    unsigned short v;
    unsigned int Sign = (u & 0x80000000);
    unsigned int MagU = u & 0x7FFFFFFF;     // Absolute value
    if( (u & (0xff<<23)) == (0xff<<23) )
    {
        // INF or NaN
        v = (unsigned short)(((1 << m_cExpBits) - 1)<<m_cFracBits);
        if( (u & (~0xff800000)) != 0 )
        {
            // NaN - smash together the fraction bits to
            //       keep the random 1's (in a way that keeps float16->float->float16
            //       conversion invertible down to the bit level, even with NaN).
            v = (unsigned short)(v| (((u>>18)|(u>>13)|(u>>8)|(u>>3)|(u))&0x0000001f));
        }
        else if( Sign ) // - INF
        {
            v = 0;
        }
    }
    else if( Sign )
    {
        v = 0; // negative (float10 has no sign)
    }
    else if (MagU > m_wMaxNormal)
    {
        // Not representable by 10 bit float -> use flt10_max (due to round to zero)
        v = (unsigned short)(((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
    }
    else if (MagU < m_wMinNormal)
    {
        // Denormalized value

        // Make implicit 1 explicit
        unsigned int Frac = (MagU & ((1<<23)-1)) | (1<<23);
        int nshift = (m_eMin + 127 - (MagU >> 23));

        if (nshift < 24)
        {
            MagU = Frac >> nshift;
        }
        else
        {
            MagU = 0;
        }

        // Round to zero
        v = (unsigned short)(MagU>>m_cFracBitsDiff);
    }
    else
    {
        // Normalize value with Round to zero
        v = (unsigned short)((MagU + m_BiasDiffo)>>m_cFracBitsDiff);
    }
    return v;
}

inline unsigned short _LPP_ConvertFloat32ToUnorm10(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0f) src = 1.0f;
    if (src < 0.0f) src = 0.0f;

    src = src * 1023.0f + 0.5f;

    return (unsigned short) src;
}

inline unsigned short _LPP_ConvertFloat32ToUnorm2(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0f) src = 1.0f;
    if (src < 0.0f) src = 0.0f;

    src = src * 3.0f + 0.5f;

    return (unsigned short) src;
}

inline unsigned char _LPP_ConvertFloat32ToUInt8(float src)
{
    return (unsigned char) src;
}

inline char _LPP_ConvertFloat32ToSInt8(float src)
{
    return (char) src;
}

inline unsigned char _LPP_ConvertFloat32ToUnorm8(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0f) src = 1.0f;
    if (src < 0.0f) src = 0.0f;

    src = src * 255.0f + 0.5f;

    return (unsigned char) src;
}

inline char _LPP_ConvertFloat32ToSnorm8(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0f) src = 1.0f;
    if (src < -1.0f) src = -1.0f;

    src = src * 127.0f;

    if (src >= 0) {
        src += 0.5f;
    } else {
        src -= 0.5f;
    }
    
    return (char) src;
}

inline unsigned char _LPP_ConvertFloat32ToSRgb8(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0) src = 1.0;
    if (src < 0.0) src = 0.0;

    if (src <= 0.0031308) {
        src = src * 12.92f;
    } else {
        src = 1.055f * powf(src, (1.0f / 2.4f)) - 0.055f;
    }

    src = src * 255.0f + 0.5f;

    return (unsigned char) src;
}

inline unsigned short _LPP_ConvertFloat32ToFloat16RZ(const float *pfvalue) // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
{
    return _LPP_ConvertFloat32ToFloat16(pfvalue);
}

inline unsigned short _LPP_ConvertFloat32ToUInt16(float src)
{
    return (unsigned short) src;
}

inline short _LPP_ConvertFloat32ToSInt16(float src)
{
    return (short) src;
}

inline unsigned short _LPP_ConvertFloat32ToUnorm16(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0) src = 1.0;
    if (src < 0.0) src = 0.0;

    src = src * 65535.0f + 0.5f;

    return (unsigned short) src;
}

inline short _LPP_ConvertFloat32ToSnorm16(float src)
{
    if (_LPP_isNaN(src)) return 0;

    if (src > 1.0) src = 1.0;
    if (src < -1.0) src = -1.0;

    src = src * 32767.0f;

    if (src >= 0) {
        src += 0.5f;
    } else {
        src -= 0.5f;
    }

    return (short) src;
}

inline double _LPP_ConvertFloat32ToFloat64(float src)
{
    return (double) src;
}

inline int _LPP_ConvertFloat32ToInt32(float src, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    assert (rc == _MM_ROUND_MODE_NEAREST || rc == _MM_ROUND_MODE_TOWARD_ZERO);

    int round;

    src *= _LPP_expAdjFloatToIntTable[expadj];

    round = (int) src;

    if (rc == _MM_ROUND_MODE_NEAREST) {
        if (src >= 0) round = (int) (src + 0.5);
        else round = (int) (src - 0.5);

        if (src - (int) src == 0.5 && (round & 1) == 1) {
            round = (int) src;
        }
    }

    return round;
}

inline unsigned int _LPP_ConvertFloat32ToUInt32(float src, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    assert (rc == _MM_ROUND_MODE_NEAREST);
    
    int round;

    src *= _LPP_expAdjFloatToIntTable[expadj];

    if (src <= 0) return 0;

    round = (int) (src + 0.5);

    if (src - (int) src == 0.5 && (round & 1) == 1) {
        return (int) src;
    }
    return round;
}

//
// Conversion to float32
//

inline float _LPP_ConvertFloat16ToFloat32(short fp16)
{
    static const unsigned int m_cFracBits = 10;           // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_cSignBit = 15;            // Index of the sign bit
    static const unsigned int m_cSignMask = (1 << m_cSignBit);
    static const unsigned int m_cFracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    int tmp;

    if ((fp16 & ~(m_cSignMask | m_cFracMask)) == 0)
    {
        if ((fp16 & m_cFracMask) != 0)
        {
            // Normalizing the denormalized value
            unsigned int exp = (unsigned int)m_eMin;
            unsigned int frac = fp16 & m_cFracMask;
            while ((frac & (m_cFracMask + 1)) == 0)
            {
                exp--;
                frac <<= 1;
            }
            frac &= ~(m_cFracMask + 1); // Remove hidden bit
            tmp = ((fp16 & m_cSignMask) << 16) | // Sigh bit
                  ((exp + 127) << 23) |     // Exponent
                  (frac << m_cFracBitsDiff);  // Fraction
        }
        else
        {
            // Zero - only sign bit is used
            tmp = (fp16 & m_cSignMask) << 16;
        }
    }
    else
    {
        tmp = ((fp16 & m_cSignMask) << 16) |                         // Sigh bit
              ((((fp16 >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
                m_cExpBias + 127) << 23) |                        // Exponent
              ((fp16 & m_cFracMask) << m_cFracBitsDiff);               // Fraction
    }
    return *(float *) &tmp;
}

inline float _LPP_ConvertFloat11ToFloat32(unsigned int src)
{
    static const unsigned int m_cFracBits = 6;            // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const unsigned int m_ExpMask = ((1<< m_cExpBits) - 1) << m_cFracBits; // Exponent mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    unsigned int tmp;
    unsigned int exp = src & m_ExpMask;
    if( exp == 0 )
    {
        if ((src & m_FracMask) != 0)
        {
            // Normalizing the denormalized value
            exp = (unsigned int) m_eMin;
            unsigned int frac = src & m_FracMask;
            while ((frac & (m_FracMask + 1)) == 0)
            {
                exp--;
                frac <<= 1;
            }
            frac &= ~(m_FracMask + 1); // Remove hidden bit
            tmp = ((exp + 127) << 23) |     // Exponent
                  (frac << m_cFracBitsDiff);  // Fraction
        }
        else
        {
            // Zero
            tmp = 0;
        }
    }
    else if( exp == m_ExpMask )
    {
        tmp =   0x7f800000 |                                       // Exponent
              ((src & m_FracMask) << m_cFracBitsDiff);               // Fraction
    }
    else
    {
        tmp = ((((src >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
                m_cExpBias + 127) << 23) |                        // Exponent
              ((src & m_FracMask) << m_cFracBitsDiff);               // Fraction
    }
    return *(float *) &tmp;
}

inline float _LPP_ConvertFloat10ToFloat32(unsigned int src)
{
    static const unsigned int m_cFracBits = 5;            // Number of fraction bits
    static const unsigned int m_cExpBits = 5;             // Number of exponent bits
    static const unsigned int m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const unsigned int m_ExpMask = ((1<< m_cExpBits) - 1) << m_cFracBits; // Exponent mask
    static const int  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const int  m_eMin = -m_cExpBias+1;       // Min exponent
    static const unsigned int m_cFracBitsDiff = 23 - m_cFracBits;
    unsigned int tmp;
    unsigned int exp = src & m_ExpMask;
    if( exp == 0 )
    {
        if ((src & m_FracMask) != 0)
        {
            // Normalizing the denormalized value
            exp = (unsigned int) m_eMin;
            unsigned int frac = src & m_FracMask;
            while ((frac & (m_FracMask + 1)) == 0)
            {
                exp--;
                frac <<= 1;
            }
            frac &= ~(m_FracMask + 1); // Remove hidden bit
            tmp = ((exp + 127) << 23) |     // Exponent
                  (frac << m_cFracBitsDiff);  // Fraction
        }
        else
        {
            // Zero
            tmp = 0;
        }
    }
    else if( exp == m_ExpMask )
    {
        tmp =   0x7f800000 |                                       // Exponent
              ((src & m_FracMask) << m_cFracBitsDiff);               // Fraction
    }
    else
    {
        tmp = ((((src >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
                m_cExpBias + 127) << 23) |                        // Exponent
              ((src & m_FracMask) << m_cFracBitsDiff);               // Fraction
    }
    return *(float *) &tmp;
}

inline float _LPP_ConvertUnorm10ToFloat32(unsigned int src)
{
    float tmp = (float) src;

    return tmp / 1023.0f;
}

inline float _LPP_ConvertUnorm2ToFloat32(unsigned int src)
{
    float tmp = (float) src;

    return tmp / 3.0f;
}

inline float _LPP_ConvertUInt16ToFloat32(unsigned short src)
{
    return (float) src;
}

inline float _LPP_ConvertSInt16ToFloat32(short src)
{
    return (float) src;
}

inline float _LPP_ConvertUnorm16ToFloat32(unsigned short src)
{
    float tmp = (float) src;

    return tmp / 65535.0f;
}

inline float _LPP_ConvertSnorm16ToFloat32(short src)
{
    float tmp = (float) src;

    tmp = tmp * 32767.0f;

    if (tmp < -1.0f) tmp = -1.0f;

    return tmp;
}

inline float _LPP_ConvertFloat64ToFloat32(const double src, _MM_ROUND_MODE_ENUM)
{
    return (float) src;
}

inline float _LPP_ConvertSRgb8ToFloat32(unsigned char src)
{
    float tmp;

    tmp = (float) src;

    tmp = tmp * (1.0f / 255.0f);

    if (tmp <= 0.04045f) {
        tmp = tmp /12.92f;
    } else {
        tmp = powf(((tmp + 0.055f) / 1.055f), 2.4f);
    }

    return tmp;
}

inline float _LPP_ConvertUInt8ToFloat32(unsigned char src)
{
    return (float) src;
}

inline float _LPP_ConvertSInt8ToFloat32(char src)
{
    return (float) src;
}

inline float _LPP_ConvertUnorm8ToFloat32(unsigned char src)
{
    return (float) src / 255.0f;
}

inline float _LPP_ConvertSnorm8ToFloat32(char src)
{
    float tmp = (float) src;

    tmp = tmp / 127.0f;

    if (tmp < -1.0f) tmp = -1.0f;

    return tmp;
}

inline float _LPP_ConvertInt32ToFloat32(int src, _MM_EXP_ADJ_ENUM expadj)
{
    return ((float) src) * _LPP_expAdjIntToFloatTable[expadj];
}

inline float _LPP_ConvertUInt32ToFloat32(unsigned int src, _MM_EXP_ADJ_ENUM expadj)
{
    return ((float) src) * _LPP_expAdjIntToFloatTable[expadj];
}

//
// Conversion from int32
//

inline char _LPP_ConvertInt32ToSInt8(int src)
{
    return (char) src;
}

inline short _LPP_ConvertInt32ToSInt16(int src)
{
    return (short) src;
}

inline double _LPP_ConvertInt32ToFloat64(const int src)
{
    return (double) src;
}

//
// Conversion to int32
//

inline int _LPP_ConvertSInt8ToInt32(char src)
{
    return (int) src;
}

inline int _LPP_ConvertSInt16ToInt32(short src)
{
    return (int) src;
}

inline int _LPP_ConvertFloat64ToInt32(const double src, _MM_ROUND_MODE_ENUM rc)
{
    assert(rc == _MM_ROUND_MODE_NEAREST);

    int round;

    if (src >= 0) round = (int) (src + 0.5);
    else round = (int) (src - 0.5);

    if (src - (int) src == 0.5 && (round & 1) == 1) {
        return (int) src;
    }
    return round;
}

//
// Conversion from uint32
//

inline unsigned char _LPP_ConvertUInt32ToUInt8(unsigned int src)
{
    return (unsigned char) src;
}

inline unsigned short _LPP_ConvertUInt32ToUInt16(unsigned int src)
{
    return (unsigned short) src;
}

inline double _LPP_ConvertUInt32ToFloat64(const unsigned int src)
{
    return (double) src;
}

//
// Conversion to uint32
//

inline unsigned int _LPP_ConvertUInt8ToUInt32(unsigned char src)
{
    return (unsigned int) src;
}

inline unsigned int _LPP_ConvertUInt16ToUInt32(unsigned short src)
{
    return (unsigned int) src;
}

inline unsigned int _LPP_ConvertFloat64ToUInt32(const double src, _MM_ROUND_MODE_ENUM rc)
{
    assert(rc == _MM_ROUND_MODE_NEAREST);

    int round;

    if (src < 0) return 0;

    round = (int) (src + 0.5);

    if (src - (int) src == 0.5 && (round & 1) == 1) {
        return (int) src;
    }
    return round;
}


inline char _LPP_DownConvByte(int src, _MM_DOWNCONV32_ENUM down_conv)
{
    char result;

    switch(down_conv) {
        case _MM_DOWNC_UINT8:
            *(unsigned char *) &result = _LPP_ConvertFloat32ToUInt8(*(float *) &src);
        break;
        case _MM_DOWNC_SINT8:
            result = _LPP_ConvertFloat32ToSInt8(*(float *) &src);
        break;
        case _MM_DOWNC_UNORM8:
            *(unsigned char *) &result = _LPP_ConvertFloat32ToUnorm8(*(float *) &src);
        break;
        case _MM_DOWNC_SNORM8:
            result = _LPP_ConvertFloat32ToSnorm8(*(float *) &src);
        break;
        case _MM_DOWNC_UINT8I:
            *(unsigned char *) &result = _LPP_ConvertUInt32ToUInt8(*(unsigned int *) &src);
        break;
        case _MM_DOWNC_SINT8I:
            result = _LPP_ConvertInt32ToSInt8(src);
        break;
        default:
            assert(0);
            result = 0;
        break;
    }
    return result;
}

inline short _LPP_DownConvWord(int src, _MM_DOWNCONV32_ENUM down_conv)
{
    short result;

    switch(down_conv) {
        case _MM_DOWNC_FLOAT16:
            *(unsigned short *) &result = _LPP_ConvertFloat32ToFloat16((float *) &src);
        break;
        case _MM_DOWNC_FLOAT16RZ:
            *(unsigned short *) &result = _LPP_ConvertFloat32ToFloat16RZ((float *) &src);
        break;
        case _MM_DOWNC_UINT16:
            *(unsigned short *) &result = _LPP_ConvertFloat32ToUInt16(*(float *) &src);
        break;
        case _MM_DOWNC_SINT16:
            result = _LPP_ConvertFloat32ToSInt16(*(float *) &src);
        break;
        case _MM_DOWNC_UNORM16:
            *(unsigned short *) &result = _LPP_ConvertFloat32ToUnorm16(*(float *) &src);
        break;
        case _MM_DOWNC_SNORM16:
            result = _LPP_ConvertFloat32ToSnorm16(*(float *) &src);
        break;
        case _MM_DOWNC_UINT16I:
            *(unsigned short *) &result = _LPP_ConvertUInt32ToUInt16(*(unsigned int *) &src);
        break;
        case _MM_DOWNC_SINT16I:
            result = _LPP_ConvertInt32ToSInt16(src);
        break;
        default:
            assert(0);
            result = 0;
        break;
    }

    return result;
}

inline int _LPP_DownConvSize(_MM_DOWNCONV32_ENUM conv)
{
    switch(conv) {
        case _MM_DOWNC_NONE:
            return 4;
        break;
        case _MM_DOWNC_FLOAT16:
        case _MM_DOWNC_FLOAT16RZ:
        case _MM_DOWNC_UINT16:
        case _MM_DOWNC_SINT16:
        case _MM_DOWNC_UNORM16:
        case _MM_DOWNC_SNORM16:
        case _MM_DOWNC_UINT16I:
        case _MM_DOWNC_SINT16I:
            return 2;
        break;
        case _MM_DOWNC_UINT8:
        case _MM_DOWNC_SINT8:
        case _MM_DOWNC_UNORM8:
        case _MM_DOWNC_SNORM8:
        case _MM_DOWNC_UINT8I:
        case _MM_DOWNC_SINT8I:
            return 1;
        break;
        default:
            // illegal downConv arg
            assert(0);
            return 0;
        break;
    }
}

inline int _LPP_FullUpConv(void *src, _MM_FULLUP32_ENUM full_up)
{
    int result;

    switch(full_up) {
        case _MM_FULLUPC_NONE:
            result = *(unsigned int *) src;
        break;
        case _MM_FULLUPC_UNORM10A:
            *(float *) &result = _LPP_ConvertUnorm10ToFloat32((*(unsigned int *) src) & 0x3FF);
        break;
        case _MM_FULLUPC_UNORM10B:
            *(float *) &result = _LPP_ConvertUnorm10ToFloat32(((*(unsigned int *) src) >> 10) & 0x3FF);
        break;
        case _MM_FULLUPC_UNORM10C:
            *(float *) &result = _LPP_ConvertUnorm10ToFloat32(((*(unsigned int *) src) >> 20) & 0x3FF);
        break;
        case _MM_FULLUPC_UNORM2D:
            *(float *) &result = _LPP_ConvertUnorm2ToFloat32(((*(unsigned int *) src) >> 30) & 0x3);
        break;
        case _MM_FULLUPC_FLOAT11A:
            *(float *) &result = _LPP_ConvertFloat11ToFloat32((*(unsigned int *) src) & 0x7FF);
        break;
        case _MM_FULLUPC_FLOAT11B:
            *(float *) &result = _LPP_ConvertFloat11ToFloat32(((*(unsigned int *) src) >> 11) & 0x7FF);
        break;
        case _MM_FULLUPC_FLOAT10C:
            *(float *) &result = _LPP_ConvertFloat10ToFloat32(((*(unsigned int *) src) >> 22) & 0x3FF);
        break;
        case _MM_FULLUPC_FLOAT16:
            *(float *) &result = _LPP_ConvertFloat16ToFloat32(*(short *) src);
        break;
        case _MM_FULLUPC_UINT16:
            *(float *) &result = _LPP_ConvertUInt16ToFloat32(*(unsigned short *) src);
        break;
        case _MM_FULLUPC_SINT16:
            *(float *) &result = _LPP_ConvertSInt16ToFloat32(*(short *) src);
        break;
        case _MM_FULLUPC_UNORM16:
            *(float *) &result = _LPP_ConvertUnorm16ToFloat32(*(unsigned short *) src);
        break;
        case _MM_FULLUPC_SNORM16:
            *(float *) &result = _LPP_ConvertSnorm16ToFloat32(*(short *) src);
        break;
        case _MM_FULLUPC_UINT16I:
            *(unsigned int *) &result = _LPP_ConvertUInt16ToUInt32(*(unsigned short *) src);
        break;
        case _MM_FULLUPC_SINT16I:
            result = _LPP_ConvertSInt16ToInt32(*(short *) src);
        break;
        case _MM_FULLUPC_SRGB8:
            *(float *) &result = _LPP_ConvertSRgb8ToFloat32(*(unsigned char *) src);
        break;
        case _MM_FULLUPC_UINT8:
            *(float *) &result = _LPP_ConvertUInt8ToFloat32(*(unsigned char *) src);
        break;
        case _MM_FULLUPC_SINT8:
            *(float *) &result = _LPP_ConvertSInt8ToFloat32(*(char *) src);
        break;
        case _MM_FULLUPC_UNORM8:
            *(float *) &result = _LPP_ConvertUnorm8ToFloat32(*(unsigned char *) src);
        break;
        case _MM_FULLUPC_SNORM8:
            *(float *) &result = _LPP_ConvertSnorm8ToFloat32(*(char *) src);
        break;
        case _MM_FULLUPC_UINT8I:
            *(unsigned int *) &result = _LPP_ConvertUInt8ToUInt32(*(unsigned char *) src);
        break;
        case _MM_FULLUPC_SINT8I:
            result = _LPP_ConvertSInt8ToInt32(*(char *) src);
        break;
        default:
            // illegal downConv arg
            assert(0);
            return 0;
        break;
    }
    return result;
}

inline int _LPP_FullUpConvSize(_MM_FULLUP32_ENUM full_up)
{
    switch(full_up) {
        case _MM_FULLUPC_NONE:
        case _MM_FULLUPC_UNORM10A:
        case _MM_FULLUPC_UNORM10B:
        case _MM_FULLUPC_UNORM10C:
        case _MM_FULLUPC_UNORM2D:
        case _MM_FULLUPC_FLOAT11A:
        case _MM_FULLUPC_FLOAT11B:
        case _MM_FULLUPC_FLOAT10C:
            return 4;
        break;
        case _MM_FULLUPC_FLOAT16:
        case _MM_FULLUPC_UINT16:
        case _MM_FULLUPC_SINT16:
        case _MM_FULLUPC_UNORM16:
        case _MM_FULLUPC_SNORM16:
        case _MM_FULLUPC_UINT16I:
        case _MM_FULLUPC_SINT16I:
            return 2;
        break;
        case _MM_FULLUPC_SRGB8:
        case _MM_FULLUPC_UINT8:
        case _MM_FULLUPC_SINT8:
        case _MM_FULLUPC_UNORM8:
        case _MM_FULLUPC_SNORM8:
        case _MM_FULLUPC_UINT8I:
        case _MM_FULLUPC_SINT8I:
            return 1;
        break;
        default:
            // illegal downConv arg
            assert(0);
            return 0;
        break;
    }
}


/****
 **** Vector operations
 ****/

/* Swizzle doubleword vector */
inline _M512 _mm512_swizzle_r32(_M512 v, _MM_SWIZZLE_ENUM s)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p = (int *) &v;

    switch(s) {
        case _MM_SWIZ_REG_DCBA:
            return v;
        break;
        case _MM_SWIZ_REG_CDAB:
            r[0] = p[1];  r[1] = p[0];  r[2] = p[3];  r[3] = p[2];
            r[4] = p[5];  r[5] = p[4];  r[6] = p[7];  r[7] = p[6];
            r[8] = p[9];  r[9] = p[8];  r[10] = p[11];  r[11] = p[10];
            r[12] = p[13];  r[13] = p[12];  r[14] = p[15];  r[15] = p[14];
            return result;
        break;
        case _MM_SWIZ_REG_BADC:
            r[0] = p[2];  r[1] = p[3];  r[2] = p[0];  r[3] = p[1];
            r[4] = p[6];  r[5] = p[7];  r[6] = p[4];  r[7] = p[5];
            r[8] = p[10];  r[9] = p[11];  r[10] = p[8];  r[11] = p[9];
            r[12] = p[14];  r[13] = p[15];  r[14] = p[12];  r[15] = p[13];
            return result;
        break;
        case _MM_SWIZ_REG_AAAA:
            r[0] = p[0];  r[1] = p[0];  r[2] = p[0];  r[3] = p[0];
            r[4] = p[4];  r[5] = p[4];  r[6] = p[4];  r[7] = p[4];
            r[8] = p[8];  r[9] = p[8];  r[10] = p[8];  r[11] = p[8];
            r[12] = p[12];  r[13] = p[12];  r[14] = p[12];  r[15] = p[12];
            return result;
        break;
        case _MM_SWIZ_REG_BBBB:
            r[0] = p[1];  r[1] = p[1];  r[2] = p[1];  r[3] = p[1];
            r[4] = p[5];  r[5] = p[5];  r[6] = p[5];  r[7] = p[5];
            r[8] = p[9];  r[9] = p[9];  r[10] = p[9];  r[11] = p[9];
            r[12] = p[13];  r[13] = p[13];  r[14] = p[13];  r[15] = p[13];
            return result;
        break;
        case _MM_SWIZ_REG_CCCC:
            r[0] = p[2];  r[1] = p[2];  r[2] = p[2];  r[3] = p[2];
            r[4] = p[6];  r[5] = p[6];  r[6] = p[6];  r[7] = p[6];
            r[8] = p[10];  r[9] = p[10];  r[10] = p[10];  r[11] = p[10];
            r[12] = p[14];  r[13] = p[14];  r[14] = p[14];  r[15] = p[14];
            return result;
        break;
        case _MM_SWIZ_REG_DDDD:
            r[0] = p[3];  r[1] = p[3];  r[2] = p[3];  r[3] = p[3];
            r[4] = p[7];  r[5] = p[7];  r[6] = p[7];  r[7] = p[7];
            r[8] = p[11];  r[9] = p[11];  r[10] = p[11];  r[11] = p[11];
            r[12] = p[15];  r[13] = p[15];  r[14] = p[15];  r[15] = p[15];
            return result;
        break;
        case _MM_SWIZ_REG_DACB:
            r[0] = p[1];  r[1] = p[2];  r[2] = p[0];  r[3] = p[3];
            r[4] = p[5];  r[5] = p[6];  r[6] = p[4];  r[7] = p[7];
            r[8] = p[9];  r[9] = p[10];  r[10] = p[8];  r[11] = p[11];
            r[12] = p[13];  r[13] = p[14];  r[14] = p[12];  r[15] = p[15];
            return result;
        break;
        default:
            // illegal reg32 upconv arg
            assert(0);
            return v;
        break;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p = (__m128i *) &v;

    switch(s) {
        case _MM_SWIZ_REG_DCBA:
            return v;
        break;
        case _MM_SWIZ_REG_CDAB:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(2, 3, 0, 1));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(2, 3, 0, 1));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(2, 3, 0, 1));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(2, 3, 0, 1));
            return result;
        break;
        case _MM_SWIZ_REG_BADC:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(1, 0, 3, 2));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(1, 0, 3, 2));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(1, 0, 3, 2));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(1, 0, 3, 2));
            return result;
        break;
        case _MM_SWIZ_REG_AAAA:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(0, 0, 0, 0));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(0, 0, 0, 0));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(0, 0, 0, 0));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(0, 0, 0, 0));
            return result;
        break;
        case _MM_SWIZ_REG_BBBB:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(1, 1, 1, 1));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(1, 1, 1, 1));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(1, 1, 1, 1));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(1, 1, 1, 1));
            return result;
        break;
        case _MM_SWIZ_REG_CCCC:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(2, 2, 2, 2));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(2, 2, 2, 2));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(2, 2, 2, 2));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(2, 2, 2, 2));
            return result;
        break;
        case _MM_SWIZ_REG_DDDD:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(3, 3, 3, 3));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(3, 3, 3, 3));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(3, 3, 3, 3));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(3, 3, 3, 3));
            return result;
        break;
        case _MM_SWIZ_REG_DACB:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(3, 0, 2, 1));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(3, 0, 2, 1));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(3, 0, 2, 1));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(3, 0, 2, 1));
            return result;
        break;
        default:
            // illegal reg32 upconv arg
            assert(0);
            return v;
        break;
    }
#endif
}

/* Swizzle quadword vector */
inline _M512 _mm512_swizzle_r64(_M512 v, _MM_SWIZZLE_ENUM s)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p = (int64_t *) &v;

    switch(s) {
        case _MM_SWIZ_REG_DCBA:
            return v;
        break;
        case _MM_SWIZ_REG_CDAB:
            r[0] = p[1];  r[1] = p[0];  r[2] = p[3];  r[3] = p[2];
            r[4] = p[5];  r[5] = p[4];  r[6] = p[7];  r[7] = p[6];
            return result;
        break;
        case _MM_SWIZ_REG_BADC:
            r[0] = p[2];  r[1] = p[3];  r[2] = p[0];  r[3] = p[1];
            r[4] = p[6];  r[5] = p[7];  r[6] = p[4];  r[7] = p[5];
            return result;
        break;
        case _MM_SWIZ_REG_AAAA:
            r[0] = p[0];  r[1] = p[0];  r[2] = p[0];  r[3] = p[0];
            r[4] = p[4];  r[5] = p[4];  r[6] = p[4];  r[7] = p[4];
            return result;
        break;
        case _MM_SWIZ_REG_BBBB:
            r[0] = p[1];  r[1] = p[1];  r[2] = p[1];  r[3] = p[1];
            r[4] = p[5];  r[5] = p[5];  r[6] = p[5];  r[7] = p[5];
            return result;
        break;
        case _MM_SWIZ_REG_CCCC:
            r[0] = p[2];  r[1] = p[2];  r[2] = p[2];  r[3] = p[2];
            r[4] = p[6];  r[5] = p[6];  r[6] = p[6];  r[7] = p[6];
            return result;
        break;
        case _MM_SWIZ_REG_DDDD:
            r[0] = p[3];  r[1] = p[3];  r[2] = p[3];  r[3] = p[3];
            r[4] = p[7];  r[5] = p[7];  r[6] = p[7];  r[7] = p[7];
            return result;
        break;
        case _MM_SWIZ_REG_DACB:
            r[0] = p[1];  r[1] = p[2];  r[2] = p[0];  r[3] = p[3];
            r[4] = p[5];  r[5] = p[6];  r[6] = p[4];  r[7] = p[7];
            return result;
        break;
        default:
            // illegal reg64 upconv arg
            assert(0);
            return v;
        break;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p = (__m128i *) &v;

    switch(s) {
        case _MM_SWIZ_REG_DCBA:
            return v;
        break;
        case _MM_SWIZ_REG_CDAB:
            r[0] = _mm_shuffle_epi32(p[0], _MM_SHUFFLE(1, 0, 3, 2));
            r[1] = _mm_shuffle_epi32(p[1], _MM_SHUFFLE(1, 0, 3, 2));
            r[2] = _mm_shuffle_epi32(p[2], _MM_SHUFFLE(1, 0, 3, 2));
            r[3] = _mm_shuffle_epi32(p[3], _MM_SHUFFLE(1, 0, 3, 2));
            return result;
        break;
        case _MM_SWIZ_REG_BADC:
            r[0] = p[1];
            r[1] = p[0];
            r[2] = p[3];
            r[3] = p[2];
            return result;
        break;
        case _MM_SWIZ_REG_AAAA:
            r[0] = _mm_unpacklo_epi64(p[0], p[0]);
            r[1] = _mm_unpacklo_epi64(p[0], p[0]);
            r[2] = _mm_unpacklo_epi64(p[2], p[2]);
            r[3] = _mm_unpacklo_epi64(p[2], p[2]);
            return result;
        break;
        case _MM_SWIZ_REG_BBBB:
            r[0] = _mm_unpackhi_epi64(p[0], p[0]);
            r[1] = _mm_unpackhi_epi64(p[0], p[0]);
            r[2] = _mm_unpackhi_epi64(p[2], p[2]);
            r[3] = _mm_unpackhi_epi64(p[2], p[2]);
            return result;
        break;
        case _MM_SWIZ_REG_CCCC:
            r[0] = _mm_unpacklo_epi64(p[1], p[1]);
            r[1] = _mm_unpacklo_epi64(p[1], p[1]);
            r[2] = _mm_unpacklo_epi64(p[3], p[3]);
            r[3] = _mm_unpacklo_epi64(p[3], p[3]);
            return result;
        break;
        case _MM_SWIZ_REG_DDDD:
            r[0] = _mm_unpackhi_epi64(p[1], p[1]);
            r[1] = _mm_unpackhi_epi64(p[1], p[1]);
            r[2] = _mm_unpackhi_epi64(p[3], p[3]);
            r[3] = _mm_unpackhi_epi64(p[3], p[3]);
            return result;
        break;
        case _MM_SWIZ_REG_DACB:
            *(__m128 *) &r[0] = _mm_shuffle_ps(*(__m128 *) &p[0], *(__m128 *) &p[1], _MM_SHUFFLE(1, 0, 3, 2));
            *(__m128 *) &r[1] = _mm_shuffle_ps(*(__m128 *) &p[0], *(__m128 *) &p[1], _MM_SHUFFLE(3, 2, 1, 0));
            *(__m128 *) &r[2] = _mm_shuffle_ps(*(__m128 *) &p[2], *(__m128 *) &p[3], _MM_SHUFFLE(1, 0, 3, 2));
            *(__m128 *) &r[3] = _mm_shuffle_ps(*(__m128 *) &p[2], *(__m128 *) &p[3], _MM_SHUFFLE(3, 2, 1, 0));
            return result;
        break;
        default:
            // illegal reg64 upconv arg
            assert(0);
            return v;
        break;
    }
#endif
}

/* Load from memory and upconvert to int32 vector */
inline _M512I _mm512_upconv_int32(void *v, _MM_UPCONV_I32_ENUM s, _MM_MEM_HINT_ENUM)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p;

    switch(s) {
        case _MM_BROADCAST_16X16:
            p = (int *) v;
            for (int i=0; i < 16; i++) {
                r[i] = p[i];
            }
        break;
        case _MM_BROADCAST_1X16:
            p = (int *) v;
            for (int i=0; i < 16; i++) {
                r[i] = p[0];
            }
        break;
        case _MM_BROADCAST_4X16:
            p = (int *) v;
            for (int i=0; i < 4; i++) {
                r[i*4 + 0] = p[0];
                r[i*4 + 1] = p[1];
                r[i*4 + 2] = p[2];
                r[i*4 + 3] = p[3];
            }
        break;
        case _MM_UI8_TO_I32:
            for (int i=0; i < 16; i++) {
                *(unsigned int *) &r[i] = _LPP_ConvertUInt8ToUInt32(((unsigned char *) v)[i]);
            }
        break;
        case _MM_SI8_TO_I32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertSInt8ToInt32(((char *) v)[i]);
            }
        break;
        case _MM_UI16_TO_I32:
            for (int i=0; i < 16; i++) {
                *(unsigned int *) &r[i] = _LPP_ConvertUInt16ToUInt32(((unsigned short *) v)[i]);
            }
        break;
        case _MM_SI16_TO_I32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertSInt16ToInt32(((short *) v)[i]);
            }
        break;
        default:
            // illegal mem int32 upconv arg
            assert(0);
        break;
    }
#else
    __m128i *r = (__m128i *) &result;
    int *pi;
    unsigned char *puc;
    char *pc;
    unsigned short *pus;
    short *ps;

    switch(s) {
        case _MM_BROADCAST_16X16:
            pi = (int *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32(pi[i*4 + 3], pi[i*4 + 2], pi[i*4 + 1], pi[i*4 + 0]);
            }
        break;
        case _MM_BROADCAST_1X16:
            pi = (int *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set1_epi32(pi[0]);
            }
        break;
        case _MM_BROADCAST_4X16:
            pi = (int *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32(pi[3], pi[2], pi[1], pi[0]);
            }
        break;
        case _MM_UI8_TO_I32:
            puc = (unsigned char *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32((int) puc[i*4 + 3], (int) puc[i*4 + 2], (int) puc[i*4 + 1], (int) puc[i*4 + 0]);
            }
        break;
        case _MM_SI8_TO_I32:
            pc = (char *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32((int) pc[i*4 + 3], (int) pc[i*4 + 2], (int) pc[i*4 + 1], (int) pc[i*4 + 0]);
            }
        break;
        case _MM_UI16_TO_I32:
            pus = (unsigned short *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32((int) pus[i*4 + 3], (int) pus[i*4 + 2], (int) pus[i*4 + 1], (int) pus[i*4 + 0]);
            }
        break;
        case _MM_SI16_TO_I32:
            ps = (short *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32((int) ps[i*4 + 3], (int) ps[i*4 + 2], (int) ps[i*4 + 1], (int) ps[i*4 + 0]);
            }
        break;
        default:
            // illegal mem int32 upconv arg
            assert(0);
        break;
    }

#endif
    return result;
}

/* Load from memory and upconvert to float32 vector */
inline _M512 _mm512_upconv_float32(void *v, _MM_UPCONV_F32_ENUM s, _MM_MEM_HINT_ENUM)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p;

    switch(s) {
        case _MM_BROADCAST_16X16:
            p = (float *) v;
            for (int i=0; i < 16; i++) {
                r[i] = p[i];
            }
        break;
        case _MM_BROADCAST_1X16:
            p = (float *) v;
            for (int i=0; i < 16; i++) {
                r[i] = p[0];
            }
        break;
        case _MM_BROADCAST_4X16:
            p = (float *) v;
            for (int i=0; i < 4; i++) {
                r[i*4 + 0] = p[0];
                r[i*4 + 1] = p[1];
                r[i*4 + 2] = p[2];
                r[i*4 + 3] = p[3];
            }
        break;
        case _MM_UI8_TO_F32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertUInt8ToFloat32(((unsigned char *) v)[i]);
            }
        break;
        case _MM_UN8_TO_F32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertUnorm8ToFloat32(((unsigned char *) v)[i]);
            }
        break;
        case _MM_F16_TO_F32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertFloat16ToFloat32(((short *) v)[i]);
            }
        break;
        case _MM_SI16_TO_F32:
            for (int i=0; i < 16; i++) {
                r[i] = _LPP_ConvertSInt16ToFloat32(((short *) v)[i]);
            }
        break;
        default:
            // illegal mem float32 upconv arg
            assert(0);
        break;
    }
#else
    __m128 *r = (__m128 *) &result;
    float *pf;
    unsigned char *puc;
    short *ps;

    switch(s) {
        case _MM_BROADCAST_16X16:
            pf = (float *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(pf[i*4 + 3], pf[i*4 + 2], pf[i*4 + 1], pf[i*4 + 0]);
            }
        break;
        case _MM_BROADCAST_1X16:
            pf = (float *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set1_ps(pf[0]);
            }
        break;
        case _MM_BROADCAST_4X16:
            pf = (float *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(pf[3], pf[2], pf[1], pf[0]);
            }
        break;
        case _MM_UI8_TO_F32:
            puc = (unsigned char *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(_LPP_ConvertUInt8ToFloat32(puc[i*4 + 3]), _LPP_ConvertUInt8ToFloat32(puc[i*4 + 2]), _LPP_ConvertUInt8ToFloat32(puc[i*4 + 1]), _LPP_ConvertUInt8ToFloat32(puc[i*4 + 0]));
            }
        break;
        case _MM_UN8_TO_F32:
            puc = (unsigned char *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(_LPP_ConvertUnorm8ToFloat32(puc[i*4 + 3]), _LPP_ConvertUnorm8ToFloat32(puc[i*4 + 2]), _LPP_ConvertUnorm8ToFloat32(puc[i*4 + 1]), _LPP_ConvertUnorm8ToFloat32(puc[i*4 + 0]));
            }
        break;
        case _MM_F16_TO_F32:
            ps = (short *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(_LPP_ConvertFloat16ToFloat32(ps[i*4 + 3]), _LPP_ConvertFloat16ToFloat32(ps[i*4 + 2]), _LPP_ConvertFloat16ToFloat32(ps[i*4 + 1]), _LPP_ConvertFloat16ToFloat32(ps[i*4 + 0]));
            }
        break;
        case _MM_SI16_TO_F32:
            ps = (short *) v;
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_ps(_LPP_ConvertSInt16ToFloat32(ps[i*4 + 3]), _LPP_ConvertSInt16ToFloat32(ps[i*4 + 2]), _LPP_ConvertSInt16ToFloat32(ps[i*4 + 1]), _LPP_ConvertSInt16ToFloat32(ps[i*4 + 0]));
            }
        break;
        default:
            // illegal mem float32 upconv arg
            assert(0);
        break;
    }
#endif
    return result;
}

/* Load from memory and upconvert to int64 vector */
inline _M512I _mm512_upconv_int64(void *v, _MM_UPCONV_I64_ENUM s, _MM_MEM_HINT_ENUM)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p = (int64_t *) v;

    switch(s) {
        case _MM_BROADCAST_8X8:
            for (int i=0; i < 8; i++) {
                r[i] = p[i];
            }
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 8; i++) {
                r[i] = p[0];
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 2; i++) {
                r[i*4+0] = p[0];
                r[i*4+1] = p[1];
                r[i*4+2] = p[2];
                r[i*4+3] = p[3];
            }
        break;
        default:
            // illegal mem int64 upconv arg
            assert(0);
        break;
    }
#else
    __m128i *r = (__m128i *) &result;
    int64_t *p = (int64_t *) v;

    switch(s) {
        case _MM_BROADCAST_8X8:
            r[0] = _mm_set_epi32((int) (p[1] >> 32), (int) (p[1] & 0xFFFFFFFF), (int) (p[0] >> 32), (int) (p[0] & 0xFFFFFFFF));
            r[1] = _mm_set_epi32((int) (p[3] >> 32), (int) (p[3] & 0xFFFFFFFF), (int) (p[2] >> 32), (int) (p[2] & 0xFFFFFFFF));
            r[2] = _mm_set_epi32((int) (p[5] >> 32), (int) (p[5] & 0xFFFFFFFF), (int) (p[4] >> 32), (int) (p[4] & 0xFFFFFFFF));
            r[3] = _mm_set_epi32((int) (p[7] >> 32), (int) (p[7] & 0xFFFFFFFF), (int) (p[6] >> 32), (int) (p[6] & 0xFFFFFFFF));
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set_epi32((int) (p[0] >> 32), (int) (p[0] & 0xFFFFFFFF), (int) (p[0] >> 32), (int) (p[0] & 0xFFFFFFFF));
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 2; i++) {
                r[i*2+0] = _mm_set_epi32((int) (p[1] >> 32), (int) (p[1] & 0xFFFFFFFF), (int) (p[0] >> 32), (int) (p[0] & 0xFFFFFFFF));
                r[i*2+1] = _mm_set_epi32((int) (p[3] >> 32), (int) (p[3] & 0xFFFFFFFF), (int) (p[2] >> 32), (int) (p[2] & 0xFFFFFFFF));
            }
        break;
        default:
            // illegal mem int64 upconv arg
            assert(0);
        break;
    }
#endif
    return result;
}

/* Load from memory and upconvert to float64 vector */
inline _M512D _mm512_upconv_float64(void *v, _MM_UPCONV_F64_ENUM s, _MM_MEM_HINT_ENUM)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p = (double *) v;

    switch(s) {
        case _MM_BROADCAST_8X8:
            for (int i=0; i < 8; i++) {
                r[i] = p[i];
            }
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 8; i++) {
                r[i] = p[0];
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 2; i++) {
                r[i*4+0] = p[0];
                r[i*4+1] = p[1];
                r[i*4+2] = p[2];
                r[i*4+3] = p[3];
            }
        break;
        default:
            // illegal mem int64 upconv arg
            assert(0);
        break;
    }
#else
    __m128d *r = (__m128d *) &result;
    double *p = (double *) v;

    switch(s) {
        case _MM_BROADCAST_8X8:
            r[0] = _mm_set_pd(p[1], p[0]);
            r[1] = _mm_set_pd(p[3], p[2]);
            r[2] = _mm_set_pd(p[5], p[4]);
            r[3] = _mm_set_pd(p[7], p[6]);
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 4; i++) {
                r[i] = _mm_set1_pd(p[0]);
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 2; i++) {
                r[i*2+0] = _mm_set_pd(p[1], p[0]);
                r[i*2+1] = _mm_set_pd(p[3], p[2]);
            }
        break;
        default:
            // illegal mem int64 upconv arg
            assert(0);
        break;
    }
#endif
    return result;
}

/* Add int32 vectors with carry. */
inline _M512I _mm512_adc_pi(_M512I v1, __mmask k2, _M512I v3, __mmask *carry)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] + p3[i] + (k2 & 1);
        uint64_t tmp = (uint64_t) p1[i] + (uint64_t) p3[i] + (uint64_t) (k2 & 1);
        returnMask |= (((tmp >> 32) & 0x01) << i);
        k2 >>= 1;
    }
    *carry = returnMask;
    return result;
}

/* Add int32 vectors with carry under mask. */
inline _M512I _mm512_mask_adc_pi(_M512I v1, __mmask k1, __mmask k2, _M512I v3, __mmask *carry)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] + p3[i] + (k2 & 1);
            uint64_t tmp = (uint64_t) p1[i] + (uint64_t) p3[i] + (uint64_t) (k2 & 1);
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2 & 1) << i);
        }
        k1 >>= 1;
        k2 >>= 1;
    }
    *carry = returnMask;
    return result;
}

/* Add and negate float64 vectors. */
inline _M512D _mm512_addn_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = -(p2[i] + p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Add and negate float64 vectors under mask. */
inline _M512D _mm512_mask_addn_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
     _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = -(p2[i] + p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Add and negate float32 vectors. */
inline _M512 _mm512_addn_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i = 0; i < 16; i++ ) {
        r[i] = -(p2[i] + p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Add and negate float32 vectors under mask. */
inline _M512 _mm512_mask_addn_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = -(p2[i] + p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Add float64 vectors. */
inline _M512D _mm512_add_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p2[i] + p3[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Add float64 vectors under mask. */
inline _M512D _mm512_mask_add_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] + p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Add int32 vectors. */
inline _M512I _mm512_add_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] + p3[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_epi32(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Add int32 vectors under mask. */
inline _M512I _mm512_mask_add_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] + p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_epi32(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Add float32 vectors. */
inline _M512 _mm512_add_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] + p3[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Add float32 vectors under mask. */
inline _M512 _mm512_mask_add_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] + p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Add int32 vectors and set carry. */
inline _M512I _mm512_addsetc_pi(_M512I v1, _M512I v3, __mmask *carry)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] + p3[i];
        uint64_t tmp = (uint64_t) p1[i] + (uint64_t) p3[i];
        returnMask |= (((tmp >> 32) & 0x01) << i);
    }
    *carry = returnMask;
    return result;
}

/* Add int32 vectors and set carry under mask. */
inline _M512I _mm512_mask_addsetc_pi(_M512I v1, __mmask k1, __mmask k2_old, _M512I v3, __mmask *carry)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] + p3[i];
            uint64_t tmp = (uint64_t) p1[i] + (uint64_t) p3[i];
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2_old & 1) << i);
        }
        k1 >>= 1;
        k2_old >>= 1;
    }
    *carry = returnMask;
    return result;
}

/* Add int32 vectors and set mask to sign. */
inline _M512I _mm512_addsets_pi(_M512I v2, _M512I v3, __mmask *sign)
{
    __mmask newMask = 0;
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] + p3[i];
        newMask |= ((r[i] < 0 ? 1 : 0) << i);
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_epi32(p2[i], p3[i]);
        newMask |= (_mm_movemask_ps(*(__m128 *) &r[i]) << i*4);
    }
#endif
    *sign = newMask;
    return result;
}

/* Add int32 vectors and set mask to sign under mask. */
inline _M512I _mm512_mask_addsets_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3, __mmask *sign)
{
    __mmask newMask = 0;
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] + p3[i];
            newMask |= ((r[i] < 0 ? 1 : 0) << i);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_epi32(p2[i], p3[i]);
        newMask |= (_mm_movemask_ps(*(__m128 *) &r[i]) << i*4);
    }

    newMask &= k1;
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    *sign = newMask;
    return result;
}

/* Add float32 vectors and set mask to sign. */
inline _M512 _mm512_addsets_ps(_M512 v2, _M512 v3, __mmask *sign)
{
    _M512 result;
    __mmask returnMask = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] + p3[i];
        returnMask |= ((r[i] < 0 ? 1 : 0) << i);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(p2[i], p3[i]);
        returnMask |= (_mm_movemask_ps(r[i]) << i*4);
    }
#endif
    *sign = returnMask;
    return result;
}

/* Add float32 vectors and set mask to sign under mask. */
inline _M512 _mm512_mask_addsets_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3, __mmask *sign)
{
    __mmask newMask = 0;
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] + p3[i];
            newMask |= ((r[i] < 0 ? 1 : 0) << i);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(p2[i], p3[i]);
        newMask |= (_mm_movemask_ps(r[i]) << i*4);
    }

    newMask &= k1;
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    *sign = newMask;
    return result;
}

/* Bitwise AND NOT int32 vectors. */
inline _M512I _mm512_andn_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (~p2[i]) & p3[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_andnot_si128(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Bitwise AND NOT int32 vectors under mask. */
inline _M512I _mm512_mask_andn_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (~p2[i]) & p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_andnot_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Bitwise AND NOT int64 vectors. */
inline _M512I _mm512_andn_pq(_M512I v2, _M512I v3)
{
    return _mm512_andn_pi(v2, v3);
}

/* Bitwise AND NOT int64 vectors under mask. */
inline _M512I _mm512_mask_andn_pq(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *p2 = (int64_t *) &v2;
    int64_t *p3 = (int64_t *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (~p2[i]) & p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_andnot_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Bitwise AND int32 vectors. */
inline _M512I _mm512_and_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] & p3[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_and_si128(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Bitwise AND int32 vectors under mask. */
inline _M512I _mm512_mask_and_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] & p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_and_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Bitwise AND int64 vectors. */
inline _M512I _mm512_and_pq(_M512I v2, _M512I v3)
{
    return _mm512_and_pi(v2, v3);
}

/* Bitwise AND int64 vectors under mask. */
inline _M512I _mm512_mask_and_pq(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *p2 = (int64_t *) &v2;
    int64_t *p3 = (int64_t *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] & p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_and_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* 1:1 bit-interleave int32 vectors. */
inline _M512I _mm512_bitinterleave11_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        unsigned int bit = 1;
        r[i] = 0;
        for (int j=0; j < 16; j++) {
            r[i] |= ((p3[i] & bit) << j);
            r[i] |= ((p2[i] & bit) << (j + 1));
            bit <<= 1;
        }
    }
    return result;
}

/* 1:1 bit-interleave int32 vectors under mask. */
inline _M512I _mm512_mask_bitinterleave11_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
       if (k1 & 1) {
            unsigned int bit = 1;
            r[i] = 0;
            for (int j=0; j < 16; j++) {
                r[i] |= ((p3[i] & bit) << j);
                r[i] |= ((p2[i] & bit) << (j + 1));
                bit <<= 1;
            }
       } else {
            r[i] = p1[i];
       }
        k1 >>= 1;
    }
    return result;
}

/* 2:1 bit-interleave int32 vectors. */
inline _M512I _mm512_bitinterleave21_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        unsigned int bit = 1;
        unsigned int bits = 3;
        r[i] = 0;
        for (int j=0; j < 10; j++) {
            r[i] |= ((p3[i] & bit) << (j*2));
            r[i] |= ((p2[i] & bits) << (j + 1));
            bit <<= 1;
            bits <<= 2;
        }
        r[i] |= ((p3[i] & 0x400) << 20);
        r[i] |= ((p2[i] & 0x100000) << 11);
    }
    return result;
}

/* 2:1 bit-interleave int32 vectors under mask. */
inline _M512I _mm512_mask_bitinterleave21_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
       if (k1 & 1) {
            unsigned int bit = 1;
            unsigned int bits = 3;
            r[i] = 0;
            for (int j=0; j < 10; j++) {
                r[i] |= ((p3[i] & bit) << (j*2));
                r[i] |= ((p2[i] & bits) << (j + 1));
                bit <<= 1;
                bits <<= 2;
            }
            r[i] |= ((p3[i] & 0x400) << 20);
            r[i] |= ((p2[i] & 0x100000) << 11);
       } else {
           r[i] = p1[i];
       }
       k1 >>= 1;
    }
    return result;
}

/* Clamp to zero int32 vector. */
inline _M512I _mm512_clampz_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (p3[i] < p2[i]) r[i] = p3[i];
        else               r[i] = p2[i];
        if (r[i] < 0) r[i] = 0;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;
    __m128i zero = _mm_setzero_si128();

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p3[i]), _mm_andnot_si128(tmp, p2[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(r[i], zero);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, r[i]), _mm_andnot_si128(tmp, zero));
    }
#endif
    return result;
}

/* Clamp to zero int32 vector under mask. */
inline _M512I _mm512_mask_clampz_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p3[i] < p2[i]) r[i] = p3[i];
            else               r[i] = p2[i];
            if (r[i] < 0) r[i] = 0;
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;
    __m128i zero = _mm_setzero_si128();

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p3[i]), _mm_andnot_si128(tmp, p2[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(r[i], zero);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, r[i]), _mm_andnot_si128(tmp, zero));
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Clamp to zero float32 vector. */
inline _M512 _mm512_clampz_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (p3[i] >= 0) {
            if (p2[i] > 0) r[i] = p2[i];
            else r[i] = 0;
            if (p3[i] < r[i]) r[i] = p3[i];
        } else {
            if (p2[i] < 0) r[i] = p2[i];
            else r[i] = 0;
            if (p3[i] > r[i]) r[i] = p3[i];
        }
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        __m128 tmp1 = _mm_min_ps(_mm_max_ps(p2[i], zero), p3[i]);
        __m128 tmp2 = _mm_max_ps(_mm_min_ps(p2[i], zero), p3[i]);
        __m128 v3Pos = _mm_cmpge_ps(p3[i], zero);
        r[i] = _mm_or_ps(_mm_and_ps(v3Pos, tmp1), _mm_andnot_ps(v3Pos, tmp2));
    }
#endif
    return result;
}

/* Clamp to zero float32 vector under mask. */
inline _M512 _mm512_mask_clampz_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p3[i] >= 0) {
                if (p2[i] > 0) r[i] = p2[i];
                else r[i] = 0;
                if (p3[i] < r[i]) r[i] = p3[i];
            } else {
                if (p2[i] < 0) r[i] = p2[i];
                else r[i] = 0;
                if (p3[i] > r[i]) r[i] = p3[i];
            }
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        __m128 tmp1 = _mm_min_ps(_mm_max_ps(p2[i], zero), p3[i]);
        __m128 tmp2 = _mm_max_ps(_mm_min_ps(p2[i], zero), p3[i]);
        __m128 v3Pos = _mm_cmpge_ps(p3[i], zero);
        r[i] = _mm_or_ps(_mm_and_ps(v3Pos, tmp1), _mm_andnot_ps(v3Pos, tmp2));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmskeq_pu(__mmask k1, _M512I v1, _M512I v2, int field)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if ((p1[i] & byteMask) == (p2[i] & byteMask)) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;
    __m128i byteMask = _mm_set1_epi32(_LPP_byteMaskTable[field]);

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(_mm_and_si128(byteMask, p1[i]), _mm_and_si128(byteMask, p2[i]));
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
#endif
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmskeq_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmskeq_pu(0xFFFF, v1, v2, field);
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmsklt_pu(__mmask k1, _M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if ((p1[i] & byteMask) < (p2[i] & byteMask)) result |= (1 << i);
    }
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmsklt_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmsklt_pu(0xFFFF, v1, v2, field);
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmskle_pu(__mmask k1, _M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if ((p1[i] & byteMask) <= (p2[i] & byteMask)) result |= (1 << i);
    }
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmskle_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmskle_pu(0xFFFF, v1, v2, field);
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmskneq_pu(__mmask k1, _M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if ((p1[i] & byteMask) != (p2[i] & byteMask)) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;
    __m128i byteMask = _mm_set1_epi32(_LPP_byteMaskTable[field]);

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(_mm_and_si128(byteMask, p1[i]), _mm_and_si128(byteMask, p2[i]));
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmskneq_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmskneq_pu(0xFFFF, v1, v2, field);
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmsknlt_pu(__mmask k1, _M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if (!((p1[i] & byteMask) < (p2[i] & byteMask))) result |= (1 << i);
    }
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmsknlt_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmsknlt_pu(0xFFFF, v1, v2, field);
}

/* Compare bytemasked uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpbmsknle_pu(__mmask k1, _M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int byteMask = _LPP_byteMaskTable[field];

    for (int i=0; i < 16; i++) {
        if (!((p1[i] & byteMask) <= (p2[i] & byteMask))) result |= (1 << i);
    }
    return result & k1;
}

/* Compare bytemasked uint32 vectors and set mask. */
inline __mmask _mm512_cmpbmsknle_pu(_M512I v1, _M512I v2, _MM_BMSK_FIELD_ENUM field)
{
    return _mm512_mask_cmpbmsknle_pu(0xFFFF, v1, v2, field);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpeq_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (p1[i] == p2[i]) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpeq_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpeq_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpeq_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmplt_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (p1[i] < p2[i]) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmplt_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmplt_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmplt_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmple_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (p1[i] <= p2[i]) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmple_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmple_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmple_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpunord_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (!(p1[i] <= p2[i] || p1[i] > p2[i])) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpunord_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpunord_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpunord_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpneq_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (p1[i] != p2[i]) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpneq_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpneq_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpneq_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnlt_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (!(p1[i] < p2[i])) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpnlt_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpnlt_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpnlt_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnle_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (!(p1[i] <= p2[i])) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpnle_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpnle_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpnle_pd(0xFFFF, v1, v2);
}

/* Compare float64 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpord_pd(__mmask k1, _M512D v1, _M512D v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (p1[i] <= p2[i] || p1[i] > p2[i]) result |= (1 << i);
    }
#else
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_pd(_mm_cmpord_pd(p1[i], p2[i])) << i*2);
    }
#endif
    return (k1 & 0xFF00) | (result & k1);
}

/* Compare float64 vectors and set mask. */
inline __mmask _mm512_cmpord_pd(_M512D v1, _M512D v2)
{
    return _mm512_mask_cmpord_pd(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpeq_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] == p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmpeq_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpeq_pi(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmplt_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] < p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmplt_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmplt_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmplt_pi(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmple_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] <= p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmple_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmple_pi(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpneq_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] != p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmpneq_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpneq_pi(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnlt_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (!(p1[i] < p2[i])) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmplt_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmpnlt_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpnlt_pi(0xFFFF, v1, v2);
}

/* Compare int32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnle_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] > p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
#endif
    return result & k1;
}

/* Compare int32 vectors and set mask. */
inline __mmask _mm512_cmpnle_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpnle_pi(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpeq_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] == p2[i]) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpeq_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpeq_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpeq_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmplt_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] < p2[i]) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmplt_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmplt_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmplt_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmple_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] <= p2[i]) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmple_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmple_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmple_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpunord_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (!(p1[i] <= p2[i] || p1[i] > p2[i])) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpunord_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpunord_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpunord_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpneq_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] != p2[i]) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpneq_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpneq_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpneq_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnlt_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (!(p1[i] < p2[i])) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpnlt_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpnlt_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpnlt_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnle_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (!(p1[i] <= p2[i])) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpnle_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpnle_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpnle_ps(0xFFFF, v1, v2);
}

/* Compare float32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpord_ps(__mmask k1, _M512 v1, _M512 v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] <= p2[i] || p1[i] > p2[i]) result |= (1 << i);
    }
#else
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 4; i++) {
        result |= (_mm_movemask_ps(_mm_cmpord_ps(p1[i], p2[i])) << i*4);
    }
#endif
    return result & k1;
}

/* Compare float32 vectors and set mask. */
inline __mmask _mm512_cmpord_ps(_M512 v1, _M512 v2)
{
    return _mm512_mask_cmpord_ps(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpeq_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] == p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
#endif
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmpeq_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpeq_pu(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmplt_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] < p2[i]) result |= (1 << i);
    }
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmplt_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmplt_pu(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmple_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] <= p2[i]) result |= (1 << i);
    }
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmple_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmple_pu(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpneq_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] != p2[i]) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(p1[i], p2[i]);
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmpneq_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpneq_pu(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnlt_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (!(p1[i] < p2[i])) result |= (1 << i);
    }
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmpnlt_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpnlt_pu(0xFFFF, v1, v2);
}

/* Compare uint32 vectors and set mask under mask. */
inline __mmask _mm512_mask_cmpnle_pu(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p1[i] > p2[i]) result |= (1 << i);
    }
    return result & k1;
}

/* Compare uint32 vectors and set mask. */
inline __mmask _mm512_cmpnle_pu(_M512I v1, _M512I v2)
{
    return _mm512_mask_cmpnle_pu(0xFFFF, v1, v2);
}

/* Convert and insert float32 vector to float11:11:10 vector. */
inline _M512 _mm512_cvtins_ps2f11(_M512 v1, _M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_FLOAT11_FIELD_ENUM field)
{
    assert(rc == _MM_ROUND_MODE_NEAREST);

    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (field == _MM_FLOAT11A) {
            r[i] = (p1[i] & ~0x000007FF) | (_LPP_ConvertFloat32ToFloat11(&p2[i]) & 0x000007FF);
        } else if (field == _MM_FLOAT11B) {
            r[i] = (p1[i] & ~0x003FF800) | ((_LPP_ConvertFloat32ToFloat11(&p2[i]) << 11) & 0x003FF800);
        } else if (field == _MM_FLOAT10C) {
            r[i] = (p1[i] & ~0xFFC00000) | ((_LPP_ConvertFloat32ToFloat10(&p2[i]) << 22) & 0xFFC00000);
        } else if (field == _MM_FLOAT11NONE) {
            r[i] = p1[i];
        }
    }
    return result;
}

/* Convert and insert float32 vector to float11:11:10 vector under mask. */
inline _M512 _mm512_mask_cvtins_ps2f11(_M512 v1, __mmask k1, _M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_FLOAT11_FIELD_ENUM field)
{
    assert(rc == _MM_ROUND_MODE_NEAREST);

    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (field == _MM_FLOAT11A) {
                r[i] = (p1[i] & ~0x000007FF) | (_LPP_ConvertFloat32ToFloat11(&p2[i]) & 0x000007FF);
            } else if (field == _MM_FLOAT11B) {
                r[i] = (p1[i] & ~0x003FF800) | ((_LPP_ConvertFloat32ToFloat11(&p2[i]) << 11) & 0x003FF800);
            } else if (field == _MM_FLOAT10C) {
                r[i] = (p1[i] & ~0xFFC00000) | ((_LPP_ConvertFloat32ToFloat10(&p2[i]) << 22) & 0xFFC00000);
            } else if (field == _MM_FLOAT11NONE) {
                r[i] = p1[i];
            }
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert and insert float32 vector to unorm10:10:10:2 vector. */
inline _M512 _mm512_cvtins_ps2u10(_M512 v1, _M512 v2, _MM_UNORM10_FIELD_ENUM field)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (field == _MM_UNORM10A) {
            r[i] = (p1[i] & ~0x000003FF) | (_LPP_ConvertFloat32ToUnorm10(p2[i]) & 0x000003FF);
        } else if (field == _MM_UNORM10B) {
            r[i] = (p1[i] & ~0x000FFC00) | ((_LPP_ConvertFloat32ToUnorm10(p2[i]) << 10) & 0x000FFC00);
        } else if (field == _MM_UNORM10C) {
            r[i] = (p1[i] & ~0x3FF00000) | ((_LPP_ConvertFloat32ToUnorm10(p2[i]) << 20) & 0x3FF00000);
        } else if (field == _MM_UNORM2D) {
            r[i] = (p1[i] & ~0xC0000000) | ((_LPP_ConvertFloat32ToUnorm2(p2[i]) << 30) & 0xC0000000);
        }
    }
    return result;
}

/* Convert and insert float32 vector to unorm10:10:10:2 vector under mask. */
inline _M512 _mm512_mask_cvtins_ps2u10(_M512 v1, __mmask k1, _M512 v2, _MM_UNORM10_FIELD_ENUM field)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (field == _MM_UNORM10A) {
                r[i] = (p1[i] & ~0x000003FF) | (_LPP_ConvertFloat32ToUnorm10(p2[i]) & 0x000003FF);
            } else if (field == _MM_UNORM10B) {
                r[i] = (p1[i] & ~0x000FFC00) | ((_LPP_ConvertFloat32ToUnorm10(p2[i]) << 10) & 0x000FFC00);
            } else if (field == _MM_UNORM10C) {
                r[i] = (p1[i] & ~0x3FF00000) | ((_LPP_ConvertFloat32ToUnorm10(p2[i]) << 20) & 0x3FF00000);
            } else if (field == _MM_UNORM2D) {
                r[i] = (p1[i] & ~0xC0000000) | ((_LPP_ConvertFloat32ToUnorm2(p2[i]) << 30) & 0xC0000000);
            }
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert float64 vector to lower half of int32 vector. */
inline _M512I _mm512_cvtl_pd2pi(_M512I v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertFloat64ToInt32(p2[i], rc);
        r[8 + i] = p1[8 + i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_cvtpd_epi32(p2[i*2 + 0]);
        __m128i tmp2 = _mm_cvtpd_epi32(p2[i*2 + 1]);
        r[i] = _mm_unpacklo_epi64(tmp, tmp2);
        r[2 + i] = p1[2 + i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Convert float64 vector to lower half of int32 vector under mask. */
inline _M512I _mm512_mask_cvtl_pd2pi(_M512I v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat64ToInt32(p2[i], rc);
        } else {
            r[i] = p1[i];
        }
        r[8 + i] = p1[8 + i];
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_cvtpd_epi32(p2[i*2 + 0]);
        __m128i tmp2 = _mm_cvtpd_epi32(p2[i*2 + 1]);
        r[i] = _mm_unpacklo_epi64(tmp, tmp2);
        r[2 + i] = p1[2 + i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 2; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert float64 vector to higher half of int32 vector. */
inline _M512I _mm512_cvth_pd2pi(_M512I v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[8 + i] = _LPP_ConvertFloat64ToInt32(p2[i], rc);
        r[i] = p1[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_cvtpd_epi32(p2[i*2 + 0]);
        __m128i tmp2 = _mm_cvtpd_epi32(p2[i*2 + 1]);
        r[2 + i] = _mm_unpacklo_epi64(tmp, tmp2);
        r[i] = p1[i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Convert float64 vector to higher half of int32 vector under mask. */
inline _M512I _mm512_mask_cvth_pd2pi(_M512I v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[8 + i] = _LPP_ConvertFloat64ToInt32(p2[i], rc);
        } else {
            r[8 + i] = p1[8 + i];
        }
        r[i] = p1[i];
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_cvtpd_epi32(p2[i*2 + 0]);
        __m128i tmp2 = _mm_cvtpd_epi32(p2[i*2 + 1]);
        r[2 + i] = _mm_unpacklo_epi64(tmp, tmp2);
        r[i] = p1[i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 2; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[2 + i] = _mm_or_si128(_mm_and_si128(mask, r[2 + i]), _mm_andnot_si128(mask, p1[2 + i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert float64 vector to lower half of float32 vector. */
inline _M512 _mm512_cvtl_pd2ps(_M512 v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertFloat64ToFloat32(p2[i], rc);
        r[8 + i] = p1[8 + i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128 tmp1 = _mm_cvtpd_ps(p2[i*2 + 0]);
        __m128 tmp2 = _mm_cvtpd_ps(p2[i*2 + 1]);
        r[i] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(1, 0, 1, 0));
        r[2 + i] = p1[2 + i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Convert float64 vector to lower half of float32 vector under mask. */
inline _M512 _mm512_mask_cvtl_pd2ps(_M512 v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat64ToFloat32(p2[i], rc);
        } else {
            r[i] = p1[i];
        }
        r[8 + i] = p1[8 + i];
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128 tmp1 = _mm_cvtpd_ps(p2[i*2 + 0]);
        __m128 tmp2 = _mm_cvtpd_ps(p2[i*2 + 1]);
        r[i] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(1, 0, 1, 0));
        r[2 + i] = p1[2 + i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 2; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert float64 vector to higher half of float32 vector. */
inline _M512 _mm512_cvth_pd2ps(_M512 v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[8 + i] = _LPP_ConvertFloat64ToFloat32(p2[i], rc);
        r[i] = p1[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128 tmp1 = _mm_cvtpd_ps(p2[i*2 + 0]);
        __m128 tmp2 = _mm_cvtpd_ps(p2[i*2 + 1]);
        r[2 + i] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(1, 0, 1, 0));
        r[i] = p1[i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Convert float64 vector to higher half of float32 vector under mask. */
inline _M512 _mm512_mask_cvth_pd2ps(_M512 v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[8 + i] = _LPP_ConvertFloat64ToFloat32(p2[i], rc);
        } else {
            r[8 + i] = p1[8 + i];
        }
        r[i] = p1[i];
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;

    _LPP_SetSSERoundingMode(rc);
    for (int i=0; i < 2; i++) {
        __m128 tmp1 = _mm_cvtpd_ps(p2[i*2 + 0]);
        __m128 tmp2 = _mm_cvtpd_ps(p2[i*2 + 1]);
        r[2 + i] = _mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(1, 0, 1, 0));
        r[i] = p1[i];
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 2; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[2 + i] = _mm_or_ps(_mm_and_ps(mask, r[2 + i]), _mm_andnot_ps(mask, p1[2 + i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert float64 vector to lower half of uint32 vector. */
inline _M512I _mm512_cvtl_pd2pu(_M512I v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertFloat64ToUInt32(p2[i], rc);
        r[8 + i] = p1[8 + i];
    }
    return result;
}

/* Convert float64 vector to lower half of uint32 vector under mask. */
inline _M512I _mm512_mask_cvtl_pd2pu(_M512I v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat64ToUInt32(p2[i], rc);
        } else {
            r[i] = p1[i];
        }
        r[8 + i] = p1[8 + i];
        k1 >>= 1;
    }
    return result;
}

/* Convert float64 vector to higher half of uint32 vector. */
inline _M512I _mm512_cvth_pd2pu(_M512I v1_old, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[8 + i] = _LPP_ConvertFloat64ToUInt32(p2[i], rc);
        r[i] = p1[i];
    }
    return result;
}

/* Convert float64 vector to higher half of uint32 vector under mask. */
inline _M512I _mm512_mask_cvth_pd2pu(_M512I v1_old, __mmask k1, _M512D v2, _MM_ROUND_MODE_ENUM rc)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[8 + i] = _LPP_ConvertFloat64ToUInt32(p2[i], rc);
        } else {
            r[8 + i] = p1[8 + i];
        }
        r[i] = p1[i];
        k1 >>= 1;
    }
    return result;
}

/* Convert lower half of int32 vector to float64 vector. */
inline _M512D _mm512_cvtl_pi2pd(_M512I v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    int *p2 = (int *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertInt32ToFloat64(p2[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_shuffle_epi32(p2[i], _MM_SHUFFLE(3, 2, 3, 2));
        r[i*2 + 0] = _mm_cvtepi32_pd(p2[i]);
        r[i*2 + 1] = _mm_cvtepi32_pd(tmp);
    }
#endif
    return result;
}

/* Convert lower half of int32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvtl_pi2pd(_M512D v1_old, __mmask k1, _M512I v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    int *p2 = (int *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertInt32ToFloat64(p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_shuffle_epi32(p2[i], _MM_SHUFFLE(3, 2, 3, 2));
        r[i*2 + 0] = _mm_cvtepi32_pd(p2[i]);
        r[i*2 + 1] = _mm_cvtepi32_pd(tmp);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Convert higher half of int32 vector to float64 vector. */
inline _M512D _mm512_cvth_pi2pd(_M512I v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    int *p2 = (int *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertInt32ToFloat64(p2[8 + i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_shuffle_epi32(p2[2 + i], _MM_SHUFFLE(3, 2, 3, 2));
        r[i*2 + 0] = _mm_cvtepi32_pd(p2[2 + i]);
        r[i*2 + 1] = _mm_cvtepi32_pd(tmp);
    }
#endif
    return result;
}

/* Convert higher half of int32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvt_pi2pd(_M512D v1_old, __mmask k1, _M512I v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    int *p2 = (int *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertInt32ToFloat64(p2[8 + i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 2; i++) {
        __m128i tmp = _mm_shuffle_epi32(p2[2 + i], _MM_SHUFFLE(3, 2, 3, 2));
        r[i*2 + 0] = _mm_cvtepi32_pd(p2[2 + i]);
        r[i*2 + 1] = _mm_cvtepi32_pd(tmp);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Convert int32 vector to float32 vector. */
inline _M512 _mm512_cvt_pi2ps(_M512I v2, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_ConvertInt32ToFloat32(p2[i], expadj);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_cvtepi32_ps(p2[i]);
    }
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjIntToFloatTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_mul_ps(r[i], adjust);
        }
    }
#endif
    return result;
}

/* Convert int32 vector to float32 vector under mask. */
inline _M512 _mm512_mask_cvt_pi2ps(_M512 v1_old, __mmask k1, _M512I v2, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertInt32ToFloat32(p2[i], expadj);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_cvtepi32_ps(p2[i]);
    }
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjIntToFloatTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_mul_ps(r[i], adjust);
        }
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert lower half of float32 vector to float64 vector. */
inline _M512D _mm512_cvtl_ps2pd(_M512 v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertFloat32ToFloat64(p2[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 2; i++) {
        r[i*2 + 0] = _mm_cvtps_pd(p2[i]);
        r[i*2 + 1] = _mm_cvtps_pd(_mm_shuffle_ps(p2[i], p2[i], _MM_SHUFFLE(1, 0, 3, 2)));
    }
#endif
    return result;
}

/* Convert lower half of float32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvtl_ps2pd(_M512D v1_old, __mmask k1, _M512 v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat32ToFloat64(p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 2; i++) {
        r[i*2 + 0] = _mm_cvtps_pd(p2[i]);
        r[i*2 + 1] = _mm_cvtps_pd(_mm_shuffle_ps(p2[i], p2[i], _MM_SHUFFLE(1, 0, 3, 2)));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Convert higher half of float32 vector to float64 vector. */
inline _M512D _mm512_cvth_ps2pd(_M512 v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertFloat32ToFloat64(p2[8 + i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 2; i++) {
        r[i*2 + 0] = _mm_cvtps_pd(p2[2 + i]);
        r[i*2 + 1] = _mm_cvtps_pd(_mm_shuffle_ps(p2[2 + i], p2[2 + i], _MM_SHUFFLE(1, 0, 3, 2)));
    }
#endif
    return result;
}

/* Convert higher half of float32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvth_ps2pd(_M512D v1_old, __mmask k1, _M512 v2)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat32ToFloat64(p2[8 + i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;

    for (int i=0; i < 2; i++) {
        r[i*2 + 0] = _mm_cvtps_pd(p2[2 + i]);
        r[i*2 + 1] = _mm_cvtps_pd(_mm_shuffle_ps(p2[2 + i], p2[2 + i], _MM_SHUFFLE(1, 0, 3, 2)));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Convert float32 vector to int32 vector. */
inline _M512I _mm512_cvt_ps2pi(_M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_ConvertFloat32ToInt32(p2[i], rc, expadj);
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128 *p2 = (__m128 *) &v2;

    _LPP_SetSSERoundingMode(rc);
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjFloatToIntTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtps_epi32(_mm_mul_ps(p2[i], adjust));
        }
    } else {
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtps_epi32(p2[i]);
        }
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Convert float32 vector to int32 vector under mask. */
inline _M512I _mm512_mask_cvt_ps2pi(_M512I v1_old, __mmask k1, _M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat32ToInt32(p2[i], rc, expadj);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;

    _LPP_SetSSERoundingMode(rc);
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjFloatToIntTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtps_epi32(_mm_mul_ps(p2[i], adjust));
        }
    } else {
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtps_epi32(p2[i]);
        }
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Convert float32 vector to uint32 vector. */
inline _M512I _mm512_cvt_ps2pu(_M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512I result;

    int *r = (int *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_ConvertFloat32ToUInt32(p2[i], rc, expadj);
    }
    return result;
}

/* Convert float32 vector to uint32 vector under mask. */
inline _M512I _mm512_mask_cvt_ps2pu(_M512I v1_old, __mmask k1, _M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertFloat32ToUInt32(p2[i], rc, expadj);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert float32 vector to SRGB8 vector. */
inline _M512 _mm512_cvt_ps2srgb8(_M512 v2)
{
    _M512 result;

    int *r = (int *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = (int) _LPP_ConvertFloat32ToSRgb8(p2[i]);
    }
    return result;
}

/* Convert float32 vector to SRGB8 vector under mask. */
inline _M512 _mm512_mask_cvt_ps2srgb8(_M512 v1_old, __mmask k1, _M512 v2)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (int) _LPP_ConvertFloat32ToSRgb8(p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert lower half of uint32 vector to float64 vector. */
inline _M512D _mm512_cvtl_pu2pd(_M512I v2)
{
    _M512D result;

    double *r = (double *) &result;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertUInt32ToFloat64(p2[i]);
    }
    return result;
}

/* Convert lower half of uint32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvtl_pu2pd(_M512D v1_old, __mmask k1, _M512I v2)
{
    _M512D result;

    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertUInt32ToFloat64(p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert higher half of uint32 vector to float64 vector. */
inline _M512D _mm512_cvth_pu2pd(_M512I v2)
{
    _M512D result;

    double *r = (double *) &result;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_ConvertUInt32ToFloat64(p2[8 + i]);
    }
    return result;
}

/* Convert higher half of uint32 vector to float64 vector under mask. */
inline _M512D _mm512_mask_cvth_pu2pd(_M512D v1_old, __mmask k1, _M512I v2)
{
    _M512D result;

    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertUInt32ToFloat64(p2[8 + i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Convert uint32 vector to float32 vector. */
inline _M512 _mm512_cvt_pu2ps(_M512I v2, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

    float *r = (float *) &result;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_ConvertUInt32ToFloat32(p2[i], expadj);
    }
    return result;
}

/* Convert uint32 vector to float32 vector under mask. */
inline _M512 _mm512_mask_cvt_pu2ps(_M512 v1_old, __mmask k1, _M512I v2, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_ConvertUInt32ToFloat32(p2[i], expadj);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Fix up special float32 vector numbers under mask. */
inline _M512 _mm512_mask_fixup_ps(_M512 v1, __mmask k1, _M512 v2, _MM_FIXUPTABLE_ENUM table)
{
    _M512 result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        int type;

        if (k1 & 1) {
            if (_LPP_isINF(*(float *) &p2[i]) && (p2[i] & 0x80000000)) type = 0;
            else if (p2[i] == 0x80000000)                              type = 2;
            else if (p2[i] == 0)                                       type = 3;
            else if (_LPP_isINF(*(float *) &p2[i]))                    type = 5;
            else if (_LPP_isNaN(*(float *) &p2[i]))                    type = 6;
            else if (p2[i] & 0x80000000)                               type = 1;
            else                                                       type = 4;

            int conv = ((table >> (type * 3)) & 0x07);
            switch (conv) {
                case 0:     // NO_CHANGE_TOKEN
                    r[i] = p2[i];
                break;
                case 1:     // NEG_INF_TOKEN
                    r[i] = 0xff800000;
                break;
                case 2:     // NEG_ZERO_TOKEN
                    r[i] = 0x80000000;
                break;
                case 3:     // POS_ZERO_TOKEN
                    r[i] = 0x00000000;
                break;
                case 4:     // POS_INF_TOKEN
                    r[i] = 0x7f800000;
                break;
                case 5:     // NAN_TOKEN
                    r[i] = 0xffc00000;
                break;
                case 6:     // MAX_FLOAT_TOKEN
                    r[i] = 0x7f7fffff;
                break;
                case 7:     // MIN_FLOAT_TOKEN
                    r[i] = 0xff7fffff;
                break;
            }
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Fix up special float32 vector numbers. */
inline _M512 _mm512_fixup_ps(_M512 v1, _M512 v2, _MM_FIXUPTABLE_ENUM table)
{
    return _mm512_mask_fixup_ps(v1, 0xFFFF, v2, table);
}

/* Gather all elements vector under mask. */
inline _M512 _mm512_mask_gatherd(_M512 v1_old, __mmask k1, _M512I index, void *m, _MM_FULLUP32_ENUM up_conv, _MM_INDEX_SCALE_ENUM scale, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *idx = (int *) &index;
    char *src = (char *) m;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_FullUpConv((void *) (src + idx[i]*scale), up_conv);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }

    return result;
}

/* Gather all elements vector. */
inline _M512 _mm512_gatherd(_M512I index, void *m, _MM_FULLUP32_ENUM up_conv, _MM_INDEX_SCALE_ENUM scale, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    
    int *r = (int *) &result;
    int *idx = (int *) &index;
    char *src = (char *) m;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_FullUpConv((void *) (src + idx[i]*scale), up_conv);
    }

    return result;
}

inline void _mm512_gatherpfd(_M512I, void *, _MM_FULLUP32_ENUM, _MM_INDEX_SCALE_ENUM, _MM_MEM_HINT_ENUM)
{
    // No operation.
}
                                   
inline void _mm512_mask_gatherpfd(_M512I, __mmask, void *, _MM_FULLUP32_ENUM, _MM_INDEX_SCALE_ENUM, _MM_MEM_HINT_ENUM)
{
    // No operation.
}

/* Extract float32 vector of exponents. */
inline _M512 _mm512_getexp_ps(_M512 v2)
{
    _M512 result;

    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    int exponent;

    for (int i=0; i < 16; i++) {
        (void) frexp(p2[i], &exponent);
        r[i] = (float) (exponent - 1);
    }
    return result;
}

/* Extract float32 vector of exponents under mask. */
inline _M512 _mm512_mask_getexp_ps(_M512 v1_old, __mmask k1, _M512 v2)
{
    _M512 result;

    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    int exponent;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            (void) frexp(p2[i], &exponent);
            r[i] = (float) (exponent - 1);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Rotate int32 vector and bitfield-insert into int32 vector. */
inline _M512I _mm512_insertfield_pi(_M512I v2, _M512I v3, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    for (int i=0; i < 16; i++) {
        unsigned int rotated = (p3[i] << rotation) | (p3[i] >> (32 - rotation));
        r[i] = (p2[i] & ~mask) | (rotated & mask);
    }
    return result;
}

/* Rotate int32 vector and bitfield-insert into int32 vector under mask. */
inline _M512I _mm512_mask_insertfield_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            unsigned int rotated = (p3[i] << rotation) | (p3[i] >> (32 - rotation));
            r[i] = (p2[i] & ~mask) | (rotated & mask);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Load doubleword vector. */
inline _M512 _mm512_loadd(void *m, _MM_FULLUP32_ENUM full_up, _MM_BROADCAST32_ENUM broadcast, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    int size, i;
    
    int *r = (int *) &result;

    size = _LPP_FullUpConvSize(full_up);

    switch(broadcast) {
        case _MM_BROADCAST_16X16:
            for (i=0; i < 16; i++) {
                r[i] = _LPP_FullUpConv((void *) ((char *) m + i*size), full_up);
            }
       break;
        case _MM_BROADCAST_1X16:
            for (i=0; i < 16; i++) {
                r[i] = _LPP_FullUpConv(m, full_up);
            }
        break;
        case _MM_BROADCAST_4X16:
            for (i=0; i < 16; i++) {
                r[i] = _LPP_FullUpConv((void *) ((char *) m + (i % 4) * size), full_up);
            }
        break;
    }
    return result;
}

/* Load doubleword vector under mask. */
inline _M512 _mm512_mask_loadd(_M512 v1_old, __mmask k1, void *m, _MM_FULLUP32_ENUM full_up, _MM_BROADCAST32_ENUM broadcast, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    int size, i;
    
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;

    size = _LPP_FullUpConvSize(full_up);

    switch(broadcast) {
        case _MM_BROADCAST_16X16:
            for (i=0; i < 16; i++) {
                if (k1 & 1) {
                    r[i] = _LPP_FullUpConv((void *) ((char *) m + i*size), full_up);
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
       break;
        case _MM_BROADCAST_1X16:
            for (i=0; i < 16; i++) {
                if (k1 & 1) {
                    r[i] = _LPP_FullUpConv(m, full_up);
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
        break;
        case _MM_BROADCAST_4X16:
            for (i=0; i < 16; i++) {
                if (k1 & 1) {
                    r[i] = _LPP_FullUpConv((void *) ((char *) m + (i % 4) * size), full_up);
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
        break;
    }
    return result;
}

/* Load quadword vector. */
inline _M512 _mm512_loadq(void *m, _MM_FULLUP64_ENUM, _MM_BROADCAST64_ENUM broadcast, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    
    int64_t *r = (int64_t *) &result;
    int64_t *src = (int64_t *) m;

    switch(broadcast) {
        case _MM_BROADCAST_8X8:
            for (int i=0; i < 8; i++) {
                r[i] = src[i];
            }
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 8; i++) {
                r[i] = src[0];
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 8; i++) {
                r[i] = src[i % 4];
            }
        break;
    }
    return result;
}

/* Load quadword vector under mask. */
inline _M512 _mm512_mask_loadq(_M512 v1_old, __mmask k1, void *m, _MM_FULLUP64_ENUM, _MM_BROADCAST64_ENUM broadcast, _MM_MEM_HINT_ENUM)
{
    _M512 result;
    
    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *src = (int64_t *) m;

    switch(broadcast) {
        case _MM_BROADCAST_8X8:
            for (int i=0; i < 8; i++) {
                if (k1 & 1) {
                    r[i] = src[i];
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
        break;
        case _MM_BROADCAST_1X8:
            for (int i=0; i < 8; i++) {
                if (k1 & 1) {
                    r[i] = src[0];
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
        break;
        case _MM_BROADCAST_4X8:
            for (int i=0; i < 8; i++) {
                if (k1 & 1) {
                    r[i] = src[i % 4];
                } else {
                    r[i] = p1[i];
                }
                k1 >>= 1;
            }
        break;
    }
    return result;
}

/* Load unaligned and expand to doubleword vector. */
inline _M512 _mm512_expandd(void *m, _MM_FULLUP32_ENUM full_up, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int *r = (int *) &result;
    char *src = (char *) m;
    int size = _LPP_FullUpConvSize(full_up);

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_FullUpConv((void *) src, full_up);
        src += size;
    }
    return result;
}

/* Load unaligned and expand to doubleword vector under mask. */
inline _M512 _mm512_mask_expandd(_M512 v1_old, __mmask k1, void *m, _MM_FULLUP32_ENUM full_up, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    char *src = (char *) m;
    int size = _LPP_FullUpConvSize(full_up);

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_FullUpConv((void *) src, full_up);
            src += size;
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Load unaligned and expand to quadword vector. */
inline _M512 _mm512_expandq(void *m, _MM_FULLUP64_ENUM, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int64_t *r = (int64_t *) &result;
    int64_t *src = (int64_t *) m;

    for (int i=0; i < 8; i++) {
        r[i] = *src++;
    }
    return result;
}

/* Load unaligned and expand to quadword vector under mask. */
inline _M512 _mm512_mask_expandq(_M512 v1_old, __mmask k1, void *m, _MM_FULLUP64_ENUM, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *src = (int64_t *) m;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = *src++;
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply and add float64 vectors. */
inline _M512D _mm512_madd132_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p1[i] * p3[i]) + p2[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]);
    }
#endif
    return result;
}

/* Multiply and add float64 vectors under mask. */
inline _M512D _mm512_mask_madd132_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p1[i] * p3[i]) + p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and add float32 vectors. */
inline _M512 _mm512_madd132_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p1[i] * p3[i]) + p2[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]);
    }
#endif
    return result;
}

/* Multiply and add float32 vectors under mask. */
inline _M512 _mm512_mask_madd132_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p1[i] * p3[i]) + p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and add float64 vectors. */
inline _M512D _mm512_madd213_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p2[i] * p1[i]) + p3[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]);
    }
#endif
    return result;
}

/* Multiply and add float64 vectors under mask. */
inline _M512D _mm512_mask_madd213_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p1[i]) + p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and add float32 vectors. */
inline _M512 _mm512_madd213_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p1[i]) + p3[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]);
    }
#endif
    return result;
}

/* Multiply and add float32 vectors under mask. */
inline _M512 _mm512_mask_madd213_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p1[i]) + p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and add float64 vectors. */
inline _M512D _mm512_madd231_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p2[i] * p3[i]) + p1[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]);
    }
#endif
    return result;
}

/* Multiply and add float64 vectors under mask. */
inline _M512D _mm512_mask_madd231_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[i]) + p1[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and add int32 vectors. */
inline _M512I _mm512_madd231_pi(_M512I v1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p3[i]) + p1[i];
    }
    return result;
}

/* Multiply and add int32 vectors under mask. */
inline _M512I _mm512_mask_madd231_pi(_M512I v1, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[i]) + p1[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply and add float32 vectors. */
inline _M512 _mm512_madd231_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p3[i]) + p1[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]);
    }
#endif
    return result;
}

/* Multiply and add float32 vectors under mask. */
inline _M512 _mm512_mask_madd231_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[i]) + p1[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and add int32 vectors. */
inline _M512I _mm512_madd233_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p3[(i & 0x0C) + 1]) + p3[(i & 0x0C) + 0];
    }
    return result;
}

/* Multiply and add int32 vectors under mask. */
inline _M512I _mm512_mask_madd233_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[(i & 0x0C) + 1]) + p3[(i & 0x0C) + 0];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply and add float32 vectors. */
inline _M512 _mm512_madd233_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p3[(i & 0x0C) + 1]) + p3[(i & 0x0C) + 0];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        __m128 scale = _mm_shuffle_ps(p3[i], p3[i], _MM_SHUFFLE(1, 1, 1, 1));
        __m128 bias = _mm_shuffle_ps(p3[i], p3[i], _MM_SHUFFLE(0, 0, 0, 0));
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], scale), bias);
    }
#endif
    return result;
}

/* Multiply and add float32 vectors under mask. */
inline _M512 _mm512_mask_madd233_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[(i & 0x0C) + 1]) + p3[(i & 0x0C) + 0];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        __m128 scale = _mm_shuffle_ps(p3[i], p3[i], _MM_SHUFFLE(1, 1, 1, 1));
        __m128 bias = _mm_shuffle_ps(p3[i], p3[i], _MM_SHUFFLE(0, 0, 0, 0));
        r[i] = _mm_add_ps(_mm_mul_ps(p2[i], scale), bias);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors. */
inline _M512D _mm512_maddn132_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = -((p1[i] * p3[i]) + p2[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors under mask. */
inline _M512D _mm512_mask_maddn132_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = -((p1[i] * p3[i]) + p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors. */
inline _M512 _mm512_maddn132_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = -((p1[i] * p3[i]) + p2[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors under mask. */
inline _M512 _mm512_mask_maddn132_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = -((p1[i] * p3[i]) + p2[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors. */
inline _M512D _mm512_maddn213_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = -((p2[i] * p1[i]) + p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors under mask. */
inline _M512D _mm512_mask_maddn213_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = -((p2[i] * p1[i]) + p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors. */
inline _M512 _mm512_maddn213_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = -((p2[i] * p1[i]) + p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors under mask. */
inline _M512 _mm512_mask_maddn213_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = -((p2[i] * p1[i]) + p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors. */
inline _M512D _mm512_maddn231_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = -((p2[i] * p3[i]) + p1[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float64 vectors under mask. */
inline _M512D _mm512_mask_maddn231_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = -((p2[i] * p3[i]) + p1[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d zero = _mm_setzero_pd();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(zero, _mm_add_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors. */
inline _M512 _mm512_maddn231_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = -((p2[i] * p3[i]) + p1[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]));
    }
#endif
    return result;
}

/* Multiply, add and negate float32 vectors under mask. */
inline _M512 _mm512_mask_maddn231_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = -((p2[i] * p3[i]) + p1[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 zero = _mm_setzero_ps();

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(zero, _mm_add_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Absolute maximum of float32 vectors. */
inline _M512 _mm512_maxabs_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_max(fabs(p2[i]), fabs(p3[i]));
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128i absMask = _mm_set1_epi32(0x7FFFFFFF);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_ps(_mm_and_ps(*(__m128 *) &absMask, p2[i]), _mm_and_ps(*(__m128 *) &absMask, p3[i]));
    }
#endif
    return result;
}

/* Absolute maximum of float32 vectors under mask. */
inline _M512 _mm512_mask_maxabs_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_max(fabs(p2[i]), fabs(p3[i]));
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128i absMask = _mm_set1_epi32(0x7FFFFFFF);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_ps(_mm_and_ps(*(__m128 *) &absMask, p2[i]), _mm_and_ps(*(__m128 *) &absMask, p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Maximum of float64 vectors. */
inline _M512D _mm512_max_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_max(p2[i], p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_pd(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Maximum of float64 vectors under mask. */
inline _M512D _mm512_mask_max_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_max(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_pd(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Maximum of int32 vectors. */
inline _M512I _mm512_max_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_max(p2[i], p3[i]);
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p2[i]), _mm_andnot_si128(tmp, p3[i]));
    }
#endif
    return result;
}

/* Maximum of int32 vectors under mask. */
inline _M512I _mm512_mask_max_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_max(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p2[i]), _mm_andnot_si128(tmp, p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Maximum of float32 vectors. */
inline _M512 _mm512_max_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_max(p2[i], p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_ps(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Maximum of float32 vectors under mask. */
inline _M512 _mm512_mask_max_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_max(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_max_ps(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Maximum of uint32 vectors. */
inline _M512I _mm512_max_pu(_M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_max(p2[i], p3[i]);
    }
    return result;
}

/* Maximum of uint32 vectors under mask. */
inline _M512I _mm512_mask_max_pu(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_max(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Minimum of float64 vectors. */
inline _M512D _mm512_min_pd(_M512D v2,_M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = _LPP_min(p2[i], p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_min_pd(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Minimum of float64 vectors under mask. */
inline _M512D _mm512_mask_min_pd(_M512D v1_old, __mmask k1, _M512D v2,_M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _LPP_min(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_min_pd(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Minimum of int32 vectors. */
inline _M512I _mm512_min_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_min(p2[i], p3[i]);
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p3[i]), _mm_andnot_si128(tmp, p2[i]));
    }
#endif
    return result;
}

/* Minimum of int32 vectors under mask. */
inline _M512I _mm512_mask_min_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_min(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpgt_epi32(p2[i], p3[i]);
        r[i] = _mm_or_si128(_mm_and_si128(tmp, p3[i]), _mm_andnot_si128(tmp, p2[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Minimum of float32 vectors. */
inline _M512 _mm512_min_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_min(p2[i], p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_min_ps(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Minimum of float32 vectors under mask. */
inline _M512 _mm512_mask_min_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_min(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_min_ps(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Minimum of uint32 vectors. */
inline _M512I _mm512_min_pu(_M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = _LPP_min(p2[i], p3[i]);
    }
    return result;
}

/* Move one vector to another. */
inline _M512 _mm512_mov(_M512 v2)
{
    return v2;
}

/* Move a doubleword vector to another under mask. */
inline _M512 _mm512_mask_movd(_M512 v1_old, __mmask k1, _M512 v2)
{
    _M512 result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Move a quadword vector to another under mask. */
inline _M512 _mm512_mask_movq(_M512 v1_old, __mmask k1, _M512 v2)
{
    _M512 result;

    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *p2 = (int64_t *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Minimum of uint32 vectors under mask. */
inline _M512I _mm512_mask_min_pu(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _LPP_min(p2[i], p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msub132_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p1[i] * p3[i]) - p2[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msub132_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p1[i] * p3[i]) - p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p1[i], p3[i]), p2[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msub132_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p1[i] * p3[i]) - p2[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msub132_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p1[i] * p3[i]) - p2[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p1[i], p3[i]), p2[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msub213_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p2[i] * p1[i]) - p3[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msub213_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p1[i]) - p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p2[i], p1[i]), p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msub213_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p1[i]) - p3[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msub213_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p1[i]) - p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p2[i], p1[i]), p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msub231_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = (p2[i] * p3[i]) - p1[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msub231_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[i]) - p1[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(_mm_mul_pd(p2[i], p3[i]), p1[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msub231_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] * p3[i]) - p1[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]);
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msub231_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (p2[i] * p3[i]) - p1[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(_mm_mul_ps(p2[i], p3[i]), p1[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msubr132_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p2[i] - (p1[i] * p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p2[i], _mm_mul_pd(p1[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msubr132_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] - (p1[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p2[i], _mm_mul_pd(p1[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msubr132_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] - (p1[i] * p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p2[i], _mm_mul_ps(p1[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msubr132_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] - (p1[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p2[i], _mm_mul_ps(p1[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msubr213_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p3[i] - (p2[i] * p1[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p3[i], _mm_mul_pd(p2[i], p1[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msubr213_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p3[i] - (p2[i] * p1[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p3[i], _mm_mul_pd(p2[i], p1[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msubr213_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p3[i] - (p2[i] * p1[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p3[i], _mm_mul_ps(p2[i], p1[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msubr213_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p3[i] - (p2[i] * p1[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p3[i], _mm_mul_ps(p2[i], p1[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors. */
inline _M512D _mm512_msubr231_pd(_M512D v1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p1[i] - (p2[i] * p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p1[i], _mm_mul_pd(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float64 vectors under mask. */
inline _M512D _mm512_mask_msubr231_pd(_M512D v1, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p1[i] - (p2[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p1[i], _mm_mul_pd(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors. */
inline _M512 _mm512_msubr231_ps(_M512 v1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] - (p2[i] * p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p1[i], _mm_mul_ps(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply and subtract float32 vectors under mask. */
inline _M512 _mm512_mask_msubr231_ps(_M512 v1, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] - (p2[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p1[i], _mm_mul_ps(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply float64 vectors and subtract from 1. */
inline _M512D _mm512_msubr23c1_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = 1.0 - (p2[i] * p3[i]);
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d one = _mm_set1_pd(1.0f);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(one, _mm_mul_pd(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply float64 vectors and subtract from 1 under mask. */
inline _M512D _mm512_mask_msubr23c1_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = 1.0 - (p2[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;
    __m128d one = _mm_set1_pd(1.0f);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(one, _mm_mul_pd(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply float32 vectors and subtract from 1. */
inline _M512 _mm512_msubr23c1_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = 1.0f - (p2[i] * p3[i]);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 one = _mm_set1_ps(1.0f);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(one, _mm_mul_ps(p2[i], p3[i]));
    }
#endif
    return result;
}

/* Multiply float32 vectors and substract from 1 under mask. */
inline _M512 _mm512_mask_msubr23c1_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = 1.0f - (p2[i] * p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;
    __m128 one = _mm_set1_ps(1.0f);

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(one, _mm_mul_ps(p2[i], p3[i]));
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Multiply int32 vectors and store high result. */
inline _M512I _mm512_mulh_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        int64_t tmp = p2[i] * p3[i];
        r[i] = (int) (tmp >> 32);
    }
    return result;
}

/* Multiply int32 vectors and store high result under mask. */
inline _M512I _mm512_mask_mulh_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            int64_t tmp = p2[i] * p3[i];
            r[i] = (int) (tmp >> 32);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply uint32 vectors and store high result. */
inline _M512I _mm512_mulh_pu(_M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        uint64_t tmp = p2[i] * p3[i];
        r[i] = (unsigned int) (tmp >> 32);
    }
    return result;
}

/* Multiply uint32 vectors and store high result under mask. */
inline _M512I _mm512_mask_mulh_pu(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            int64_t tmp = p2[i] * p3[i];
            r[i] = (unsigned int) (tmp >> 32);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply int32 vectors and store low result. */
inline _M512I _mm512_mull_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        int64_t tmp = p2[i] * p3[i];
        r[i] = (int) (tmp & 0xFFFFFFFF);
    }
    return result;
}

/* Multiply int32 vectors and store low result under mask. */
inline _M512I _mm512_mask_mull_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            int64_t tmp = p2[i] * p3[i];
            r[i] = (int) (tmp & 0xFFFFFFFF);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Multiply float64 vectors. */
inline _M512D _mm512_mul_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p2[i] * p3[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_mul_pd(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Multiply float64 vectors under mask. */
inline _M512D _mm512_mask_mul_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] * p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_mul_pd(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Multiply float32 vectors. */
inline _M512 _mm512_mul_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] * p3[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_mul_ps(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Multiply float32 vectors under mask. */
inline _M512 _mm512_mask_mul_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] * p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_mul_ps(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Bitwise OR int32 vectors. */
inline _M512I _mm512_or_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] | p3[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_or_si128(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Bitwise OR int32 vectors under mask. */
inline _M512I _mm512_mask_or_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] | p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_or_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Bitwise OR int64 vectors. */
inline _M512I _mm512_or_pq(_M512I v2, _M512I v3)
{
    return _mm512_or_pi(v2, v3);
}

/* Bitwise OR int64 vectors under mask. */
inline _M512I _mm512_mask_or_pq(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *p2 = (int64_t *) &v2;
    int64_t *p3 = (int64_t *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] | p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_or_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Compress and store unaligned from doubleword vector. */
inline void _mm512_compressd(void *m, _M512 v2, _MM_DOWNCONV32_ENUM down_conv, _MM_MEM_HINT_ENUM)
{
    int *p2 = (int *) &v2;
    char *dstB;
    short *dstW;
    int *dstD;
    int size = _LPP_DownConvSize(down_conv);

    switch(size) {
        case 1:
            dstB = (char *) m;
            for (int i=0; i < 16; i++) {
                *dstB++ = _LPP_DownConvByte(p2[i], down_conv);
            }
        break;
        case 2:
            dstW = (short *) m;
            for (int i=0; i < 16; i++) {
                *dstW++ = _LPP_DownConvWord(p2[i], down_conv);
            }
        break;
        case 4:
            dstD = (int *) m;
            for (int i=0; i < 16; i++) {
                *dstD++ = p2[i];
            }
        break;
    }
}

/* Compress and store unaligned from doubleword vector under mask. */
inline void _mm512_mask_compressd(void *m, __mmask k1, _M512 v2, _MM_DOWNCONV32_ENUM down_conv, _MM_MEM_HINT_ENUM)
{
    int *p2 = (int *) &v2;
    char *dstB;
    short *dstW;
    int *dstD;
    int size = _LPP_DownConvSize(down_conv);

    switch(size) {
        case 1:
            dstB = (char *) m;
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *dstB++ = _LPP_DownConvByte(p2[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 2:
            dstW = (short *) m;
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *dstW++ = _LPP_DownConvWord(p2[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 4:
            dstD = (int *) m;
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *dstD++ = p2[i];
                }
                k1 >>= 1;
            }
        break;
    }
}

/* Compress and store unaligned from quadword vector. */
inline void _mm512_compressq(void *m, _M512 v2, _MM_DOWNCONV64_ENUM, _MM_MEM_HINT_ENUM)
{
    int64_t *p2 = (int64_t *) &v2;
    int64_t *dst = (int64_t *) m;

    for (int i=0; i < 16; i++) {
        *dst++ = p2[i];
    }
}

/* Compress and store unaligned from quadword vector under mask. */
inline void _mm512_mask_compressq(void *m, __mmask k1, _M512 v2, _MM_DOWNCONV64_ENUM, _MM_MEM_HINT_ENUM)
{
    int64_t *p2 = (int64_t *) &v2;
    int64_t *dst = (int64_t *) m;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            *dst++ = p2[i];
        }
        k1 >>= 1;
    }
}

/* Rotate and bitfield-mask int32 vector. */
inline _M512I _mm512_rotatefield_pi(_M512I v2, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    for (int i=0; i < 16; i++) {
        unsigned int rotated = (p2[i] << rotation) | (p2[i] >> (32 - rotation));
        r[i] = rotated & mask;
    }
    return result;
}

/* Rotate and bitfield-mask int32 vector under mask. */
inline _M512I _mm512_mask_rotatefield_pi(_M512I v1_old, __mmask k1, _M512I v2, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            unsigned int rotated = (p2[i] << rotation) | (p2[i] >> (32 - rotation));
            r[i] = rotated & mask;
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Round float32 vector. */
inline _M512 _mm512_round_ps(_M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = (float) _LPP_ConvertFloat32ToInt32(p2[i], rc, expadj);
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;

    _LPP_SetSSERoundingMode(rc);
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjFloatToIntTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_mul_ps(p2[i], adjust)));
        }
    } else {
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtepi32_ps(_mm_cvtps_epi32(p2[i]));
        }
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
#endif
    return result;
}

/* Round float32 vector under mask. */
inline _M512 _mm512_mask_round_ps(_M512 v1_old, __mmask k1, _M512 v2, _MM_ROUND_MODE_ENUM rc, _MM_EXP_ADJ_ENUM expadj)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = (float) _LPP_ConvertFloat32ToInt32(p2[i], rc, expadj);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;

    _LPP_SetSSERoundingMode(rc);
    if (expadj != _MM_EXPADJ_NONE) {
        __m128 adjust = _mm_set1_ps(_LPP_expAdjFloatToIntTable[expadj]);
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_mul_ps(p2[i], adjust)));
        }
    } else {
        for (int i=0; i < 4; i++) {
            r[i] = _mm_cvtepi32_ps(_mm_cvtps_epi32(p2[i]));
        }
    }
    _LPP_SetSSERoundingMode(_MM_ROUND_MODE_TOWARD_ZERO);
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Subtract int32 vectors with borrow. */
inline _M512I _mm512_sbb_pi(_M512I v1, __mmask k2, _M512I v3, __mmask *k2_res)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] - p3[i] - (k2 & 1);
        uint64_t tmp = (uint64_t) p1[i] - (uint64_t) p3[i] - (uint64_t) (k2 & 1);
        returnMask |= (((tmp >> 32) & 0x01) << i);
        k2 >>= 1;
    }
    *k2_res = returnMask;
    return result;
}

/* Subtract int32 vectors with borrow under mask. */
inline _M512I _mm512_mask_sbb_pi(_M512I v1, __mmask k1, __mmask k2, _M512I v3, __mmask *k2_res)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] - p3[i] - (k2 & 1);
            uint64_t tmp = (uint64_t) p1[i] - (uint64_t) p3[i] - (uint64_t) (k2 & 1);
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2 & 1) << i);
        }
        k1 >>= 1;
        k2 >>= 1;
    }
    *k2_res = returnMask;
    return result;
}

/* Reverse subtract int32 vectors with borrow. */
inline _M512I _mm512_sbbr_pi(_M512I v1, __mmask k2, _M512I v3, __mmask *k2_res)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p3[i] - p1[i] - (k2 & 1);
        uint64_t tmp = (uint64_t) p3[i] - (uint64_t) p1[i] - (uint64_t) (k2 & 1);
        returnMask |= (((tmp >> 32) & 0x01) << i);
        k2 >>= 1;
    }
    *k2_res = returnMask;
    return result;
}

/* Reverse subtract int32 vectors with borrow under mask. */
inline _M512I _mm512_mask_sbbr_pi(_M512I v1, __mmask k1, __mmask k2, _M512I v3, __mmask *k2_res)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p3[i] - p1[i] - (k2 & 1);
            uint64_t tmp = (uint64_t) p3[i] - (uint64_t) p1[i] - (uint64_t) (k2 & 1);
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2 & 1) << i);
        }
        k1 >>= 1;
        k2 >>= 1;
    }
    *k2_res = returnMask;
    return result;
}

/* Scale float32 vectors. */
inline _M512 _mm512_scale_ps(_M512 v2, _M512 v3)
{
    _M512 result;

    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] * pow(2.0f, p3[i]);
    }
    return result;
}

/* Scale float32 vectors under mask. */
inline _M512 _mm512_mask_scale_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] * pow(2.0f, p3[i]);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Scatter element vector. */
inline void _mm512_scatterd(void *m, _M512I index, _M512 v1, _MM_DOWNCONV32_ENUM down_conv, _MM_INDEX_SCALE_ENUM scale, _MM_MEM_HINT_ENUM)
{
    int *p1 = (int *) &v1;
    int *idx = (int *) &index;
    char *dst = (char *) m;
    int size = _LPP_DownConvSize(down_conv);

    switch(size) {
        case 1:
            for (int i=0; i < 16; i++) {
                *(dst + idx[i]*scale) = _LPP_DownConvByte(p1[i], down_conv);
            }
        break;
        case 2:
            for (int i=0; i < 16; i++) {
                *((short *) (dst + idx[i]*scale)) = _LPP_DownConvWord(p1[i], down_conv);
            }
        break;
        case 4:
            for (int i=0; i < 16; i++) {
                *((int *) (dst + idx[i]*scale)) = p1[i];
            }
        break;
    }
}

/* Scatter element vector under mask. */
inline void _mm512_mask_scatterd(void *m, __mmask k1, _M512I index, _M512 v1, _MM_DOWNCONV32_ENUM down_conv, _MM_INDEX_SCALE_ENUM scale, _MM_MEM_HINT_ENUM)
{
    int *p1 = (int *) &v1;
    int *idx = (int *) &index;
    char *dst = (char *) m;
    int size = _LPP_DownConvSize(down_conv);

    switch(size) {
        case 1:
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *(dst + idx[i]*scale) = _LPP_DownConvByte(p1[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 2:
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *((short *) (dst + idx[i]*scale)) = _LPP_DownConvWord(p1[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 4:
            for (int i=0; i < 16; i++) {
                if (k1 & 1) {
                    *((int *) (dst + idx[i]*scale)) = p1[i];
                }
                k1 >>= 1;
            }
        break;
    }
}

/* Scatter prefetch element vector. */
inline void _mm512_scatterpfd(void *, _M512I, _MM_DOWNCONV32_ENUM, _MM_INDEX_SCALE_ENUM, _MM_MEM_HINT_ENUM)
{
    // No operation.
}

/* Scatter prefetch element vector under mask. */
inline void _mm512_mask_scatterpfd(void *, __mmask, _M512I, _MM_DOWNCONV32_ENUM, _MM_INDEX_SCALE_ENUM, _MM_MEM_HINT_ENUM)
{
    // No operation.
}

/* Shuffle vector dqwords then doublewords. */
inline _M512 _mm512_shuf128x32(_M512 v2, _MM_PERM_ENUM perm128, _MM_PERM_ENUM perm32)
{
    _M512 result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int lanePerm = perm128;

    for (int i=0; i < 4; i++) {
        int wordPerm = perm32;
        for (int j=0; j < 4; j++) {
            r[i*4 + j] = p2[(lanePerm & 0x03)*4 + (wordPerm & 0x03)];
            wordPerm >>= 2;
        }
        lanePerm >>= 2;
    }
    return result;
}

/* Shuffle vector dqwords then doublewords under mask. */
inline _M512 _mm512_mask_shuf128x32(_M512 v1_old, __mmask k1, _M512 v2, _MM_PERM_ENUM perm128, _MM_PERM_ENUM perm32)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int lanePerm = perm128;

    for (int i=0; i < 4; i++) {
        int wordPerm = perm32;
        for (int j=0; j < 4; j++) {
            if (k1 & 1) {
                r[i*4 + j] = p2[(lanePerm & 0x03)*4 + (wordPerm & 0x03)];
            } else {
                r[i*4+j] = p1[i*4 + j];
            }
            wordPerm >>= 2;
            k1 >>= 1;
        }
        lanePerm >>= 2;
    }
    return result;
}

/* Shuffle vector dqwords then doublewords from memory. */
inline _M512 _mm512_shuf128x32_m(void *m, _MM_PERM_ENUM perm128, _MM_PERM_ENUM perm32, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int *r = (int *) &result;
    int *src = (int *) m;
    int lanePerm = perm128;

    for (int i=0; i < 4; i++) {
        int wordPerm = perm32;
        for (int j=0; j < 4; j++) {
            r[i*4 + j] = src[(lanePerm & 0x03)*4 + (wordPerm & 0x03)];
            wordPerm >>= 2;
        }
        lanePerm >>= 2;
    }
    return result;
}

/* Shuffle vector dqwords then doublewords from memory under mask. */
inline _M512 _mm512_mask_shuf128x32_m(_M512 v1_old, __mmask k1, void *m, _MM_PERM_ENUM perm128, _MM_PERM_ENUM perm32, _MM_MEM_HINT_ENUM)
{
    _M512 result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *src = (int *) m;
    int lanePerm = perm128;

    for (int i=0; i < 4; i++) {
        int wordPerm = perm32;
        for (int j=0; j < 4; j++) {
            if (k1 & 1) {
                r[i*4 + j] = src[(lanePerm & 0x03)*4 + (wordPerm & 0x03)];
                wordPerm >>= 2;
            } else {
                r[i*4+j] = p1[i*4 + j];
            }
            k1 >>= 1;
        }
        lanePerm >>= 2;
    }
    return result;
}

/* Shift int32 vector left logical. */
inline _M512I _mm512_sll_pi(_M512I v2,_M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] << (p3[i] & 0x1F);
    }
    return result;
}

/* Shift int32 vector left logical under mask. */
inline _M512I _mm512_mask_sll_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] << (p3[i] & 0x1F);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Shift int32 vector right arithmetic. */
inline _M512I _mm512_sra_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] >> (p3[i] & 0x1F);
    }
    return result;
}

/* Shift int32 vector right arithmetic under mask. */
inline _M512I _mm512_mask_sra_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] >> (p3[i] & 0x1F);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Shift int32 vector right logical. */
inline _M512I _mm512_srl_pi(_M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p2 = (unsigned int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] >> (p3[i] & 0x1F);
    }
    return result;
}

/* Shift int32 vector right logical under mask. */
inline _M512I _mm512_mask_srl_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1_old;
    unsigned int *p2 = (unsigned int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] >> (p3[i] & 0x1F);
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Store doubleword vector. */
inline void _mm512_stored(void *m, _M512 v2, _MM_DOWNCONV32_ENUM down_conv, _MM_STORE_SUBSET32_ENUM subset, _MM_MEM_HINT_ENUM)
{
    int *p2 = (int *) &v2;
    char *dstB;
    short *dstW;
    int *dstD;
    int size = _LPP_DownConvSize(down_conv);
    int elements;

    if (subset == _MM_SUBSET32_16) elements = 16;
    else if (subset == _MM_SUBSET32_4) elements = 4;
    else elements = 1;

    switch(size) {
        case 1:
            dstB = (char *) m;
            for (int i=0; i < elements; i++) {
                dstB[i] = _LPP_DownConvByte(p2[i], down_conv);
            }
        break;
        case 2:
            dstW = (short *) m;
            for (int i=0; i < elements; i++) {
                dstW[i] = _LPP_DownConvWord(p2[i], down_conv);
            }
        break;
        case 4:
            dstD = (int *) m;
            for (int i=0; i < elements; i++) {
                dstD[i] = p2[i];
            }
        break;
    }
}

/* Store doubleword vector under mask. */
inline void _mm512_mask_stored(void *m, __mmask k1, _M512 v2, _MM_DOWNCONV32_ENUM down_conv, _MM_STORE_SUBSET32_ENUM subset, _MM_MEM_HINT_ENUM)
{
    int *p2 = (int *) &v2;
    char *dstB;
    short *dstW;
    int *dstD;
    int size = _LPP_DownConvSize(down_conv);
    int elements;

    if (subset == _MM_SUBSET32_16) elements = 16;
    else if (subset == _MM_SUBSET32_4) elements = 4;
    else elements = 1;

    switch(size) {
        case 1:
            dstB = (char *) m;
            for (int i=0; i < elements; i++) {
                if (k1 & 1) {
                    dstB[i] = _LPP_DownConvByte(p2[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 2:
            dstW = (short *) m;
            for (int i=0; i < elements; i++) {
                if (k1 & 1) {
                    dstW[i] = _LPP_DownConvWord(p2[i], down_conv);
                }
                k1 >>= 1;
            }
        break;
        case 4:
            dstD = (int *) m;
            for (int i=0; i < elements; i++) {
                if (k1 & 1) {
                    dstD[i] = p2[i];
                }
                k1 >>= 1;
            }
        break;
    }
}

/* Store quadword vector. */
inline void _mm512_storeq(void *m, _M512 v2, _MM_DOWNCONV64_ENUM, _MM_STORE_SUBSET64_ENUM subset, _MM_MEM_HINT_ENUM)
{
    int64_t *p2 = (int64_t *) &v2;
    int64_t *dst = (int64_t *) m;
    int elements;

    if (subset == _MM_SUBSET64_8) elements = 8;
    else if (subset == _MM_SUBSET64_4) elements = 4;
    else elements = 1;

    for (int i=0; i < elements; i++) {
        dst[i] = p2[i];
    }
}

/* Store quadword vector under mask. */
inline void _mm512_mask_storeq(void *m, __mmask k1, _M512 v2, _MM_DOWNCONV64_ENUM, _MM_STORE_SUBSET64_ENUM subset, _MM_MEM_HINT_ENUM)
{
    int64_t *p2 = (int64_t *) &v2;
    int64_t *dst = (int64_t *) m;
    int elements;

    if (subset == _MM_SUBSET64_8) elements = 8;
    else if (subset == _MM_SUBSET64_4) elements = 4;
    else elements = 1;

    for (int i=0; i < elements; i++) {
        if (k1 & 1) {
            dst[i] = p2[i];
        }
        k1 >>= 1;
    }
}

/* Subtract float64 vectors. */
inline _M512D _mm512_sub_pd(_M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        r[i] = p2[i] - p3[i];
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Subtract float64 vectors under mask. */
inline _M512D _mm512_mask_sub_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;
    double *p1 = (double *) &v1_old;
    double *p2 = (double *) &v2;
    double *p3 = (double *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] - p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128d *r = (__m128d *) &result;
    __m128d *p1 = (__m128d *) &v1_old;
    __m128d *p2 = (__m128d *) &v2;
    __m128d *p3 = (__m128d *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_pd(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128d mask = *(__m128d *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_pd(_mm_and_pd(mask, r[i]), _mm_andnot_pd(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}

/* Subtract int32 vectors. */
inline _M512I _mm512_sub_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] - p3[i];
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_epi32(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Subtract int32 vectors under mask. */
inline _M512I _mm512_mask_sub_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] - p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_epi32(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Subtract float32 vectors. */
inline _M512 _mm512_sub_ps(_M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p2[i] - p3[i];
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Subtract float32 vectors under mask. */
inline _M512 _mm512_mask_sub_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;
    float *p1 = (float *) &v1_old;
    float *p2 = (float *) &v2;
    float *p3 = (float *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] - p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128 *r = (__m128 *) &result;
    __m128 *p1 = (__m128 *) &v1_old;
    __m128 *p2 = (__m128 *) &v2;
    __m128 *p3 = (__m128 *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_sub_ps(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128 mask = *(__m128 *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_ps(_mm_and_ps(mask, r[i]), _mm_andnot_ps(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Reverse subtract float64 vectors. */
inline _M512D _mm512_subr_pd(_M512D v2, _M512D v3)
{
    return _mm512_sub_pd(v3, v2);
}

/* Reverse subtract float64 vectors under mask. */
inline _M512D _mm512_mask_subr_pd(_M512D v1_old, __mmask k1, _M512D v2, _M512D v3)
{
    return _mm512_mask_sub_pd(v1_old, k1, v3, v2);
}

/* Reverse subtract int32 vectors. */
inline _M512I _mm512_subr_pi(_M512I v2,_M512I v3)
{
    return _mm512_sub_pi(v3, v2);
}

/* Reverse subtract int32 vectors under mask. */
inline _M512I _mm512_mask_subr_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    return _mm512_mask_sub_pi(v1_old, k1, v3, v2);
}

/* Reverse subtract float32 vectors. */
inline _M512 _mm512_subr_ps(_M512 v2,_M512 v3)
{
    return _mm512_sub_ps(v3, v2);
}

/* Reverse subtract float32 vectors under mask. */
inline _M512 _mm512_mask_subr_ps(_M512 v1_old, __mmask k1, _M512 v2, _M512 v3)
{
    return _mm512_mask_sub_ps(v1_old, k1, v3, v2);
}

/* Subtract int32 vectors and set borrow. */
inline _M512I _mm512_subrsetb_pi(_M512I v1, _M512I v3, __mmask *borrow)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p3[i] - p1[i];
        uint64_t tmp = (uint64_t) p3[i] - (uint64_t) p1[i];
        returnMask |= (((tmp >> 32) & 0x01) << i);
    }
    *borrow = returnMask;
    return result;
}

/* Subtract int32 vectors and set borrow under mask. */
inline _M512I _mm512_mask_subrsetb_pi(_M512I v1, __mmask k1, __mmask k2_old, _M512I v3, __mmask *borrow)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p3[i] - p1[i];
            uint64_t tmp = (uint64_t) p3[i] - (uint64_t) p1[i];
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2_old & 1) << i);
        }
        k1 >>= 1;
        k2_old >>= 1;
    }
    *borrow = returnMask;
    return result;
}

/* Subtract int32 vectors and set borrow. */
inline _M512I _mm512_subsetb_pi(_M512I v1, _M512I v3, __mmask *borrow)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] - p3[i];
        uint64_t tmp = (uint64_t) p1[i] - (uint64_t) p3[i];
        returnMask |= (((tmp >> 32) & 0x01) << i);
    }
    *borrow = returnMask;
    return result;
}

/* Subtract int32 vectors and set borrow under mask. */
inline _M512I _mm512_mask_subsetb_pi(_M512I v1, __mmask k1, __mmask k2_old, _M512I v3, __mmask *borrow)
{
    _M512I result;
    __mmask returnMask = 0;

    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p3 = (unsigned int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] - p3[i];
            uint64_t tmp = (uint64_t) p1[i] - (uint64_t) p3[i];
            returnMask |= (((tmp >> 32) & 0x01) << i);
        } else {
            r[i] = p1[i];
            returnMask |= ((k2_old & 1) << i);
        }
        k1 >>= 1;
        k2_old >>= 1;
    }
    *borrow = returnMask;
    return result;
}

/* Logical AND and set vector mask under mask. */
inline __mmask _mm512_mask_test_pi(__mmask k1, _M512I v1, _M512I v2)
{
    __mmask result = 0;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if ((p1[i] & p2[i]) != 0) result |= (1 << i);
    }
#else
    __m128i *p1 = (__m128i *) &v1;
    __m128i *p2 = (__m128i *) &v2;
    __m128i zero = _mm_setzero_si128();

    for (int i=0; i < 4; i++) {
        __m128i tmp = _mm_cmpeq_epi32(zero, _mm_and_si128(p1[i], p2[i]));
        result |= (_mm_movemask_ps(*(__m128 *) &tmp) << i*4);
    }
    result ^= 0xFFFF;
#endif
    return result & k1;
}

/* Logical AND and set vector mask. */
inline __mmask _mm512_test_pi(_M512I v1, _M512I v2)
{
    return _mm512_mask_test_pi(0xFFFF, v1, v2);
}

/* Bitwize XOR int32 vectors. */
inline _M512I _mm512_xor_pi(_M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        r[i] = (p2[i] ^ p3[i]);
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_xor_si128(p2[i], p3[i]);
    }
#endif
    return result;
}

/* Bitwize XOR int32 vectors under mask. */
inline _M512I _mm512_mask_xor_pi(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;
    int *p1 = (int *) &v1_old;
    int *p2 = (int *) &v2;
    int *p3 = (int *) &v3;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p2[i] ^ p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_xor_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable32[k1 & 0x0f];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 4;
    }
#endif
    return result;
}

/* Bitwize XOR int64 vectors. */
inline _M512I _mm512_xor_pq(_M512I v2, _M512I v3)
{
    return _mm512_xor_pi(v2, v3);
}

/* Bitwize XOR int64 vectors under mask. */
inline _M512I _mm512_mask_xor_pq(_M512I v1_old, __mmask k1, _M512I v2, _M512I v3)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;
    int64_t *p1 = (int64_t *) &v1_old;
    int64_t *p2 = (int64_t *) &v2;
    int64_t *p3 = (int64_t *) &v3;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p2[i] ^ p3[i];
        } else {
            r[i] = p1[i];
        }
        k1 >>= 1;
    }
#else
    __m128i *r = (__m128i *) &result;
    __m128i *p1 = (__m128i *) &v1_old;
    __m128i *p2 = (__m128i *) &v2;
    __m128i *p3 = (__m128i *) &v3;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_xor_si128(p2[i], p3[i]);
    }
    for (int i=0; i < 4; i++) {
        __m128i mask = *(__m128i *) &_LPP_maskTable64[k1 & 0x03];
        r[i] = _mm_or_si128(_mm_and_si128(mask, r[i]), _mm_andnot_si128(mask, p1[i]));
        k1 >>= 2;
    }
#endif
    return result;
}


/****
 **** Utility operations
 ****/

/* Conversion from one type to another, no change in value. */
inline _M512 _mm512_castpd_ps(_M512D in)
{
    return (*(_M512 *) &in);
}

/* Conversion from one type to another, no change in value. */
inline _M512I _mm512_castpd_si512(_M512D in)
{
    return (*(_M512I *) &in);
}

/* Conversion from one type to another, no change in value. */
inline _M512D _mm512_castps_pd(_M512 in)
{
    return (*(_M512D *) &in);
}

/* Conversion from one type to another, no change in value. */
inline _M512I _mm512_castps_si512(_M512 in)
{
    return (*(_M512I *) &in);
}

/* Conversion from one type to another, no change in value. */
inline _M512 _mm512_castsi512_ps(_M512I in)
{
    return (*(_M512 *) &in);
}

/* Conversion from one type to another, no change in value. */
inline _M512D _mm512_castsi512_pd(_M512I in)
{
    return (*(_M512D *) &in);
}

/* Return 512 vector with all elements 0. */
inline _M512 _mm512_setzero(void)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;

    for (int i=0; i < 16; i++) {
        *r++ = 0;
    }
#else
    __m128 *r = (__m128 *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_setzero_ps();
    }
#endif

    return result;
}


#define _mm512_setzero_pd() _mm512_castps_pd(_mm512_setzero())
#define _mm512_setzero_ps() _mm512_setzero()
#define _mm512_setzero_pi() _mm512_castps_si512(_mm512_setzero())


/* Return float64 vector initialized with 8 elements of a. */
inline _M512D _mm512_set_pd(double a[8])
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;

    for (int i=0; i < 8; i++) {
        r[i] = a[i];
    }
#else
    __m128d *r = (__m128d *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_pd(a[i*2+1], a[i*2+0]);
    }
#endif

    return result;
}

/* Return float32 vector initialized with 16 elements of a. */
inline _M512 _mm512_set_ps(float a[16])
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;

    for (int i=0; i < 16; i++) {
        r[i] = a[i];
    }
#else
    __m128 *r = (__m128 *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_ps(a[i*4+3], a[i*4+2], a[i*4+1], a[i*4+0]);
    }
#endif

    return result;
}

/* Return int32 vector with initialized with 16 elements of a. */
inline _M512I _mm512_set_pi(int a[16])
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;

    for (int i=0; i < 16; i++) {
        r[i] = a[i];
    }
#else
    __m128i *r = (__m128i *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_epi32(a[i*4+3], a[i*4+2], a[i*4+1], a[i*4+0]);
    }
#endif

    return result;
}

/* Return float64 vector with all 8 elements equal to double a. */
inline _M512D _mm512_set_1to8_pd(double a)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;

    for (int i=0; i < 8; i++) {
        r[i] = a;
    }
#else
    __m128d *r = (__m128d *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set1_pd(a);
    }
#endif

    return result;
}

/* Return int64 vector with all 8 elements equal to long a. */
inline _M512I _mm512_set_1to8_pq(int64_t a)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;

    for (int i=0; i < 8; i++) {
        r[i] = a;
    }
#else
    __m128i *r = (__m128i *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_epi32((int) (a >> 32), (int) (a & 0xFFFFFFFF), (int) (a >> 32), (int) (a & 0xFFFFFFFF));
    }
#endif

    return result;
}

/* Return float32 vector with all 16 elements equal to float a. */
inline _M512 _mm512_set_1to16_ps(float a)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;

    for (int i=0; i < 16; i++) {
        r[i] = a;
    }
#else
    __m128 *r = (__m128 *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set1_ps(a);
    }
#endif

    return result;
}

/* Return int32 vector with all 16 elements equal to int a. */
inline _M512I _mm512_set_1to16_pi(int a)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;

    for (int i=0; i < 16; i++) {
        r[i] = a;
    }
#else
    __m128i *r = (__m128i *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set1_epi32(a);
    }
#endif

    return result;
}

/* Return float64 vector dcbadcba. */
inline _M512D _mm512_set_4to8_pd(double a, double b, double c, double d)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;

    for (int i=0; i < 2; i++) {
        r[i*4+0] = a;
        r[i*4+1] = b;
        r[i*4+2] = c;
        r[i*4+3] = d;
    }
#else
    __m128d *r = (__m128d *) &result;

    for (int i=0; i < 2; i++) {
        r[i*2+0] = _mm_set_pd(b, a);
        r[i*2+1] = _mm_set_pd(d, c);
    }
#endif

    return result;
}

/* Return int64 vector dcbadcba. */
inline _M512I _mm512_set_4to8_pq(int64_t a, int64_t b, int64_t c, int64_t d)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;

    for (int i=0; i < 2; i++) {
        r[i*4+0] = a;
        r[i*4+1] = b;
        r[i*4+2] = c;
        r[i*4+3] = d;
    }
#else
    __m128i *r = (__m128i *) &result;

    for (int i=0; i < 2; i++) {
        r[i*2+0] = _mm_set_epi32((int) (b >> 32), (int) (b & 0xFFFFFFFF), (int) (a >> 32), (int) (a & 0xFFFFFFFF));
        r[i*2+1] = _mm_set_epi32((int) (d >> 32), (int) (d & 0xFFFFFFFF), (int) (c >> 32), (int) (c & 0xFFFFFFFF));
    }
#endif

    return result;
}

/* Return float32 vector dcbadcbadcbadcba. */
inline _M512 _mm512_set_4to16_ps(float a, float b, float c, float d)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;

    for (int i=0; i < 4; i++) {
        r[i*4 + 0] = a;
        r[i*4 + 1] = b;
        r[i*4 + 2] = c;
        r[i*4 + 3] = d;
    }
#else
    __m128 *r = (__m128 *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_ps(d, c, b, a);
    }
#endif

    return result;
}

/* Return int32 vector dcbadcbadcbadcba. */
inline _M512I _mm512_set_4to16_pi(int a, int b, int c, int d)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;

    for (int i=0; i < 4; i++) {
        r[i*4 + 0] = a;
        r[i*4 + 1] = b;
        r[i*4 + 2] = c;
        r[i*4 + 3] = d;
    }
#else
    __m128i *r = (__m128i *) &result;

    for (int i=0; i < 4; i++) {
        r[i] = _mm_set_epi32(d, c, b, a);
    }
#endif

    return result;
}

/* Return float32 vector e15 e14 e13 ... e1 e0 (v15=e15, v14=e14, ..., v0=e0). */
inline _M512 _mm512_set_16to16_ps(float e15, float e14, float e13, float e12, float e11, float e10, float e9, float e8,
                                  float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0)
{
    _M512 result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *r = (float *) &result;

    r[0] = e0;  r[1] = e1;  r[2] = e2;  r[3] = e3;
    r[4] = e4;  r[5] = e5;  r[6] = e6;  r[7] = e7;
    r[8] = e8;  r[9] = e9;  r[10] = e10;  r[11] = e11;
    r[12] = e12;  r[13] = e13;  r[14] = e14;  r[15] = e15;

#else
    __m128 *r = (__m128 *) &result;

    r[0] = _mm_set_ps(e3, e2, e1, e0);
    r[1] = _mm_set_ps(e7, e6, e5, e4);
    r[2] = _mm_set_ps(e11, e10, e9, e8);
    r[3] = _mm_set_ps(e15, e14, e13, e12);
#endif

    return result;
}

/* Return int32 vector e15 e14 e13 ... e1 e0 (v15=e15, v14=e14, ..., v0=e0). */
inline _M512I _mm512_set_16to16_pi(int e15, int e14, int e13, int e12, int e11, int e10, int e9, int e8,
                                   int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *r = (int *) &result;

    r[0] = e0;  r[1] = e1;  r[2] = e2;  r[3] = e3;
    r[4] = e4;  r[5] = e5;  r[6] = e6;  r[7] = e7;
    r[8] = e8;  r[9] = e9;  r[10] = e10;  r[11] = e11;
    r[12] = e12;  r[13] = e13;  r[14] = e14;  r[15] = e15;

#else
    __m128i *r = (__m128i *) &result;

    r[0] = _mm_set_epi32(e3, e2, e1, e0);
    r[1] = _mm_set_epi32(e7, e6, e5, e4);
    r[2] = _mm_set_epi32(e11, e10, e9, e8);
    r[3] = _mm_set_epi32(e15, e14, e13, e12);
#endif

    return result;
}

/* Return float64 vector e7 e6 e5 ... e1 e0 (v7=e7, v6=e6, ..., v0=e0). */
inline _M512D _mm512_set_8to8_pd(double e7, double e6, double e5, double e4, double e3, double e2, double e1, double e0)
{
    _M512D result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *r = (double *) &result;

    r[0] = e0;  r[1] = e1;
    r[2] = e2;  r[3] = e3;
    r[4] = e4;  r[5] = e5;
    r[6] = e6;  r[7] = e7;

#else
    __m128d *r = (__m128d *) &result;

    r[0] = _mm_set_pd(e1, e0);
    r[1] = _mm_set_pd(e3, e2);
    r[2] = _mm_set_pd(e5, e4);
    r[3] = _mm_set_pd(e7, e6);
#endif

    return result;
}

/* Return int64 vector e7 e6 e5 ... e1 e0 (v7=e7, v6=e6, ..., v0=e0). */
inline _M512I _mm512_set_8to8_pq(int64_t e7, int64_t e6, int64_t e5, int64_t e4, int64_t e3, int64_t e2, int64_t e1, int64_t e0)
{
    _M512I result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int64_t *r = (int64_t *) &result;

    r[0] = e0;  r[1] = e1;
    r[2] = e2;  r[3] = e3;
    r[4] = e4;  r[5] = e5;
    r[6] = e6;  r[7] = e7;
#else
    __m128i *r = (__m128i *) &result;

    r[0] = _mm_set_epi32((int) (e1 >> 32), (int) (e1 & 0xFFFFFFFF), (int) (e0 >> 32), (int) (e0 & 0xFFFFFFFF));
    r[1] = _mm_set_epi32((int) (e3 >> 32), (int) (e3 & 0xFFFFFFFF), (int) (e2 >> 32), (int) (e2 & 0xFFFFFFFF));
    r[2] = _mm_set_epi32((int) (e5 >> 32), (int) (e5 & 0xFFFFFFFF), (int) (e4 >> 32), (int) (e4 & 0xFFFFFFFF));
    r[3] = _mm_set_epi32((int) (e7 >> 32), (int) (e7 & 0xFFFFFFFF), (int) (e6 >> 32), (int) (e6 & 0xFFFFFFFF));
#endif

    return result;
}

/****
 **** Math utility operations
 ****/

/* Arc cosine of a float64 vector. */
inline _M512D _mm512_acos_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = acos(p1[i]);
    }
    return result;
}

/* Arc cosine of a float64 vector under mask. */
inline _M512D _mm512_mask_acos_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = acos(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc cosine of a float32 vector. */
inline _M512 _mm512_acos_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = acosf(p1[i]);
    }
    return result;
}

/* Arc cosine of a float32 vector under mask. */
inline _M512 _mm512_mask_acos_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = acosf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc sine of a float64 vector. */
inline _M512D _mm512_asin_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = asin(p1[i]);
    }
    return result;
}

/* Arc sine of a float64 vector under mask. */
inline _M512D _mm512_mask_asin_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = asin(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc sine of a float32 vector. */
inline _M512 _mm512_asin_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = asinf(p1[i]);
    }
    return result;
}

/* Arc sine of a float32 vector under mask. */
inline _M512 _mm512_mask_asin_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = asinf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc tangent of float64 vectors. */
inline _M512D _mm512_atan2_pd(_M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = atan2(p1[i], p2[i]);
    }
    return result;
}

/* Arc tangent of float64 vectors under mask. */
inline _M512D _mm512_mask_atan2_pd(_M512D v0_old, __mmask k1, _M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = atan2(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc tangent of float32 vectors. */
inline _M512 _mm512_atan2_ps(_M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = atan2f(p1[i], p2[i]);
    }
    return result;
}

/* Arc tangent of float32 vectors under mask. */
inline _M512 _mm512_mask_atan2_ps(_M512 v0_old, __mmask k1, _M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = atan2f(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc tangent of a float64 vector. */
inline _M512D _mm512_atan_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = atan(p1[i]);
    }
    return result;
}

/* Arc tangent of a float64 vector under mask. */
inline _M512D _mm512_mask_atan_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = atan(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Arc tangent of a float32 vector. */
inline _M512 _mm512_atan_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = atanf(p1[i]);
    }
    return result;
}

/* Arc tangent of a float32 vector under mask. */
inline _M512 _mm512_mask_atan_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = atanf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Round float64 vector to nearest upper integer. */
inline _M512D _mm512_ceil_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = ceil(p1[i]);
    }
    return result;
}

/* Round float64 vector to nearest upper integer under mask. */
inline _M512D _mm512_mask_ceil_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = ceil(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Round float32 vector to nearest upper integer. */
inline _M512 _mm512_ceil_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = ceilf(p1[i]);
    }
    return result;
}

/* Round float32 vector to nearest upper integer under mask. */
inline _M512 _mm512_mask_ceil_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = ceilf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Cosine of a float64 vector. */
inline _M512D _mm512_cos_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = cos(p1[i]);
    }
    return result;
}

/* Cosine of a float64 vector under mask. */
inline _M512D _mm512_mask_cos_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = cos(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Cosine of a float32 vector. */
inline _M512 _mm512_cos_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = cosf(p1[i]);
    }
    return result;
}

/* Cosine of a float32 vector under mask. */
inline _M512 _mm512_mask_cos_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = cosf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic cosine of a float64 vector. */
inline _M512D _mm512_cosh_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = cosh(p1[i]);
    }
    return result;
}

/* Hyperbolic cosine of a float64 vector under mask. */
inline _M512D _mm512_mask_cosh_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = cosh(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic cosine of a float32 vector. */
inline _M512 _mm512_cosh_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = coshf(p1[i]);
    }
    return result;
}

/* Hyperbolic cosine of a float32 vector under mask. */
inline _M512 _mm512_mask_cosh_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = coshf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Exponential base-2 of a float64 vector. */
inline _M512D _mm512_exp2_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = pow(2.0, p1[i]);
    }
    return result;
}

/* Exponential base-2 of a float64 vector under mask. */
inline _M512D _mm512_mask_exp2_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = pow(2.0, p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Exponential base-2 of a float32 vector. */
inline _M512 _mm512_exp2_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = powf(2.0f, p1[i]);
    }
    return result;
}

/* Exponential base-2 of a float32 vector under mask. */
inline _M512 _mm512_mask_exp2_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = powf(2.0f, p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Exponential base-e of a float64 vector. */
inline _M512D _mm512_exp_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = exp(p1[i]);
    }
    return result;
}

/* Exponential base-e of a float64 vector under mask. */
inline _M512D _mm512_mask_exp_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = exp(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Exponential base-e of a float32 vector. */
inline _M512 _mm512_exp_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = expf(p1[i]);
    }
    return result;
}

/* Exponential base-e of a float32 vector under mask. */
inline _M512 _mm512_mask_exp_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = expf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Round float64 vector to nearest lower integer. */
inline _M512D _mm512_floor_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = floor(p1[i]);
    }
    return result;
}

/* Round float64 vector to nearest lower integer under mask. */
inline _M512D _mm512_mask_floor_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = exp(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Round float32 vector to nearest lower integer. */
inline _M512 _mm512_floor_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = floorf(p1[i]);
    }
    return result;
}

/* Round float32 vector to nearest lower integer under mask. */
inline _M512 _mm512_mask_floor_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = floorf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hypotenuse of float64 vectors. */
inline _M512D _mm512_hypot_pd(_M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = _hypot(p1[i], p2[i]);
    }
    return result;
}

/* Hypotenuse of float64 vectors under mask. */
inline _M512D _mm512_mask_hypot_pd(_M512D v0_old, __mmask k1, _M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = _hypot(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hypotenuse of float32 vectors. */
inline _M512 _mm512_hypot_ps(_M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = _hypotf(p1[i], p2[i]);
    }
    return result;
}

/* Hypotenuse of float32 vectors under mask. */
inline _M512 _mm512_mask_hypot_ps(_M512 v0_old, __mmask k1, _M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = _hypotf(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Quotient of int32 vectors. */
inline _M512I _mm512_div_pi(_M512I v1, _M512I v2)
{
    _M512I result;
    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p2[i] != 0) r[i] = p1[i] / p2[i];
        else r[i] = 0;
    }
    return result;
}

/* Quotient of int32 vectors under mask. */
inline _M512I _mm512_mask_div_pi(_M512I v0_old, __mmask k1, _M512I v1, _M512I v2)
{
    _M512I result;
    int *r = (int *) &result;
    int *p0 = (int *) &v0_old;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p2[i] != 0) r[i] = p1[i] / p2[i];
            else r[i] = 0;
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Quotient of float32 vectors. */
inline _M512 _mm512_div_ps(_M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = p1[i] / p2[i];
    }
    return result;
}

/* Quotient of float32 vectors under mask. */
inline _M512 _mm512_mask_div_ps(_M512 v0_old, __mmask k1, _M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = p1[i] / p2[i];
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Quotient of float64 vectors. */
inline _M512D _mm512_div_pd(_M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = p1[i] / p2[i];
    }
    return result;
}

/* Quotient of float64 vectors under mask. */
inline _M512D _mm512_mask_div_pd(_M512D v0_old, __mmask k1, _M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = p1[i] / p2[i];
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Quotient of uint32 vectors. */
inline _M512I _mm512_div_pu(_M512I v1, _M512I v2)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p2[i] != 0) r[i] = p1[i] / p2[i];
        else r[i] = 0;
    }
    return result;
}

/* Quotient of uint32 vectors under mask. */
inline _M512I _mm512_mask_div_pu(_M512I v0_old, __mmask k1, _M512I v1, _M512I v2)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p0 = (unsigned int *) &v0_old;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p2[i] != 0) r[i] = p1[i] / p2[i];
            else r[i] = 0;
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Reciprocal square root of float64 vector. */
inline _M512D _mm512_rsqrt_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = 1.0 / sqrt(p1[i]);
    }
    return result;
}

/* Reciprocal square root of float64 vector under mask. */
inline _M512D _mm512_mask_rsqrt_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = 1.0 / sqrt(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Reciprocal square root of float32 vector. */
inline _M512 _mm512_rsqrt_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = 1.0f / sqrtf(p1[i]);
    }
    return result;
}

/* Reciprocal square root of float32 vector under mask. */
inline _M512 _mm512_mask_rsqrt_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = 1.0f / sqrtf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Reciprocal of float64 vector. */
inline _M512D _mm512_recip_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = 1.0 / p1[i];
    }
    return result;
}

/* Reciprocal of float64 vector under mask. */
inline _M512D _mm512_mask_recip_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = 1.0 / p1[i];
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Reciprocal of float32 vector. */
inline _M512 _mm512_recip_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = 1.0f / p1[i];
    }
    return result;
}

/* Reciprocal of float32 vector under mask. */
inline _M512 _mm512_mask_recip_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = 1.0f / p1[i];
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Remainder of the division of int32 vectors. */
inline _M512I _mm512_rem_pi(_M512I v1, _M512I v2)
{
    _M512I result;
    int *r = (int *) &result;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p2[i] != 0) r[i] = p1[i] % p2[i];
        else r[i] = 0;
    }
    return result;
}

/* Remainder of the division of int32 vectors under mask. */
inline _M512I _mm512_mask_rem_pi(_M512I v0_old, __mmask k1, _M512I v1, _M512I v2)
{
    _M512I result;
    int *r = (int *) &result;
    int *p0 = (int *) &v0_old;
    int *p1 = (int *) &v1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p2[i] != 0) r[i] = p1[i] % p2[i];
            else r[i] = 0;
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Remainder of the division of two uint32 vectors. */
inline _M512I _mm512_rem_pu(_M512I v1, _M512I v2)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (p2[i] != 0) r[i] = p1[i] % p2[i];
        else r[i] = 0;
    }
    return result;
}

/* Remainder of the division of two uint32 vectors under mask. */
inline _M512I _mm512_mask_rem_pu(_M512I v0_old, __mmask k1, _M512I v1, _M512I v2)
{
    _M512I result;
    unsigned int *r = (unsigned int *) &result;
    unsigned int *p0 = (unsigned int *) &v0_old;
    unsigned int *p1 = (unsigned int *) &v1;
    unsigned int *p2 = (unsigned int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            if (p2[i] != 0) r[i] = p1[i] % p2[i];
            else r[i] = 0;
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-10 of float64 vector. */
inline _M512D _mm512_log10_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = log10(p1[i]);
    }
    return result;
}

/* Logarithm base-10 of float64 vector under mask. */
inline _M512D _mm512_mask_log10_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = log10(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-10 of float32 vector. */
inline _M512 _mm512_log10_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = log10f(p1[i]);
    }
    return result;
}

/* Logarithm base-10 of float32 vector under mask. */
inline _M512 _mm512_mask_log10_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = log10f(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-2 of float64 vector. */
inline _M512D _mm512_log2_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = log(p1[i]) * 1.4426950408889634073599246810019;  // 1 / Ln(2)
    }
    return result;
}

/* Logarithm base-2 of float64 vector under mask. */
inline _M512D _mm512_mask_log2_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = log(p1[i]) * 1.4426950408889634073599246810019;  // 1 / Ln(2)
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-2 of float32 vector. */
inline _M512 _mm512_log2_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = logf(p1[i]) * 1.4426950408889634073599246810019f;     // 1 / Ln(2)
    }
    return result;
}

/* Logarithm base-2 of float32 vector under mask. */
inline _M512 _mm512_mask_log2_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = logf(p1[i]) * 1.4426950408889634073599246810019f;    // 1 / Ln(2)
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-e of float64 vector. */
inline _M512D _mm512_log_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = log(p1[i]);
    }
    return result;
}

/* Logarithm base-e of float64 vector under mask. */
inline _M512D _mm512_mask_log_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = log(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Logarithm base-e of float32 vector. */
inline _M512 _mm512_log_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = logf(p1[i]);
    }
    return result;
}

/* Logarithm base-e of float32 vector under mask. */
inline _M512 _mm512_mask_log_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = logf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Float64 vector raised to the power of another float64 vector. */
inline _M512D _mm512_pow_pd(_M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        r[i] = pow(p1[i], p2[i]);
    }
    return result;
}

/* Float64 vector raised to the power of another float64 vector under mask. */
inline _M512D _mm512_mask_pow_pd(_M512D v0_old, __mmask k1, _M512D v1, _M512D v2)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = pow(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Float32 vector raised to the power of another float32 vector. */
inline _M512 _mm512_pow_ps(_M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        r[i] = powf(p1[i], p2[i]);
    }
    return result;
}

/* Float32 vector raised to the power of another float32 vector under mask. */
inline _M512 _mm512_mask_pow_ps(_M512 v0_old, __mmask k1, _M512 v1, _M512 v2)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = powf(p1[i], p2[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Sine of a float64 vector. */
inline _M512D _mm512_sin_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = sin(p1[i]);
    }
    return result;
}

/* Sine of a float64 vector under mask. */
inline _M512D _mm512_mask_sin_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = sin(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Sine of a float32 vector. */
inline _M512 _mm512_sin_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = sinf(p1[i]);
    }
    return result;
}

/* Sine of a float32 vector under mask. */
inline _M512 _mm512_mask_sin_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = sinf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic sine of a float64 vector. */
inline _M512D _mm512_sinh_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = sinh(p1[i]);
    }
    return result;
}

/* Hyperbolic sine of a float64 vector under mask. */
inline _M512D _mm512_mask_sinh_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = sinh(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic sine of a float32 vector. */
inline _M512 _mm512_sinh_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = sinhf(p1[i]);
    }
    return result;
}

/* Hyperbolic sine of a float32 vector under mask. */
inline _M512 _mm512_mask_sinh_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = sinhf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Square root of float64 vector. */
inline _M512D _mm512_sqrt_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = sqrt(p1[i]);
    }
    return result;
}

/* Square root of float64 vector under mask. */
inline _M512D _mm512_mask_sqrt_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = sqrt(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Square root of float32 vector. */
inline _M512 _mm512_sqrt_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = sqrtf(p1[i]);
    }
    return result;
}

/* Square root of float32 vector under mask. */
inline _M512 _mm512_mask_sqrt_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = sqrtf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Tangent of float64 vector. */
inline _M512D _mm512_tan_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = tan(p1[i]);
    }
    return result;
}

/* Tangent of float64 vector under mask. */
inline _M512D _mm512_mask_tan_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = tan(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Tangent of float32 vector. */
inline _M512 _mm512_tan_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = tanf(p1[i]);
    }
    return result;
}

/* Tangent of float32 vector under mask. */
inline _M512 _mm512_mask_tan_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = tanf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic tangent of float64 vector. */
inline _M512D _mm512_tanh_pd(_M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        r[i] = tanh(p1[i]);
    }
    return result;
}

/* Hyperbolic tangent of float64 vector under mask. */
inline _M512D _mm512_mask_tanh_pd(_M512D v0_old, __mmask k1, _M512D v1)
{
    _M512D result;
    double *r = (double *) &result;
    double *p0 = (double *) &v0_old;
    double *p1 = (double *) &v1;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) {
            r[i] = tanh(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Hyperbolic tangent of float32 vector. */
inline _M512 _mm512_tanh_ps(_M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        r[i] = tanhf(p1[i]);
    }
    return result;
}

/* Hyperbolic tangent of float32 vector under mask. */
inline _M512 _mm512_mask_tanh_ps(_M512 v0_old, __mmask k1, _M512 v1)
{
    _M512 result;
    float *r = (float *) &result;
    float *p0 = (float *) &v0_old;
    float *p1 = (float *) &v1;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) {
            r[i] = tanhf(p1[i]);
        } else {
            r[i] = p0[i];
        }
        k1 >>= 1;
    }
    return result;
}

/* Sum of all elements of float32 vector. */
inline float _mm512_reduce_add_ps(_M512 v2)
{
    float result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p2 = (float *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result += p2[i];
    }
#else
    __m128 *p2 = (__m128 *) &v2;
    __m128 tmp;
    float *r = (float *) &tmp;
    
    tmp = _mm_add_ps(_mm_add_ps(p2[0], p2[1]), _mm_add_ps(p2[2], p2[3]));    
    result = r[0] + r[1] + r[2] + r[3];
#endif
    return result;
}

/* Sum of all elements of float32 vector under mask. */
/* Returns 0 if mask = 0. */
inline float _mm512_mask_reduce_add_ps(__mmask k1, _M512 v2)
{
    float result = 0;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result += p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Sum of all elements of float64 vector. */
inline double _mm512_reduce_add_pd(_M512D v2)
{
    double result;
    
#if USE_C_PROTOTYPE_PRIMITIVES
    double *p2 = (double *) &v2;

    result = p2[0];
    for (int i=1; i < 8; i++) {
        result += p2[i];
    }
#else
    __m128d *p2 = (__m128d *) &v2;
    __m128d tmp;
    double *r = (double *) &tmp;
    
    tmp = _mm_add_pd(_mm_add_pd(p2[0], p2[1]), _mm_add_pd(p2[2], p2[3]));    
    result = r[0] + r[1];
#endif
    return result;
}

/* Sum of all elements of float64 vector under mask. */
/* Returns 0 if mask = 0. */
inline double _mm512_mask_reduce_add_pd(__mmask k1, _M512D v2)
{
    double result = 0;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) result += p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Sum of all elements of int32 vector. */
inline int _mm512_reduce_add_pi(_M512I v2)
{
    int result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p2 = (int *) &v2;
    
    result = p2[0];
    for (int i=1; i < 16; i++) {
        result += p2[i];
    }
#else
    __m128i *p2 = (__m128i *) &v2;
    __m128i tmp;
    int *r = (int *) &tmp;
    
    tmp = _mm_add_epi32(_mm_add_epi32(p2[0], p2[1]), _mm_add_epi32(p2[2], p2[3]));    
    result = r[0] + r[1] + r[2] + r[3];
#endif
    return result;
}

/* Sum of all elements of int32 vector under mask. */
/* Returns 0 if mask = 0. */
inline int _mm512_mask_reduce_add_pi(__mmask k1, _M512I v2)
{
    int result = 0;
    int *p2 = (int *) &v2;
    
    for (int i=0; i < 16; i++) {
        if (k1 & 1) result += p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Product of all elements of float32 vector. */
inline float _mm512_reduce_mul_ps(_M512 v2)
{
    float result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p2 = (float *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result *= p2[i];
    }
#else
    __m128 *p2 = (__m128 *) &v2;
    __m128 tmp;
    float *r = (float *) &tmp;
    
    tmp = _mm_mul_ps(_mm_mul_ps(p2[0], p2[1]), _mm_mul_ps(p2[2], p2[3]));    
    result = r[0] * r[1] * r[2] * r[3];
#endif
    return result;
}

/* Product of all elements of float32 vector under mask. */
/* Returns 1 if mask = 0. */
inline float _mm512_mask_reduce_mul_ps(__mmask k1,_M512 v2)
{
    float result = 1.0f;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result *= p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Product of all elements of float64 vector. */
inline double _mm512_reduce_mul_pd(_M512D v2)
{
    double result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p2 = (double *) &v2;

    result = p2[0];
    for (int i=1; i < 8; i++) {
        result *= p2[i];
    }
#else
    __m128d *p2 = (__m128d *) &v2;
    __m128d tmp;
    double *r = (double *) &tmp;
    
    tmp = _mm_mul_pd(_mm_mul_pd(p2[0], p2[1]), _mm_mul_pd(p2[2], p2[3]));    
    result = r[0] * r[1];
#endif
    return result;
}

/* Product of all elements of float64 vector under mask. */
/* Returns 1 if mask = 0. */
inline double _mm512_mask_reduce_mul_pd(__mmask k1, _M512D v2)
{
    double result = 1.0;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) result *= p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Product of all elements of int32 vector. */
inline int _mm512_reduce_mul_pi(_M512I v2)
{
    int result;
    int *p2 = (int *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result *= p2[i];
    }
    return result;
}

/* Product of all elements of int32 vector under mask. */
/* Returns 1 if mask = 0. */
inline int _mm512_mask_reduce_mul_pi(__mmask k1, _M512I v2)
{
    int result = 1;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result *= p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Minimum of all elements of float32 vector. */
inline float _mm512_reduce_min_ps(_M512 v2)
{
    float result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p2 = (float *) &v2;
    
    result = p2[0];
    for (int i=1; i < 16; i++) {
        result = _LPP_min(result, p2[i]);
    }
#else
    __m128 *p2 = (__m128 *) &v2;
    __m128 tmp;
    float *r = (float *) &tmp;
    
    tmp = _mm_min_ps(_mm_min_ps(p2[0], p2[1]), _mm_min_ps(p2[2], p2[3]));    
    result = _LPP_min(_LPP_min(r[0], r[1]), _LPP_min(r[2], r[3]));
#endif
    return result;
}

/* Minimum of all elements of float32 vector under mask. */
/* Returns FLT_MAX if mask = 0. */
inline float _mm512_mask_reduce_min_ps(__mmask k1, _M512 v2)
{
    float result = FLT_MAX;
    float *p2 = (float *) &v2;
    
    for (int i=0; i < 16; i++) {
        if (k1 & 1) result = _LPP_min(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Minimum of all elements of float64 vector. */
inline double _mm512_reduce_min_pd(_M512D v2)
{
    double result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p2 = (double *) &v2;

    result = p2[0];
    for (int i=1; i < 8; i++) {
        result = _LPP_min(result, p2[i]);
    }
#else
    __m128d *p2 = (__m128d *) &v2;
    __m128d tmp;
    double *r = (double *) &tmp;
    
    tmp = _mm_min_pd(_mm_min_pd(p2[0], p2[1]), _mm_min_pd(p2[2], p2[3]));    
    result = _LPP_min(r[0], r[1]);
#endif
    return result;
}

/* Minimum of all elements of float64 vector under mask. */
/* Returns DBL_MAX if mask = 0. */
inline double _mm512_mask_reduce_min_pd(__mmask k1, _M512D v2)
{
    double result = DBL_MAX;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) result = _LPP_min(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Minimum of all elements of int32 vector. */
inline int _mm512_reduce_min_pi(_M512I v2)
{
    int result;
    int *p2 = (int *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result = _LPP_min(result, p2[i]);
    }
    return result;
}

/* Minimum of all elements of int32 vector under mask. */
/* Returns 0x7fffffff if mask = 0. */
inline int _mm512_mask_reduce_min_pi(__mmask k1, _M512I v2)
{
    int result = 0x7fffffff;    // MAX_INT
    int *p2 = (int *) &v2;

    for (int i=1; i < 16; i++) {
        if (k1 & 1) result = _LPP_min(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Maximum of all elements of float32 vector. */
inline float _mm512_reduce_max_ps(_M512 v2)
{
    float result;

#if USE_C_PROTOTYPE_PRIMITIVES
    float *p2 = (float *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result = _LPP_max(result, p2[i]);
    }
#else
    __m128 *p2 = (__m128 *) &v2;
    __m128 tmp;
    float *r = (float *) &tmp;
    
    tmp = _mm_max_ps(_mm_max_ps(p2[0], p2[1]), _mm_max_ps(p2[2], p2[3]));    
    result = _LPP_max(_LPP_max(r[0], r[1]), _LPP_max(r[2], r[3]));
#endif
    return result;
}

/* Maximum of all elements of float32 vector under mask. */
/* Returns FLT_MIN if mask = 0. */
inline float _mm512_mask_reduce_max_ps(__mmask k1, _M512 v2)
{
    float result = FLT_MIN;
    float *p2 = (float *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result = _LPP_max(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Maximum of all elements of float64 vector. */
inline double _mm512_reduce_max_pd(_M512D v2)
{
    double result;

#if USE_C_PROTOTYPE_PRIMITIVES
    double *p2 = (double *) &v2;

    result = p2[0];
    for (int i=1; i < 8; i++) {
        result = _LPP_max(result, p2[i]);
    }
#else
    __m128d *p2 = (__m128d *) &v2;
    __m128d tmp;
    double *r = (double *) &tmp;
    
    tmp = _mm_max_pd(_mm_max_pd(p2[0], p2[1]), _mm_max_pd(p2[2], p2[3]));    
    result = _LPP_max(r[0], r[1]);
#endif
    return result;
}

/* Maximum of all elements of float64 vector under mask. */
/* Returns DBL_MIN if mask = 0. */
inline double _mm512_mask_reduce_max_pd(__mmask k1, _M512D v2)
{
    double result = DBL_MIN;
    double *p2 = (double *) &v2;

    for (int i=0; i < 8; i++) {
        if (k1 & 1) result = _LPP_max(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Maximum of all elements of int32 vector. */
inline int _mm512_reduce_max_pi(_M512I v2)
{
    int result;
    int *p2 = (int *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result = _LPP_max(result, p2[i]);
    }
    return result;
}

/* Maximum of all elements of int32 vector under mask. */
/* Returns 0x80000000 if mask = 0. */
inline int _mm512_mask_reduce_max_pi(__mmask k1, _M512I v2)
{
    int result = 0x80000000;        // MIN_INT
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result = _LPP_max(result, p2[i]);
        k1 >>= 1;
    }
    return result;
}

/* Logical OR of all elements of int32 vector. */
inline int _mm512_reduce_or_pi(_M512I v2)
{
    int result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p2 = (int *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result |= p2[i];
    }
#else
    __m128i *p2 = (__m128i *) &v2;
    __m128i tmp;
    int *r = (int *) &tmp;
    
    tmp = _mm_or_si128(_mm_or_si128(p2[0], p2[1]), _mm_or_si128(p2[2], p2[3]));    
    result = r[0] | r[1] | r[2] | r[3];
#endif
    return result;
}

/* Logical OR of all elements of int32 vector under mask. */
/* Returns 0 if mask = 0. */
inline int _mm512_mask_reduce_or_pi(__mmask k1, _M512I v2)
{
    int result = 0;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result |= p2[i];
        k1 >>= 1;
    }
    return result;
}

/* Logical AND of all elements of int32 vector. */
inline int _mm512_reduce_and_pi(_M512I v2)
{
    int result;

#if USE_C_PROTOTYPE_PRIMITIVES
    int *p2 = (int *) &v2;

    result = p2[0];
    for (int i=1; i < 16; i++) {
        result &= p2[i];
    }
#else
    __m128i *p2 = (__m128i *) &v2;
    __m128i tmp;
    int *r = (int *) &tmp;
    
    tmp = _mm_and_si128(_mm_and_si128(p2[0], p2[1]), _mm_and_si128(p2[2], p2[3]));    
    result = r[0] & r[1] & r[2] & r[3];
#endif
    return result;
}

/* Logical AND of all elements of int32 vector under mask. */
/* Returns 0xffffffff if mask = 0. */
inline int _mm512_mask_reduce_and_pi(__mmask k1, _M512I v2)
{
    int result = 0xffffffff;
    int *p2 = (int *) &v2;

    for (int i=0; i < 16; i++) {
        if (k1 & 1) result &= p2[i];
        k1 >>= 1;
    }
    return result;
}


/****
 **** Scalar operations
 ****/

/* 1:1 bit interleave. */
inline unsigned short _mm_bitinterleave11_16(unsigned short r1, unsigned short r2)
{
    unsigned short result = 0;
    unsigned short bit = 1;

    for (int i=0; i < 8; i++) {
        result |= ((r2 & bit) << i);
        result |= ((r1 & bit) << (i + 1));
        bit <<= 1;
    }

    return result;
}

/* 1:1 bit interleave. */
inline unsigned int _mm_bitinterleave11_32(unsigned int r1, unsigned int r2)
{
    unsigned int result = 0;
    unsigned int bit = 1;

    for (int i=0; i < 16; i++) {
        result |= ((r2 & bit) << i);
        result |= ((r1 & bit) << (i + 1));
        bit <<= 1;
    }

    return result;
}

/* 1:1 bit interleave. */
inline uint64_t _mm_bitinterleave11_64(uint64_t r1, uint64_t r2)
{
    uint64_t result = 0;
    uint64_t bit = 1;

    for (int i=0; i < 32; i++) {
        result |= ((r2 & bit) << i);
        result |= ((r1 & bit) << (i + 1));
        bit <<= 1;
    }

    return result;
}

/* 2:1 bit interleave. */
inline unsigned short _mm_bitinterleave21_16(unsigned short r1, unsigned short r2)
{
    unsigned short result = 0;
    unsigned short bit = 1;
    unsigned short bits = 3;

    for (int i=0; i < 5; i++) {
        result |= ((r2 & bit) << (i*2));
        result |= ((r1 & bits) << (i + 1));
        bit <<= 1;
        bits <<= 2;
    }
    result |= ((r2 & 0x20) << 10);

    return result;
}

/* 2:1 bit interleave. */
inline unsigned int _mm_bitinterleave21_32(unsigned int r1, unsigned int r2)
{
    unsigned int result = 0;
    unsigned int bit = 1;
    unsigned int bits = 3;

    for (int i=0; i < 10; i++) {
        result |= ((r2 & bit) << (i*2));
        result |= ((r1 & bits) << (i + 1));
        bit <<= 1;
        bits <<= 2;
    }
    result |= ((r2 & 0x400) << 20);
    result |= ((r1 & 0x100000) << 11);

    return result;
}

/* 2:1 bit interleave. */
inline uint64_t _mm_bitinterleave21_64(uint64_t r1, uint64_t r2)
{
    uint64_t result = 0;
    uint64_t bit = 1;
    uint64_t bits = 3;

    for (int i=0; i < 21; i++) {
        result |= ((r2 & bit) << (i*2));
        result |= ((r1 & bits) << (i + 1));
        bit <<= 1;
        bits <<= 2;
    }
    result |= ((r2 & 0x20000) << 42);

    return result;
}

/* Fast bit scan forward. */
inline short _mm_bsff_16(unsigned short r1)
{
    short result = 0;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 1) return result;
        r1 >>= 1;
        result++;
    }
}

/* Fast bit scan forward. */
inline int _mm_bsff_32(unsigned int r1)
{
    int result = 0;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 1) return result;
        r1 >>= 1;
        result++;
    }
}

/* Fast bit scan forward. */
inline int64_t _mm_bsff_64(uint64_t r1)
{
    int64_t result = 0;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 1) return result;
        r1 >>= 1;
        result++;
    }
}

/* Bit scan forward initialized. */
inline short _mm_bsfi_16(short r1, unsigned short r2)
{
    if (r1 >= 15 || r2 == 0) return -1;
    if (r1 < 0) r1 = -1;

    r1++;
    r2 >>= r1;

    for (short result = r1; result < 16; result++) {
        if (r2 & 1) return result;
        r2 >>= 1;
    }

    return -1;
}

/* Bit scan forward initialized. */
inline int _mm_bsfi_32(int r1, unsigned int r2)
{
    if (r1 >= 31 || r2 == 0) return -1;
    if (r1 < 0) r1 = -1;

    r1++;
    r2 >>= r1;

    for (int result = r1; result < 32; result++) {
        if (r2 & 1) return result;
        r2 >>= 1;
    }

    return -1;
}

/* Bit scan forward initialized. */
inline int64_t _mm_bsfi_64(int64_t r1, uint64_t r2)
{
    if (r1 >= 63 || r2 == 0) return -1;
    if (r1 < 0) r1 = -1;

    r1++;
    r2 >>= r1;

    for (int64_t result = r1; result < 64; result++) {
        if (r2 & 1) return result;
        r2 >>= 1;
    }

    return -1;
}

/* Fast bit scan reverse. */
inline short _mm_bsrf_16(unsigned short r1)
{
    short result = 15;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 0x8000) return result;
        r1 <<= 1;
        result--;
    }
}

/* Fast bit scan reverse. */
inline int _mm_bsrf_32(unsigned int r1)
{
    int result = 31;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 0x80000000) return result;
        r1 <<= 1;
        result--;
    }
}

/* Fast bit scan reverse. */
inline int64_t _mm_bsrf_64(uint64_t r1)
{
    int result = 63;

    if (r1 == 0) return -1;

    for (;;) {
        if (r1 & 0x8000000000000000ULL) return result;
        r1 <<= 1;
        result--;
    }
}

/* Bit scan reverse initialized. */
inline short _mm_bsri_16(short r1, unsigned short r2)
{
    if (r1 <= 0 || r2 == 0) return -1;
    if (r1 > 16) r1 = 16;

    r1--;
    r2 <<= (15 - r1);

    for (short result = r1; result >= 0; result--) {
        if (r2 & 0x8000) return result;
        r2 <<= 1;
    }

    return -1;
}

/* Bit scan reverse initialized. */
inline int _mm_bsri_32(int r1, unsigned int r2)
{
    if (r1 <= 0 || r2 == 0) return -1;
    if (r1 > 32) r1 = 32;

    r1--;
    r2 <<= (31 - r1);

    for (int result = r1; result >= 0; result--) {
        if (r2 & 0x80000000) return result;
        r2 <<= 1;
    }

    return -1;
}

/* Bit scan reverse initialized. */
inline int64_t _mm_bsri_64(int64_t r1, uint64_t r2)
{
    if (r1 <= 0 || r2 == 0) return -1;
    if (r1 > 64) r1 = 64;

    r1--;
    r2 <<= (63 - r1);

    for (int64_t result = r1; result >= 0; result--) {
        if (r2 & 0x8000000000000000ULL) return result;
        r2 <<= 1;
    }

    return -1;
}

/* Bit population count. */
inline unsigned short _mm_countbits_16(unsigned short r1)
{
    unsigned short tmp1 = (r1 & 0x5555) + ((r1 >> 1) & 0x5555);
    unsigned short tmp2 = (tmp1 & 0x3333) + ((tmp1 >> 2) & 0x3333);
    unsigned short tmp3 = (tmp2 & 0x0F0F) + ((tmp2 >> 4) & 0x0F0F);

    return (tmp3 & 0x00FF) + ((tmp3 >> 8) & 0x00FF);
}

/* Bit population count. */
inline unsigned int _mm_countbits_32(unsigned int r1)
{
    unsigned int tmp1 = (r1 & 0x55555555) + ((r1 >> 1) & 0x55555555);
    unsigned int tmp2 = (tmp1 & 0x33333333) + ((tmp1 >> 2) & 0x33333333);
    unsigned int tmp3 = (tmp2 & 0x0F0F0F0F) + ((tmp2 >> 4) & 0x0F0F0F0F);
    unsigned int tmp4 = (tmp3 & 0x00FF00FF) + ((tmp3 >> 8) & 0x00FF00FF);

    return (tmp4 & 0x0000FFFF) + ((tmp4 >> 16) & 0x0000FFFF);
}

/* Bit population count. */
inline uint64_t _mm_countbits_64(uint64_t r1)
{
    uint64_t tmp1 = (r1 & 0x5555555555555555ULL) + ((r1 >> 1) & 0x5555555555555555ULL);
    uint64_t tmp2 = (tmp1 & 0x3333333333333333ULL) + ((tmp1 >> 2) & 0x3333333333333333ULL);
    uint64_t tmp3 = (tmp2 & 0x0F0F0F0F0F0F0F0FULL) + ((tmp2 >> 4) & 0x0F0F0F0F0F0F0F0FULL);
    uint64_t tmp4 = (tmp3 & 0x00FF00FF00FF00FFULL) + ((tmp3 >> 8) & 0x00FF00FF00FF00FFULL);
    uint64_t tmp5 = (tmp4 & 0x0000FFFF0000FFFFULL) + ((tmp4 >> 16) & 0x0000FFFF0000FFFFULL);

    return (tmp5 & 0x00000000FFFFFFFFULL) + ((tmp5 >> 32) & 0x00000000FFFFFFFFULL);
}

/* Rotate and bitfield-insert. */
inline unsigned short _mm_insertfield_16(unsigned short r1, unsigned short r2, _MM_BITPOSITION16_ENUM rotation, _MM_BITPOSITION16_ENUM bit_idx_low, _MM_BITPOSITION16_ENUM bit_idx_high)
{
    unsigned short maskRight = (unsigned short) (0xFFFF >> (15 - bit_idx_high));
    unsigned short maskLeft = (unsigned short) (0xFFFF << bit_idx_low);
    unsigned short mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    unsigned short rotated = (unsigned short) ((r2 << rotation) | (r2 >> (16 - rotation)));
    return (r1 & ~mask) | (rotated & mask);
}

/* Rotate and bitfield-insert. */
inline unsigned int _mm_insertfield_32(unsigned int r1, unsigned int r2, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    unsigned int rotated = (r2 << rotation) | (r2 >> (32 - rotation));
    return (r1 & ~mask) | (rotated & mask);
}

/* Rotate and bitfield-insert. */
inline uint64_t _mm_insertfield_64(uint64_t r1, uint64_t r2, _MM_BITPOSITION64_ENUM rotation, _MM_BITPOSITION64_ENUM bit_idx_low, _MM_BITPOSITION64_ENUM bit_idx_high)
{
    uint64_t maskRight = 0xFFFFFFFFFFFFFFFFULL >> (63 - bit_idx_high);
    uint64_t maskLeft = 0xFFFFFFFFFFFFFFFFULL << bit_idx_low;
    uint64_t mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    uint64_t rotated = (r2 << rotation) | (r2 >> (64 - rotation));
    return (r1 & ~mask) | (rotated & mask);
}

/* Set per-quad mask. */
inline unsigned short _mm_quadmask16_16(unsigned short r1)
{
    unsigned short result = 0;

    for (int i=0; i < 4; i++) {
        result |= (((r1 & 0x0F) ? 1 : 0) << i);
        r1 >>= 4;
    }
    return result;
}

/* Set per-quad mask. */
inline unsigned int _mm_quadmask16_32(unsigned int r1)
{
    unsigned int result = 0;

    for (int i=0; i < 8; i++) {
        result |= (((r1 & 0x0F) ? 1 : 0) << i);
        r1 >>= 4;
    }
    return result;
}

/* Set per-quad mask. */
inline uint64_t _mm_quadmask16_64(uint64_t r1)
{
    uint64_t result = 0;

    for (int i=0; i < 16; i++) {
        result |= (((r1 & 0x0F) ? 1 : 0) << i);
        r1 >>= 4;
    }
    return result;
}

/* Rotate and mask. */
inline unsigned short _mm_rotatefield_16(unsigned short r2, _MM_BITPOSITION16_ENUM rotation, _MM_BITPOSITION16_ENUM bit_idx_low, _MM_BITPOSITION16_ENUM bit_idx_high)
{
    unsigned short maskRight = (unsigned short) (0xFFFF >> (15 - bit_idx_high));
    unsigned short maskLeft = (unsigned short) (0xFFFF << bit_idx_low);
    unsigned short mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    unsigned short rotated = (unsigned short) ((r2 << rotation) | (r2 >> (16 - rotation)));

    return rotated & mask;
}

/* Rotate and mask. */
inline unsigned int _mm_rotatefield_32(unsigned int r2, _MM_BITPOSITION32_ENUM rotation, _MM_BITPOSITION32_ENUM bit_idx_low, _MM_BITPOSITION32_ENUM bit_idx_high)
{
    unsigned int maskRight = 0xFFFFFFFF >> (31 - bit_idx_high);
    unsigned int maskLeft = 0xFFFFFFFF << bit_idx_low;
    unsigned int mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    unsigned int rotated = (r2 << rotation) | (r2 >> (32 - rotation));

    return rotated & mask;
}

/* Rotate and mask. */
inline uint64_t _mm_rotatefield_64(uint64_t r2, _MM_BITPOSITION64_ENUM rotation, _MM_BITPOSITION64_ENUM bit_idx_low, _MM_BITPOSITION64_ENUM bit_idx_high)
{
    uint64_t maskRight = 0xFFFFFFFFFFFFFFFFULL >> (63 - bit_idx_high);
    uint64_t maskLeft = 0xFFFFFFFFFFFFFFFFULL << bit_idx_low;
    uint64_t mask;

    if (bit_idx_high >= bit_idx_low) {
        mask = maskLeft & maskRight;
    } else {
        mask = maskLeft | maskRight;
    }

    uint64_t rotated = (r2 << rotation) | (r2 >> (64 - rotation));

    return rotated & mask;
}

/* Prefetch an L1 cache line. */
inline void _mm_vprefetch1(void *, _MM_PREFETCH_HINT_ENUM)
{
    // No operation.
}

/* Prefetch an L2 cache line. */
inline void _mm_vprefetch2(void *, _MM_PREFETCH_HINT_ENUM)
{
    // No operation.
}

/****
 **** Vector mask operations
 ****/

inline __mmask _mm512_vkand(__mmask k1, __mmask k2)
{
    return k1 & k2;
}

inline __mmask _mm512_vkandn(__mmask k1, __mmask k2)
{
    return (~k1) & k2;
}

inline __mmask _mm512_vkandnr(__mmask k1, __mmask k2)
{
    return (~k2) & k1;
}

inline __mmask _mm512_vkmov(__mmask k1)
{
    return k1;
}

inline __mmask _mm512_vkmovlhb(__mmask k1, __mmask k2)
{
    return ((k2 & 0xff) << 8) | (k1 & 0xff);
}

inline __mmask _mm512_vknot(__mmask k1)
{
    return ~k1;
}

inline __mmask _mm512_vkor(__mmask k1, __mmask k2)
{
    return k1 | k2;
}

inline bool _mm512_vkortestz(__mmask k1, __mmask k2)
{
    return ((k1 | k2) == 0);
}

inline bool _mm512_vkortestc(__mmask k1, __mmask k2)
{
    return ((k1 | k2) == 0xffff);
}

inline __mmask _mm512_vkxnor(__mmask k1, __mmask k2)
{
    return ~(k1 ^ k2);
}
    
inline __mmask _mm512_vkxor(__mmask k1, __mmask k2)
{
    return k1 ^ k2;
}

inline __mmask _mm512_vkswapb(__mmask k1, __mmask k2)
{
    return ((k2 & 0xff) << 8) | ((k1 & 0xff00) >> 8);
}
    
inline int _mm512_mask2int(__mmask k1)
{
    return (int) k1;
}

inline __mmask _mm512_int2mask(int k1){
    return (__mmask) k1;
}


#endif // _LRB_PROTOTYPE_PRIMITIVES_INL
