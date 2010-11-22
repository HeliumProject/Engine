//----------------------------------------------------------------------------------------------------------------------
// Graphics.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_GRAPHICS_H
#define LUNAR_GRAPHICS_GRAPHICS_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_GRAPHICS_EXPORTS
        #define LUNAR_GRAPHICS_API HELIUM_API_EXPORT
    #else
        #define LUNAR_GRAPHICS_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_GRAPHICS_API
#endif

#endif  // LUNAR_GRAPHICS_GRAPHICS_H
