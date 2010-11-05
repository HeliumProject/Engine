//----------------------------------------------------------------------------------------------------------------------
// DirectoryIteratorWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_DIRECTORY_ITERATOR_WIN_H
#define LUNAR_CORE_DIRECTORY_ITERATOR_WIN_H

#include "Core/Core.h"

#if L_OS_WIN

#include "Core/DirectoryIterator.h"

namespace Lunar
{
    /// Windows DirectoryIterator implementation.
    class DirectoryIteratorWin : public DirectoryIterator
    {
    public:
        /// @name Construction/Destruction
        //@{
        DirectoryIteratorWin( const tchar_t* pDirectoryPath );
        virtual ~DirectoryIteratorWin();
        //@}

        /// @name File Information
        //@{
        virtual bool GetFileName( String& rFileName ) const;
        //@}

        /// @name File Iteration
        //@{
        virtual bool IsValid() const;
        virtual bool Advance();
        //@}

    private:
        /// Found file data.
        WIN32_FIND_DATA m_findFileData;
        /// File search handle.
        HANDLE m_hFindFile;
    };
}

#endif  // L_OS_WIN

#endif  // LUNAR_CORE_DIRECTORY_ITERATOR_WIN_H
