//----------------------------------------------------------------------------------------------------------------------
// Rendering.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_RENDERING_H
#define HELIUM_RENDERING_RENDERING_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_RENDERING_EXPORTS
        #define HELIUM_RENDERING_API HELIUM_API_EXPORT
    #else
        #define HELIUM_RENDERING_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_RENDERING_API
#endif

#endif  // HELIUM_RENDERING_RENDERING_H
