#pragma once

//
// MACROS
//

#ifndef SQR
#define SQR(A)          ((A) * (A))
#endif
#ifndef MIN
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#endif
#ifndef ABS
#define ABS(A)          ((A) > 0 ? (A) : -(A))
#endif

// value compare with error
#define MATH_NEAR(v1, v2, error) ((((v1)-(error))<=(v2)) && (((v1)+(error))>=(v2)))

// simple sign
#define MATH_SIGN(v1) (v1 < 0 ? -1 : 1)