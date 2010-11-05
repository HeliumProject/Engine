//----------------------------------------------------------------------------------------------------------------------
// PathWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PATH_WIN_H
#define LUNAR_CORE_PATH_WIN_H

#include "Core/Core.h"

#if L_OS_WIN

/// Primary directory separator character for the current platform.
#define L_PATH_SEPARATOR_CHAR L_T( '\\' )
/// Alternate directory separator character for the current platform.
#define L_ALT_PATH_SEPARATOR_CHAR L_T( '/' )

/// String containing the primary directory separator character for the current platform.
#define L_PATH_SEPARATOR_CHAR_STRING L_T( "\\" )
/// String containing the alternate directory separator character for the current platform.
#define L_ALT_PATH_SEPARATOR_CHAR_STRING L_T( "/" )

/// Null-terminated string containing the valid directory separators for the current platform (first character is the
/// primary separator, while the second character, if any, is the alternate separator).
#define L_PATH_SEPARATOR_LIST L_T( "\\/" )

#endif  // L_OS_WIN

#endif  // LUNAR_CORE_PATH_WIN_H
