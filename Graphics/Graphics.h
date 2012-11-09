//----------------------------------------------------------------------------------------------------------------------
// Graphics.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_GRAPHICS_H
#define HELIUM_GRAPHICS_GRAPHICS_H

#include "Platform/System.h"

#if HELIUM_SHARED
    #ifdef HELIUM_GRAPHICS_EXPORTS
        #define HELIUM_GRAPHICS_API HELIUM_API_EXPORT
    #else
        #define HELIUM_GRAPHICS_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_GRAPHICS_API
#endif

#endif  // HELIUM_GRAPHICS_GRAPHICS_H
