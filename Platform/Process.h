#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    /// Creates a new process with no window or output, use it for running scripts and file association apps
    HELIUM_PLATFORM_API int Execute( const tstring& command, bool showWindow = false, bool block = false );

    /// Creates a new process and captures its standard out and standard error into the passed string
    HELIUM_PLATFORM_API int Execute( const tstring& command, tstring& output, bool showWindow = false );

    /// Get a unique string for this process
    HELIUM_PLATFORM_API tstring GetProcessString();

    /// Get the application path for this process
    HELIUM_PLATFORM_API tstring GetProcessPath();

    /// Get the executable name for this process
    HELIUM_PLATFORM_API tstring GetProcessName();

	/// Get username of the current proess
    HELIUM_PLATFORM_API tstring GetUserName();
    
	/// Get machine name of the current process
	HELIUM_PLATFORM_API tstring GetMachineName();
    
	/// Location for user preferences on disk
	HELIUM_PLATFORM_API tstring GetPreferencesDirectory();

	/// Location for app cache data on disk
    HELIUM_PLATFORM_API tstring GetAppDataDirectory();

    /// Location for crash dumps
    HELIUM_PLATFORM_API tstring GetDumpDirectory();
}