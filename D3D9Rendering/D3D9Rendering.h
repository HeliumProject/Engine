//----------------------------------------------------------------------------------------------------------------------
// D3D9Rendering.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_D3D9_RENDERING_D3D9_RENDERING_H
#define LUNAR_D3D9_RENDERING_D3D9_RENDERING_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_D3D9_RENDERING_EXPORTS
        #define LUNAR_D3D9_RENDERING_API HELIUM_API_EXPORT
    #else
        #define LUNAR_D3D9_RENDERING_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_D3D9_RENDERING_API
#endif

#endif  // LUNAR_D3D9_RENDERING_D3D9_RENDERING_H
