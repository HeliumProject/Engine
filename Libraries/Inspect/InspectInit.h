#pragma once

// Includes
#include "API.h"
#include "AppUtils/AppUtils.h"
#include "Foundation/Automation/Event.h"

namespace Inspect
{
  INSPECT_API void Initialize();
  INSPECT_API void Cleanup();

  struct EditFilePathArgs
  {
    std::string m_File;

    EditFilePathArgs( const std::string& file )
      : m_File( file )
    {

    }
  };
  typedef Nocturnal::Signature<void, const EditFilePathArgs&> EditFilePathSignature;

  extern INSPECT_API EditFilePathSignature::Event g_EditFilePath;
}

