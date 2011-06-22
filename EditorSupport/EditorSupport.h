//----------------------------------------------------------------------------------------------------------------------
// EditorSupport.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_EDITOR_SUPPORT_H
#define HELIUM_EDITOR_SUPPORT_EDITOR_SUPPORT_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef HELIUM_EDITOR_SUPPORT_EXPORTS
        #define HELIUM_EDITOR_SUPPORT_API HELIUM_API_EXPORT
    #else
        #define HELIUM_EDITOR_SUPPORT_API HELIUM_API_IMPORT
    #endif
#else
    #define HELIUM_EDITOR_SUPPORT_API
#endif

#endif  // HELIUM_EDITOR_SUPPORT_EDITOR_SUPPORT_H
