
#pragma once
#ifndef HELIUM_OIS_OIS_H
#define HELIUM_OIS_OIS_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_OIS_EXPORTS
        #define HELIUM_OIS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_OIS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_OIS_API
#endif

#endif  // HELIUM_OIS_OIS_H
