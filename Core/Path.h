//----------------------------------------------------------------------------------------------------------------------
// Path.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PATH_H
#define LUNAR_CORE_PATH_H

#include "Core/Core.h"
#include "Core/String.h"

namespace Lunar
{
    /// Platform path name string utilities.
    class LUNAR_CORE_API Path
    {
    public:
        /// @name Static Initialization
        //@{
        static void Shutdown();
        //@}

        /// @name Path String Parsing
        //@{
        static void GetDirectoryName( String& rOut, const String& rPath );
        static void GetBaseName( String& rOut, const String& rPath, bool bStripExtension = false );
        static void GetExtension( String& rOut, const String& rPath );
        //@}

        /// @name Path String Manipulation
        //@{
        static void Combine( String& rOut, const String& rPath0, const String& rPath1 );
        //@}

        /// @name Platform Path Information
        //@{
        static bool IsRootDirectory( const tchar_t* pPath );
        static bool IsRootDirectory( const String& rPath );

        inline static const String& GetInvalidPathCharacters();
        inline static const String& GetInvalidFileNameCharacters();
        //@}

    private:
        /// Invalid path name characters.
        static String sm_invalidPathCharacters;
        /// Invalid file name characters.
        static String sm_invalidFileNameCharacters;
    };
}

#if HELIUM_OS_WIN
#include "Core/PathWin.h"
#endif

#include "Core/Path.inl"

#endif  // LUNAR_CORE_PATH_H
