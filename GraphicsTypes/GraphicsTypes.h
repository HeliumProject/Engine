//----------------------------------------------------------------------------------------------------------------------
// GraphicsTypes.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_TYPES_GRAPHICS_TYPES_H
#define LUNAR_GRAPHICS_TYPES_GRAPHICS_TYPES_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_GRAPHICS_TYPES_EXPORTS
        #define LUNAR_GRAPHICS_TYPES_API HELIUM_API_EXPORT
    #else
        #define LUNAR_GRAPHICS_TYPES_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_GRAPHICS_TYPES_API
#endif

/// Non-zero to enable Granny integration for animations, zero to disable.
#define L_USE_GRANNY_ANIMATION ( L_HAVE_GRANNY )

#endif  // LUNAR_GRAPHICS_TYPES_GRAPHICS_TYPES_H
