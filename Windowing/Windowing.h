//----------------------------------------------------------------------------------------------------------------------
// Windowing.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_WINDOWING_WINDOWING_H
#define HELIUM_WINDOWING_WINDOWING_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_WINDOWING_EXPORTS
        #define HELIUM_WINDOWING_API HELIUM_API_EXPORT
    #else
        #define HELIUM_WINDOWING_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_WINDOWING_API
#endif

#endif  // HELIUM_WINDOWING_WINDOWING_H
