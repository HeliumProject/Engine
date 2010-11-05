//----------------------------------------------------------------------------------------------------------------------
// Debug.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_DEBUG_H
#define LUNAR_CORE_DEBUG_H

#include "Core/Core.h"

#if !L_RELEASE

namespace Lunar
{
    /// @name Debug Utility Functions
    //@{

    LUNAR_CORE_API size_t GetStackTrace( void** ppStackTraceArray, size_t stackTraceArraySize, size_t skipCount = 1 );
    LUNAR_CORE_API void GetAddressSymbol( String& rSymbol, void* pAddress );

    //@}
}

#endif  // !L_RELEASE

#endif  // LUNAR_CORE_DEBUG_H
