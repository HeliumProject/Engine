//----------------------------------------------------------------------------------------------------------------------
// DynamicLibrary.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_DYNAMIC_LIBRARY_H
#define LUNAR_CORE_DYNAMIC_LIBRARY_H

#include "Core/Core.h"

namespace Lunar
{
    /// Dynamic library support functions.
    class LUNAR_CORE_API DynamicLibrary : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        ~DynamicLibrary();
        //@}

        /// @name Library Support
        //@{
        inline void* GetFunctionAddress( const CharString& rFunctionName ) const;
        void* GetFunctionAddress( const char* pFunctionName ) const;
        //@}

        /// @name Library Loading Functions
        //@{
        inline static DynamicLibrary* Load( const String& rPath );
        static DynamicLibrary* Load( const tchar_t* pPath );
        //@}

    private:
        /// Platform-specific library handle.
        void* m_pHandle;

        /// @name Construction/Destruction, Private
        //@{
        DynamicLibrary( void* pHandle );
        //@}
    };
}

#include "Core/DynamicLibrary.inl"

#endif  // LUNAR_CORE_DYNAMIC_LIBRARY_H
