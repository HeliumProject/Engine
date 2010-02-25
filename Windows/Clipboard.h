#pragma once

#include <string>

#ifndef REQUIRES_WINDOWS_H
#define REQUIRES_WINDOWS_H
#endif
#include "API.h"

namespace Windows
{
  WINDOWS_API bool CopyToClipboard( HWND owner, const std::string& data, std::string& error );

  WINDOWS_API bool RetrieveFromClipboard( HWND owner, std::string& data, std::string& error );
}