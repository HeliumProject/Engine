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
    tstring m_File;

    EditFilePathArgs( const tstring& file )
      : m_File( file )
    {

    }
  };
  typedef Nocturnal::Signature<void, const EditFilePathArgs&> EditFilePathSignature;

  extern APPLICATION_API EditFilePathSignature::Event g_EditFilePath;
}

