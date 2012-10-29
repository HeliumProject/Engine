#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#include <string>

namespace Helium
{
    //
    // Creates a new process with no window or output, use it for running scripts and file association apps
    //

    HELIUM_PLATFORM_API int Execute( const tstring& command, bool showWindow = false, bool block = false );

    //
    // Creates a new process and captures its standard out and standard error into the passed string
    //

    HELIUM_PLATFORM_API int Execute( const tstring& command, tstring& output, bool showWindow = false );

    //
    // Get a unique string for this process
    //

    HELIUM_PLATFORM_API tstring GetProcessString();

    //
    // Get the application path for this process
    //

    HELIUM_PLATFORM_API tstring GetProcessPath();

    //
    // Get the executable name for this process
    //

    HELIUM_PLATFORM_API tstring GetProcessName();

    //
    // Get the crashdump directory
    //
    HELIUM_PLATFORM_API tstring GetDumpDirectory();
}