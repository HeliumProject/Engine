//----------------------------------------------------------------------------------------------------------------------
// File.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "Foundation/File/File.h"
#include "Foundation/Stream/FileStream.h"
#include "Foundation/AppInfo.h"

#include "Platform/Environment.h"
#include "Platform/Process.h"

using namespace Helium;

/// Get the full path to the base directory of the application.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  Application base directory path, with a trailing path separator character.
static Path& GetMutableBaseDirectory( bool& rbSuccess )
{
    static Path baseDirectory;
    static bool bLocateRequested = false;
    static bool bLocateSuccess = false;

    rbSuccess = bLocateSuccess;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        baseDirectory.Set( Helium::GetProcessPath() );

        // Strip the executable file.
        // Strip the configuration type subdirectory (i.e. Debug, Intermediate, Release, etc.).
        // Strip the platform binary subdirectory (i.e. x32, x64).
        // Strip the "Bin" directory.
        baseDirectory.Set( baseDirectory.Directory() + TXT( "../../.." ) );

        if( !baseDirectory.Exists() )
        {
            baseDirectory.Clear();

            return baseDirectory;
        }

        baseDirectory += TXT( "/" );
        bLocateSuccess = true;
        rbSuccess = true;
    }

    return baseDirectory;
}

/// Get the full path to the base directory in which data files are stored.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  Data directory path, with a trailing path separator character.
static Path& GetMutableDataDirectory( bool& rbSuccess )
{
    static Path dataDirectory;
    static bool bLocateRequested = false;
    static bool bLocateSuccess = false;

    rbSuccess = bLocateSuccess;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        // Get the application base directory.
        bool bBaseDirectorySuccess;
        dataDirectory = GetMutableBaseDirectory( bBaseDirectorySuccess );
        if( !bBaseDirectorySuccess )
        {
            dataDirectory.Clear();

            return dataDirectory;
        }

        dataDirectory += TXT( "Data" );
        if( !dataDirectory.Exists() )
        {
            dataDirectory.Clear();

            return dataDirectory;
        }

        dataDirectory += TXT( "/" );
        bLocateSuccess = true;
        rbSuccess = true;
    }

    return dataDirectory;
}

/// Get the full path to the base directory in which user data is stored.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  User data directory path, with a trailing path separator character.
static Path& GetMutableUserDataDirectory( bool& rbSuccess )
{
    static Path userDataDirectory;
    static bool bLocateRequested = false;
    static bool bLocateSuccess = false;

    rbSuccess = bLocateSuccess;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        tstring prefsDir;
        if( !Helium::GetPreferencesDirectory( prefsDir ) )
        {
            return userDataDirectory;
        }

        String subDirectory = AppInfo::GetName();
        if( subDirectory.IsEmpty() )
        {
            subDirectory = TXT( "Helium" );
        }

        userDataDirectory.Set( prefsDir + TXT( "/" ) + subDirectory.GetData() );
        if( !userDataDirectory.MakePath() )
        {
            userDataDirectory.Clear();

            return userDataDirectory;
        }

        userDataDirectory += TXT( "/" );
        bLocateSuccess = true;
        rbSuccess = true;
    }

    return userDataDirectory;
}

/// Free any statically allocated resources.
///
/// This should only be called immediately prior to application exit.
void File::Shutdown()
{
    PlatformShutdown();
}

/// Attempt to open a file with a new file stream object.
///
/// @param[in] pPath      Path name of the file to open.
/// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
/// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
///
/// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
///          longer needed.
FileStream* File::Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
{
    FileStream* pStream = CreateStream();
    HELIUM_ASSERT( pStream );
    if( !pStream->Open( pPath, modeFlags, bTruncate ) )
    {
        delete pStream;
        return NULL;
    }

    return pStream;
}

/// Attempt to open a file with a new file stream object.
///
/// @param[in] rPath      Path name of the file to open.
/// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
/// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
///
/// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
///          longer needed.
FileStream* File::Open( const String& rPath, uint32_t modeFlags, bool bTruncate )
{
    return Open( *rPath, modeFlags, bTruncate );
}

/// Create a new file stream object for this platform.
///
/// @return  File stream object.
FileStream* File::CreateStream()
{
    FileStream* pStream = new FileStream();
    HELIUM_ASSERT( pStream );

    return pStream;
}

/// Get the full path to the base directory for the application.
///
/// @return  Base application directory path, with a trailing path separator character.
///
/// @see GetDataDirectory(), GetUserDataDirectory()
const bool File::GetBaseDirectory( Path& path )
{
    bool bSuccess;
    path = GetMutableBaseDirectory( bSuccess );

    return bSuccess;
}

/// Get the full path to the base directory in which data files are stored.
///
/// @return  Data directory path, with a trailing path separator character.
///
/// @see GetBaseDirectory(), GetUserDataDirectory()
const bool File::GetDataDirectory( Path& path )
{
    bool bSuccess;
    path = GetMutableDataDirectory( bSuccess );

    return bSuccess;
}

/// Get the full path to the base directory in which user-specific data files are stored.
///
/// @return  User data directory path, with a trailing path separator character.
///
/// @see GetBaseDirectory(), GetDataDirectory()
const bool File::GetUserDataDirectory( Path& path )
{
    bool bSuccess;
    path = GetMutableUserDataDirectory( bSuccess );

    return bSuccess;
}

/// Free any resources statically allocated for platform-specific purposes.
void File::PlatformShutdown()
{
    bool bSuccess;
    GetMutableUserDataDirectory( bSuccess ).Clear();
    GetMutableDataDirectory( bSuccess ).Clear();
    GetMutableBaseDirectory( bSuccess ).Clear();
}


