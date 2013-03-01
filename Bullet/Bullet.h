
#pragma once
#ifndef HELIUM_BULLET_BULLET_H
#define HELIUM_BULLET_BULLET_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_BULLET_EXPORTS
        #define HELIUM_BULLET_API HELIUM_API_EXPORT
    #else
        #define HELIUM_BULLET_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_BULLET_API
#endif

#endif  // HELIUM_BULLET_BULLET_H
