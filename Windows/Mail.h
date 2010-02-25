#pragma once

#include "API.h"
#include "Common/Types.h"

namespace Windows
{
  // dispach Mail message to auto_tools_bugs
  WINDOWS_API bool SendMail(const std::string& subject,
                          const std::string& text,
                          const V_string& files,
                          std::string destination = std::string (),
                          std::string source = std::string ());

  // handy for single files
  inline bool SendMail(const std::string& subject,
                       const std::string& text = std::string (),
                       const std::string& file = std::string (),
                       std::string destination = std::string (),
                       std::string source = std::string ())
  {
    V_string files;
    if (!file.empty())
    {
      files.push_back(file);
    }
    return SendMail(subject, text, files, destination, source);
  }
}