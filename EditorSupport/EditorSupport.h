//----------------------------------------------------------------------------------------------------------------------
// EditorSupport.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_EDITOR_SUPPORT_EDITOR_SUPPORT_H
#define LUNAR_EDITOR_SUPPORT_EDITOR_SUPPORT_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_EDITOR_SUPPORT_EXPORTS
        #define LUNAR_EDITOR_SUPPORT_API HELIUM_API_EXPORT
    #else
        #define LUNAR_EDITOR_SUPPORT_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_EDITOR_SUPPORT_API
#endif

#endif  // LUNAR_EDITOR_SUPPORT_EDITOR_SUPPORT_H
