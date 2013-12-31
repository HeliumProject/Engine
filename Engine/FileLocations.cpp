#include "EnginePch.h"
#include "FileLocations.h"

#include "Platform/Process.h"

#include "Foundation/FileStream.h"

using namespace Helium;

/// Get the full path to the base directory of the application.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  Application base directory path, with a trailing path separator character.
static FilePath& GetMutableBaseDirectory( bool& rbSuccess )
{
	static FilePath baseDirectory;
	static bool bLocateRequested = false;
	static bool bLocateSuccess = false;

	rbSuccess = bLocateSuccess;

	if( !bLocateRequested )
	{
		bLocateRequested = true;

		baseDirectory.Set( Helium::GetProcessPath() );

#if HELIUM_OS_MAC
		// Strip the executable file.
		// Strip MacOS
		// Strip Content
		// Strip Helium.app
		// Strip the configuration type subdirectory (i.e. Debug, Intermediate, Release, etc.).
		// Strip the "Bin" directory.
		baseDirectory.Set( baseDirectory.Directory() + TXT( "../../../../.." ) );
#else
		// Strip the executable file.
		// Strip the configuration type subdirectory (i.e. Debug, Intermediate, Release, etc.).
		// Strip the "Bin" directory.
		baseDirectory.Set( baseDirectory.Directory() + TXT( "../.." ) );
#endif

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
static FilePath& GetMutableDataDirectory( bool& rbSuccess )
{
	static FilePath dataDirectory;
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
static FilePath& GetMutableUserDataDirectory( bool& rbSuccess )
{
	static FilePath userDataDirectory;
	static bool bLocateRequested = false;
	static bool bLocateSuccess = false;

	rbSuccess = bLocateSuccess;

	if( !bLocateRequested )
	{
		bLocateRequested = true;

		std::string gameDataDirectory = Helium::GetAppDataDirectory();
		if ( gameDataDirectory.empty() )
		{
			return userDataDirectory;
		}

		String subDirectory ( GetProcessName().c_str() );
		userDataDirectory.Set( gameDataDirectory + TXT( "/" ) + subDirectory.GetData() );
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
void FileLocations::Shutdown()
{
	PlatformShutdown();
}

/// Get the full path to the base directory for the application.
///
/// @return  Base application directory path, with a trailing path separator character.
///
/// @see GetDataDirectory(), GetUserDataDirectory()
const bool FileLocations::GetBaseDirectory( FilePath& path )
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
const bool FileLocations::GetDataDirectory( FilePath& path )
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
const bool FileLocations::GetUserDataDirectory( FilePath& path )
{
	bool bSuccess;
	path = GetMutableUserDataDirectory( bSuccess );

	return bSuccess;
}

/// Free any resources statically allocated for platform-specific purposes.
void FileLocations::PlatformShutdown()
{
	bool bSuccess;
	GetMutableUserDataDirectory( bSuccess ).Clear();
	GetMutableDataDirectory( bSuccess ).Clear();
	GetMutableBaseDirectory( bSuccess ).Clear();
}


