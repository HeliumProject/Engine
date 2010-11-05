//----------------------------------------------------------------------------------------------------------------------
// PlatformOsWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLATFORM_OS_WIN_H
#define LUNAR_CORE_PLATFORM_OS_WIN_H

// Declare macros to ensure Windows XP compatibility level before including windows.h.
#ifndef WINVER
#define WINVER _WIN32_WINNT_WINXP
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif  // LUNAR_CORE_PLATFORM_OS_WIN_H
