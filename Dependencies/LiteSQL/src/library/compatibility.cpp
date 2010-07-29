#include "compatibility.hpp"
#include "litesql_char.hpp"
#include <stdlib.h>
#include <time.h>
#ifndef HAVE_STRTOLL
long long int strtoll(const LITESQL_Char *nptr, LITESQL_Char **endptr, int base) {
    return _tcstol(nptr, endptr, base);
}
#endif
#ifndef HAVE_STRTOF
float strtof (const LITESQL_Char *nptr, LITESQL_Char **endptr) {
#ifdef HAVE_ATOF
    return (float) _tstof(nptr);
#endif
}
#endif
#ifndef HAVE_LOCALTIME_R
// FIXME: this is not really thread safe, it's just a hack to compile with MSC
// use something like mutex (W32) / pthread_mutex (LINUX)
struct tm *localtime_r(const time_t *clock, struct tm *result) {
    struct tm *tmp;

    tmp = localtime(clock);
    if (tmp)
    {
        *result = *tmp;
        tmp = result;
    }
    return tmp;
}
#endif
                
