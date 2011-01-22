//----------------------------------------------------------------------------------------------------------------------
// WindowingWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_WINDOWING_WIN_WINDOWING_WIN_H
#define LUNAR_WINDOWING_WIN_WINDOWING_WIN_H

#include "Platform/Platform.h"  // Always make sure Platform.h gets included first.

#if HELIUM_SHARED
    #ifdef LUNAR_WINDOWING_WIN_EXPORTS
        #define LUNAR_WINDOWING_WIN_API HELIUM_API_EXPORT
    #else
        #define LUNAR_WINDOWING_WIN_API HELIUM_API_IMPORT
    #endif
#else
    #define LUNAR_WINDOWING_WIN_API
#endif

#endif  // LUNAR_WINDOWING_WIN_WINDOWING_WIN_H
