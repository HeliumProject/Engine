#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#include <string>

namespace Platform
{
  //
  // Creates a new process with no window or output, use it for running scripts and file association apps
  //

  PLATFORM_API int Execute( const tstring& command, bool showWindow = false, bool block = false );

  //
  // Creates a new process and captures its standard out and standard error into the passed string
  //

  PLATFORM_API int Execute( const tstring& command, tstring& output, bool showWindow = false );

  //
  // Get a unique string for this process
  //

  PLATFORM_API tstring GetProcessString();
}