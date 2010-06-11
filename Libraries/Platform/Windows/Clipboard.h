#pragma once

#include <string>

#include "Platform/API.h"

namespace Platform
{
  PLATFORM_API bool CopyToClipboard( HWND owner, const std::string& data, std::string& error );

  PLATFORM_API bool RetrieveFromClipboard( HWND owner, std::string& data, std::string& error );
}