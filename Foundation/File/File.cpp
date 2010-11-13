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


using namespace Lunar;

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

/// Get the type of a file entry (i.e. regular file, directory, etc.).
///
/// @param[in] rPath  Path name of the file or directory to check.
///
/// @return  Type of the entry at the specified path, or TYPE_INVALID if the entry does not exist.
File::EType File::GetFileType( const String& rPath )
{
    return GetFileType( *rPath );
}

/// Get the full path to the base directory of the application.
///
/// @return  Application base directory path, with a trailing path separator character.
static bool GetMutableBaseDirectory( Path& path )
{
    static Path baseDirectory;
    static bool bLocateRequested = false;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        baseDirectory.Set( Helium::GetProcessPath() );

        // Strip the executable file.
        // Strip the configuration type subdirectory (i.e. Debug, Intermediate, Release, etc.).
        // Strip the platform binary subdirectory (i.e. x32, x64).
        // Strip the "Bin" directory.
        baseDirectory.Set( baseDirectory.Directory() + TXT( "../../../" ) );

        if( !baseDirectory.Exists() )
        {
            bLocateRequested = false;
            return false;
        }
    }

    path = baseDirectory;
    return true;
}

/// Get the full path to the base directory in which data files are stored.
///
/// @return  Data directory path, with a trailing path separator character.
static bool GetMutableDataDirectory( Path& path )
{
    static Path dataDirectory;
    static bool bLocateRequested = false;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        // Get the application base directory.
        if ( !GetMutableBaseDirectory( dataDirectory ) )
        {
            return false;
        }

        dataDirectory += TXT( "Data/" );

        if( !dataDirectory.Exists() )
        {
            bLocateRequested = false;
            return false;
        }
    }

    path = dataDirectory;
    return true;
}

/// Get the full path to the base directory in which user data is stored.
///
/// @return  User data directory path, with a trailing path separator character.
static bool GetMutableUserDataDirectory( Path& path )
{
    static Path userDataDirectory;
    static bool bLocateRequested = false;

    if( !bLocateRequested )
    {
        bLocateRequested = true;

        tstring prefsDir;
        if ( !Helium::GetPreferencesDirectory( prefsDir ) )
        {
            bLocateRequested = false;
            return false;
        }


        String subDirectory = AppInfo::GetName();
        if( subDirectory.IsEmpty() )
        {
            subDirectory = TXT( "Lunar" );
        }

        userDataDirectory.Set( prefsDir + TXT( "/" ) + subDirectory.GetData() + TXT( "/" ) );

        if ( !userDataDirectory.MakePath() )
        {
            bLocateRequested = false;
            return false;
        }
    }

    path = userDataDirectory;
    return true;
}

/// Create a new file stream object for this platform.
///
/// @return  File stream object.
FileStream* File::CreateStream()
{
    FileStream* pStream = new FileStreamWin;
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
    return GetMutableBaseDirectory( path );
}

/// Get the full path to the base directory in which data files are stored.
///
/// @return  Data directory path, with a trailing path separator character.
///
/// @see GetBaseDirectory(), GetUserDataDirectory()
const bool File::GetDataDirectory( Path& path )
{
    return GetMutableDataDirectory( path );
}

/// Get the full path to the base directory in which user-specific data files are stored.
///
/// @return  User data directory path, with a trailing path separator character.
///
/// @see GetBaseDirectory(), GetDataDirectory()
const bool File::GetUserDataDirectory( Path& path )
{
    return GetMutableUserDataDirectory( path );
}

/// Free any resources statically allocated for platform-specific purposes.
void File::PlatformShutdown()
{
    //GetMutableUserDataDirectory().Clear();
    //GetMutableDataDirectory().Clear();
    //GetMutableBaseDirectory().Clear();
}


