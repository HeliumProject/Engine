//----------------------------------------------------------------------------------------------------------------------
// DirectoryIterator.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_DIRECTORY_ITERATOR_H
#define LUNAR_CORE_DIRECTORY_ITERATOR_H

#include "Core/String.h"

namespace Lunar
{
    /// Interface for iterating through the files in a directory.
    class LUNAR_CORE_API DirectoryIterator : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~DirectoryIterator() = 0;
        //@}

        /// @name File Information
        //@{
        virtual bool GetFileName( String& rFileName ) const = 0;
        inline String GetFileName() const;
        //@}

        /// @name File Iteration
        //@{
        virtual bool IsValid() const = 0;
        virtual bool Advance() = 0;
        //@}

        /// @name Overloaded Operators
        //@{
        inline operator bool() const;
        inline DirectoryIterator& operator++();
        //@}
    };
}

#include "Core/DirectoryIterator.inl"

#endif  // LUNAR_CORE_DIRECTORY_ITERATOR_H
