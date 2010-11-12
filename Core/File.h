//----------------------------------------------------------------------------------------------------------------------
// File.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_FILE_H
#define LUNAR_CORE_FILE_H

#include "Core/Core.h"

namespace Helium
{
    class FileStream;
}

namespace Lunar
{
    class DirectoryIterator;

    /// Low-level file utilities
    class LUNAR_CORE_API File
    {
    public:
        /// File type identifiers.
        enum EType
        {
            TYPE_FIRST   =  0,
            TYPE_INVALID = -1,

            /// File exists, but is of a type that does not fall under one of the supported identifiers.
            TYPE_OTHER,

            /// Regular file.
            TYPE_FILE,
            /// Directory.
            TYPE_DIRECTORY,

            TYPE_MAX,
            TYPE_LAST = TYPE_MAX - 1
        };

        /// Directory creation results.
        enum EDirectoryCreateResult
        {
            DIRECTORY_CREATE_RESULT_FIRST   =  0,
            DIRECTORY_CREATE_RESULT_INVALID = -1,

            /// Directory creation succeeded.
            DIRECTORY_CREATE_RESULT_SUCCESS,
            /// Directory creation failed.
            DIRECTORY_CREATE_RESULT_FAILED,
            /// Directory already exists.
            DIRECTORY_CREATE_RESULT_ALREADY_EXISTS,

            DIRECTORY_CREATE_RESULT_MAX,
            DIRECTORY_CREATE_RESULT_LAST = DIRECTORY_CREATE_RESULT_MAX - 1
        };

        /// @name Static Initialization
        //@{
        static void Shutdown();
        //@}

        /// @name File Access
        //@{
        static FileStream* CreateStream();

        static FileStream* Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate = true );
        static FileStream* Open( const String& rPath, uint32_t modeFlags, bool bTruncate = true );

        static bool Exists( const tchar_t* pPath );
        static bool Exists( const String& rPath );

        static EType GetFileType( const tchar_t* pPath );
        static EType GetFileType( const String& rPath );

        static int64_t GetSize( const tchar_t* pPath );
        static int64_t GetSize( const String& rPath );

        static int64_t GetTimestamp( const tchar_t* pPath );
        static int64_t GetTimestamp( const String& rPath );

        static EDirectoryCreateResult CreateDirectory( const tchar_t* pPath, bool bRecursive = false );
        static EDirectoryCreateResult CreateDirectory( const String& rPath, bool bRecursive = false );

        static DirectoryIterator* IterateDirectory( const tchar_t* pPath );
        static DirectoryIterator* IterateDirectory( const String& rPath );
        //@}

        /// @name Filesystem Information
        //@{
        static const String& GetBaseDirectory();
        static const String& GetDataDirectory();
        static const String& GetUserDataDirectory();
        //@}

    private:
        /// @name Platform-specific Initialization
        //@{
        static void PlatformShutdown();
        //@}

        /// @name Directory Creation Implementation
        //@{
        static EDirectoryCreateResult CreateDirectoryRecursive( tchar_t* pPath, size_t pathLength );
        static EDirectoryCreateResult PlatformCreateDirectory( const tchar_t* pPath );
        //@}
    };
}

#endif  // LUNAR_CORE_FILE_H
