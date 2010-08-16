#pragma once

// Includes
#include "Application/API.h"
#include "Foundation/Startup.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
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
        typedef Helium::Signature<void, const EditFilePathArgs&> EditFilePathSignature;

        extern APPLICATION_API EditFilePathSignature::Event g_EditFilePath;
    }
}