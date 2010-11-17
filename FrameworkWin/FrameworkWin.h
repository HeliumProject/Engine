//----------------------------------------------------------------------------------------------------------------------
// FrameworkWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WIN_FRAMEWORK_WIN_H
#define LUNAR_FRAMEWORK_WIN_FRAMEWORK_WIN_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_FRAMEWORK_WIN_EXPORTS
        #define LUNAR_FRAMEWORK_WIN_API HELIUM_API_EXPORT
    #else
        #define LUNAR_FRAMEWORK_WIN_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_FRAMEWORK_WIN_API
#endif

#endif  // LUNAR_FRAMEWORK_WIN_FRAMEWORK_WIN_H
