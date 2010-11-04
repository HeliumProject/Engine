//----------------------------------------------------------------------------------------------------------------------
// Core.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_CORE_H
#define LUNAR_CORE_CORE_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_CORE_EXPORTS
        #define LUNAR_CORE_API HELIUM_API_EXPORT
    #else
        #define LUNAR_CORE_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_CORE_API
#endif

#endif  // LUNAR_CORE_CORE_H
