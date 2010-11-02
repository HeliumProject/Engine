//----------------------------------------------------------------------------------------------------------------------
// FileWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if L_OS_WIN

#include "Core/File.h"

#include "Core/Path.h"
#include "Core/FileStreamWin.h"
#include "Core/DirectoryIteratorWin.h"

#include <ShlObj.h>

#if L_UNICODE
#define _GET_FILE_ATTRIBUTES_EX GetFileAttributesExW
#else
#define _GET_FILE_ATTRIBUTES_EX GetFileAttributesExA
#endif

// Default user data directory (if the application name is not set).
#define L_DEFAULT_USER_DATA_DIRECTORY L_T( "Lunar" )

namespace Lunar
{
    /// Get the full path to the base directory of the application.
    ///
    /// @return  Application base directory path, with a trailing path separator character.
    static String& GetMutableBaseDirectory()
    {
        static String baseDirectory;
        static bool bLocateRequested = false;

        if( !bLocateRequested )
        {
            bLocateRequested = true;

            tchar_t pathBuffer[ MAX_PATH ];
            DWORD result = GetModuleFileName( NULL, pathBuffer, L_ARRAY_COUNT( pathBuffer ) );
            L_ASSERT( result < L_ARRAY_COUNT( pathBuffer ) );
            L_UNREF( result );

            pathBuffer[ L_ARRAY_COUNT( pathBuffer ) - 1 ] = L_T( '\0' );

            baseDirectory = pathBuffer;
            if( baseDirectory.IsEmpty() )
            {
                return baseDirectory;
            }

            // Strip the executable file.
            Path::GetDirectoryName( baseDirectory, baseDirectory );
            if( baseDirectory.IsEmpty() )
            {
                return baseDirectory;
            }

            // Strip the configuration type subdirectory (i.e. Debug, Intermediate, Release, etc.).
            Path::GetDirectoryName( baseDirectory, baseDirectory );
            if( baseDirectory.IsEmpty() )
            {
                return baseDirectory;
            }

            // Strip the platform binary subdirectory (i.e. x32, x64).
            Path::GetDirectoryName( baseDirectory, baseDirectory );
            if( baseDirectory.IsEmpty() )
            {
                return baseDirectory;
            }

            // Strip the "Bin" directory.
            Path::GetDirectoryName( baseDirectory, baseDirectory );
            size_t baseDirectoryLength = baseDirectory.GetSize();
            if( baseDirectoryLength == 0 )
            {
                return baseDirectory;
            }

            // Append a trailing path separator character if one is missing.
            if( baseDirectory[ baseDirectoryLength - 1 ] != L_PATH_SEPARATOR_CHAR &&
                baseDirectory[ baseDirectoryLength - 1 ] != L_ALT_PATH_SEPARATOR_CHAR )
            {
                baseDirectory += L_PATH_SEPARATOR_CHAR;
            }

            if( !File::Exists( baseDirectory ) )
            {
                baseDirectory.Clear();

                return baseDirectory;
            }

            baseDirectory.Trim();
        }

        return baseDirectory;
    }

    /// Get the full path to the base directory in which data files are stored.
    ///
    /// @return  Data directory path, with a trailing path separator character.
    static String& GetMutableDataDirectory()
    {
        static String dataDirectory;
        static bool bLocateRequested = false;

        if( !bLocateRequested )
        {
            bLocateRequested = true;

            // Get the application base directory.
            dataDirectory = GetMutableBaseDirectory();
            if( dataDirectory.IsEmpty() )
            {
                return dataDirectory;
            }

            // Append the "Data" directory, with trailing path separator.
            Path::Combine( dataDirectory, dataDirectory, String( L_T( "Data\\" ) ) );
            if( !File::Exists( dataDirectory ) )
            {
                dataDirectory.Clear();

                return dataDirectory;
            }

            dataDirectory.Trim();
        }

        return dataDirectory;
    }

    /// Get the full path to the base directory in which user data is stored.
    ///
    /// @return  User data directory path, with a trailing path separator character.
    static String& GetMutableUserDataDirectory()
    {
        static String userDataDirectory;
        static bool bLocateRequested = false;

        if( !bLocateRequested )
        {
            bLocateRequested = true;

            tchar_t pathBuffer[ MAX_PATH ];
            BOOL bLocateResult = SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_LOCAL_APPDATA, FALSE );
            if( !bLocateResult )
            {
                return userDataDirectory;
            }

            pathBuffer[ L_ARRAY_COUNT( pathBuffer ) - 1 ] = L_T( '\0' );
            userDataDirectory = pathBuffer;

            String subDirectory = AppInfo::GetName();
            if( subDirectory.IsEmpty() )
            {
                subDirectory = L_DEFAULT_USER_DATA_DIRECTORY;
            }

            subDirectory += L_T( '\\' );

            Path::Combine( userDataDirectory, userDataDirectory, subDirectory );
            File::EDirectoryCreateResult createResult = File::CreateDirectory( userDataDirectory, true );
            if( createResult == File::DIRECTORY_CREATE_RESULT_FAILED )
            {
                userDataDirectory.Clear();

                return userDataDirectory;
            }

            userDataDirectory.Trim();
        }

        return userDataDirectory;
    }

    /// Create a new file stream object for this platform.
    ///
    /// @return  File stream object.
    FileStream* File::CreateStream()
    {
        FileStream* pStream = new FileStreamWin;
        L_ASSERT( pStream );

        return pStream;
    }

    /// Test whether a file or directory exists
    ///
    /// @param[in] pPath  Path name of the file or directory to check.
    ///
    /// @return  True if the file or directory exists, false if not.
    bool File::Exists( const tchar_t* pPath )
    {
        L_ASSERT( pPath );

        WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
        BOOL bResult = _GET_FILE_ATTRIBUTES_EX( pPath, GetFileExInfoStandard, &fileAttributes );

        return( bResult != FALSE );
    }

    /// Get the type of a file entry (i.e. regular file, directory, etc.).
    ///
    /// @param[in] pPath  Path name of the file or directory to check.
    ///
    /// @return  Type of the entry at the specified path, or TYPE_INVALID if the entry does not exist.
    File::EType File::GetFileType( const tchar_t* pPath )
    {
        L_ASSERT( pPath );

        WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
        BOOL bResult = _GET_FILE_ATTRIBUTES_EX( pPath, GetFileExInfoStandard, &fileAttributes );
        if( !bResult )
        {
            return TYPE_INVALID;
        }

        if( fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            return TYPE_DIRECTORY;
        }

        return TYPE_FILE;
    }

    /// Get the size of a file.
    ///
    /// @param[in] pPath  Path name of the file to check.
    ///
    /// @return  Size of the file if it exists, is a file, and the size could be retrieved successfully, or -1 if the
    ///          file size could not be retrieved.
    int64_t File::GetSize( const tchar_t* pPath )
    {
        L_ASSERT( pPath );

        WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
        BOOL bResult = _GET_FILE_ATTRIBUTES_EX( pPath, GetFileExInfoStandard, &fileAttributes );
        if( !bResult || ( fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            return -1;
        }

        uint64_t size =
            static_cast< uint64_t >( fileAttributes.nFileSizeLow ) |
            ( static_cast< uint64_t >( fileAttributes.nFileSizeHigh ) << 32 );

        return static_cast< int64_t >( size );
    }

    /// Get the timestamp of a file or directory.
    ///
    /// @param[in] pPath  Path name of the file or directory to check.
    ///
    /// @return  64-bit timestamp value, or INT64_MIN if acquiring the timestamp failed.  Note that the interpretation
    ///          of this value is platform dependent, but results should be consistent across the same platform.
    int64_t File::GetTimestamp( const tchar_t* pPath )
    {
        L_ASSERT( pPath );

        WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
        BOOL bResult = _GET_FILE_ATTRIBUTES_EX( pPath, GetFileExInfoStandard, &fileAttributes );
        if( !bResult )
        {
            return INT64_MIN;
        }

        ULARGE_INTEGER timestamp;
        timestamp.LowPart = fileAttributes.ftLastWriteTime.dwLowDateTime;
        timestamp.HighPart = fileAttributes.ftLastWriteTime.dwHighDateTime;

        return static_cast< int64_t >( timestamp.QuadPart );
    }

    /// Iterate over the listing of files in the given directory.
    ///
    /// Note that this will skip over virtual entries for referencing back to the current directory (".") and the parent
    /// directory ("..") if they exist.
    ///
    /// @param[in] pPath  Path of the directory over which to iterate.
    ///
    /// @return  Directory iterator instance.
    DirectoryIterator* File::IterateDirectory( const tchar_t* pPath )
    {
        DirectoryIteratorWin* pIterator = new DirectoryIteratorWin( pPath );
        L_ASSERT( pIterator );

        return pIterator;
    }

    /// Get the full path to the base directory for the application.
    ///
    /// @return  Base application directory path, with a trailing path separator character.
    ///
    /// @see GetDataDirectory(), GetUserDataDirectory()
    const String& File::GetBaseDirectory()
    {
        return GetMutableBaseDirectory();
    }

    /// Get the full path to the base directory in which data files are stored.
    ///
    /// @return  Data directory path, with a trailing path separator character.
    ///
    /// @see GetBaseDirectory(), GetUserDataDirectory()
    const String& File::GetDataDirectory()
    {
        return GetMutableDataDirectory();
    }

    /// Get the full path to the base directory in which user-specific data files are stored.
    ///
    /// @return  User data directory path, with a trailing path separator character.
    ///
    /// @see GetBaseDirectory(), GetDataDirectory()
    const String& File::GetUserDataDirectory()
    {
        return GetMutableUserDataDirectory();
    }

    /// Free any resources statically allocated for platform-specific purposes.
    void File::PlatformShutdown()
    {
        GetMutableUserDataDirectory().Clear();
        GetMutableDataDirectory().Clear();
        GetMutableBaseDirectory().Clear();
    }

    /// Create a directory non-recursively.
    ///
    /// @param[in] pPath  Path of the directory to create.
    ///
    /// @return  Identifier for the result of the directory creation attempt.
    File::EDirectoryCreateResult File::PlatformCreateDirectory( const tchar_t* pPath )
    {
        L_ASSERT( pPath );

        EDirectoryCreateResult result = DIRECTORY_CREATE_RESULT_SUCCESS;
        if( !::CreateDirectory( pPath, NULL ) )
        {
            DWORD createError = GetLastError();
            result =
                ( createError == ERROR_ALREADY_EXISTS
                  ? DIRECTORY_CREATE_RESULT_ALREADY_EXISTS
                  : DIRECTORY_CREATE_RESULT_FAILED );
        }

        return result;
    }
}

#endif  // L_OS_WIN
