#pragma once

#include "API.h"

#include <string>
#include <vector>

namespace Windows
{
  //
  // Creates a new process with no window or output, use it for running scripts and file association apps
  //

  WINDOWS_API int Execute( const std::string& command, bool showWindow = false, bool block = false );

  //
  // Creates a new process and captures its standard out and standard error into the passed string
  //

  WINDOWS_API int Execute( const std::string& command, std::string& output, bool showWindow = false );

  //
  // Creates a new process and throws an exception if it fails (and includes its output)
  //

  WINDOWS_API void TryExecute( const std::string& command, int successCode = 0, bool showWindow = false );

  //
  // Get a unique string for this process
  //

  WINDOWS_API std::string GetProcessString();

  //
  // Helps keep virtual address space fragmentation at bay
  //

  WINDOWS_API bool EnableLowFragmentationHeap();

  //
  // Get a list of the current running processes
  //
  WINDOWS_API void GetCurrentRunningProcesses( std::vector< std::string >& current_processes );
}