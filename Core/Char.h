//----------------------------------------------------------------------------------------------------------------------
// Char.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_CHAR_H
#define LUNAR_CORE_CHAR_H

#include "Core/Core.h"

#include "boost/preprocessor/wstringize.hpp"
#include "boost/preprocessor/stringize.hpp"

#ifndef L_UNICODE
/// Non-zero to use Unicode strings, zero to use 8-bit character strings.
#define L_UNICODE 1
#endif

namespace Lunar
{
#if L_UNICODE

    /// Default character type.
    typedef wchar_t tchar_t;

    /// Prefix for declaring string and character literals of the default character type.
    #define L_T( X ) L##X

    /// Convert the expanded result of a macro to a tchar_t string.
    ///
    /// @param[in] X  Macro to expand and stringize.
    #define L_TSTRINGIZE( X ) BOOST_PP_WSTRINGIZE( X )

#else  // L_UNICODE

    /// Default character type.
    typedef char tchar_t;

    /// Prefix for declaring string and character literals of the default character type.
    #define L_T( X ) X

    /// Convert the expanded result of a macro to a tchar_t string.
    ///
    /// @param[in] X  Macro to expand and stringize.
    #define L_TSTRINGIZE( X ) BOOST_PP_STRINGIZE( X )

#endif  // L_UNICODE
}

#endif  // LUNAR_CORE_CHAR_H
