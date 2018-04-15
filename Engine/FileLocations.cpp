#include "Precompile.h"
#include "FileLocations.h"

#include "Platform/Process.h"
#include "Foundation/FileStream.h"

using namespace Helium;

static FilePath g_BaseDirectory;
static bool g_BaseDirectoryRequested = false;
static bool g_BaseDirectorySuccess = false;
static FilePath g_DataDirectory;
static bool g_DataDirectoryRequested = false;
static bool g_DataDirectorySuccess = false;
static FilePath g_UserDirectory;
static bool g_UserDirectoryRequested = false;
static bool g_UserDirectorySuccess = false;

/// Get the full path to the base directory of the application.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  Application base directory path, with a trailing path separator character.
static FilePath& GetMutableBaseDirectory( bool& rbSuccess )
{
	rbSuccess = g_BaseDirectorySuccess;

	if( !g_BaseDirectoryRequested )
	{
		g_BaseDirectoryRequested = true;

		// TODO - invent a heuristic to locate game data arbitrarily (~/HeliumProject)?
		rbSuccess = g_BaseDirectorySuccess = HELIUM_VERIFY( false );
	}

	return g_BaseDirectory;
}

/// Get the full path to the base directory in which data files are stored.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  Data directory path, with a trailing path separator character.
static FilePath& GetMutableDataDirectory( bool& rbSuccess )
{
	rbSuccess = g_DataDirectorySuccess;

	if( !g_DataDirectoryRequested )
	{
		g_DataDirectoryRequested = true;

		// Get the application base directory.
		bool bBaseDirectorySuccess;
		g_DataDirectory = GetMutableBaseDirectory( bBaseDirectorySuccess );
		if( !bBaseDirectorySuccess )
		{
			g_DataDirectory.Clear();

			return g_DataDirectory;
		}

		g_DataDirectory += "Data";
		if( !HELIUM_VERIFY( g_DataDirectory.Exists() ) )
		{
			g_DataDirectory.Clear();

			return g_DataDirectory;
		}

		g_DataDirectory += "/";
		g_DataDirectorySuccess = true;
		rbSuccess = true;
	}

	return g_DataDirectory;
}

/// Get the full path to the base directory in which user data is stored.
///
/// @param[out] rbSuccess  True if the path was retrieved successfully, false if not.
///
/// @return  User data directory path, with a trailing path separator character.
static FilePath& GetMutableUserDataDirectory( bool& rbSuccess )
{
	rbSuccess = g_UserDirectorySuccess;

	if( !g_UserDirectoryRequested )
	{
		g_UserDirectoryRequested = true;

		std::string homeDirectory = Helium::GetHomeDirectory();
		if ( homeDirectory.empty() )
		{
			return g_UserDirectory;
		}

		String subDirectory ( GetProcessName().c_str() );
		g_UserDirectory.Set( homeDirectory + "/.Helium/" + subDirectory.GetData() );
		if( !g_UserDirectory.MakePath() )
		{
			g_UserDirectory.Clear();
			return g_UserDirectory;
		}

		g_UserDirectory += "/";
		g_UserDirectorySuccess = true;
		rbSuccess = true;
	}

	return g_UserDirectory;
}

/// Free any statically allocated resources.
///
/// This should only be called immediately prior to application exit.
void FileLocations::Shutdown()
{
	g_BaseDirectory.Clear();
	g_BaseDirectoryRequested = false;
	g_BaseDirectorySuccess = false;
	g_DataDirectory.Clear();
	g_DataDirectoryRequested = false;
	g_DataDirectorySuccess = false;
	g_UserDirectory.Clear();
	g_UserDirectoryRequested = false;
	g_UserDirectorySuccess = false;
}

/// Set the full path to the base directory for the application.
///
/// @see GetDataDirectory(), GetUserDirectory()
void FileLocations::SetBaseDirectory( const FilePath& path )
{
	g_BaseDirectory = path;
	g_BaseDirectoryRequested = true;
	g_BaseDirectorySuccess = true;
}

/// Get the full path to the base directory for the application.
///
/// @return  Base application directory path, with a trailing path separator character.
///
/// @see GetDataDirectory(), GetUserDirectory()
bool FileLocations::GetBaseDirectory( FilePath& path )
{
	bool bSuccess;
	path = GetMutableBaseDirectory( bSuccess );
	return bSuccess;
}

/// Get the full path to the base directory in which data files are stored.
///
/// @return  Data directory path, with a trailing path separator character.
///
/// @see GetBaseDirectory(), GetUserDirectory()
bool FileLocations::GetDataDirectory( FilePath& path )
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
bool FileLocations::GetUserDirectory( FilePath& path )
{
	bool bSuccess;
	path = GetMutableUserDataDirectory( bSuccess );
	return bSuccess;
}
