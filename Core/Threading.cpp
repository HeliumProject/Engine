//----------------------------------------------------------------------------------------------------------------------
// Threading.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/Threading.h"

namespace Lunar
{
    /// Destructor.
    Runnable::~Runnable()
    {
    }

    /// Get the global instance of thread-local storage management.
    ///
    /// @note  This must be called on the main application thread first.
    ///
    /// @return  Thread-local storage management instance.
    ThreadLocalStorage& ThreadLocalStorage::GetInstance()
    {
        static ThreadLocalStorage threadLocalStorage;
        return threadLocalStorage;
    }
}
