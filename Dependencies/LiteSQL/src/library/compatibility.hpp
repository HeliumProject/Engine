#ifndef compatibility_hpp
#define compatibility_hpp
#include "config.h"
#include "litesql_char.hpp"

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif // #ifdef HAVE_MEMORY_H

#ifndef HAVE_STRTOLL
long long int strtoll(const LITESQL_Char *nptr, LITESQL_Char **endptr, int base);
#endif
#ifndef HAVE_STRTOF
float strtof (const LITESQL_Char *nptr, LITESQL_Char **endptr);
#endif
#ifndef HAVE_LOCALTIME_R
#ifdef WIN32
#include <time.h>
#endif
struct tm *localtime_r(const time_t *clock, struct tm *result);
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif

#endif