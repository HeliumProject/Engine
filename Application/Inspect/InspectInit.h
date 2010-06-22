#pragma once

// Includes
#include "Application/API.h"
#include "Application/Application.h"
#include "Foundation/Automation/Event.h"

namespace Inspect
{
  APPLICATION_API void Initialize();
  APPLICATION_API void Cleanup();

  struct EditFilePathArgs
  {
    std::string m_File;

    EditFilePathArgs( const std::string& file )
      : m_File( file )
    {

    }
  };
  typedef Nocturnal::Signature<void, const EditFilePathArgs&> EditFilePathSignature;

  extern APPLICATION_API EditFilePathSignature::Event g_EditFilePath;
}

