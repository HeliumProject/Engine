//----------------------------------------------------------------------------------------------------------------------
// ThreadingWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_THREADING_WIN_H
#define LUNAR_CORE_THREADING_WIN_H

#include "Core/Core.h"
#include "Core/String.h"

namespace Lunar
{
    /// Thread-local storage management.
    class LUNAR_CORE_API ThreadLocalStorage
    {
    public:
        /// @name Storage Access
        //@{
        inline size_t Allocate();
        inline void Free( size_t index );

        inline uintptr_t Get( size_t index );
        inline void Set( size_t index, uintptr_t value );
        //@}

        /// @name Instance Access.
        //@{
        static ThreadLocalStorage& GetInstance();
        //@}

    private:
        /// @name Construction/Destruction
        //@{
        inline ThreadLocalStorage();
        inline ThreadLocalStorage( const ThreadLocalStorage& );  // Not implemented.
        inline ~ThreadLocalStorage();
        //@}

        /// @name Overloaded Operators
        //@{
        inline ThreadLocalStorage& operator=( const ThreadLocalStorage& );  // Not implemented.
        //@}
    };
}

#endif  // LUNAR_CORE_THREADING_WIN_H
