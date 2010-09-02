#pragma once

// Includes
#include "Foundation/API.h"
#include "Foundation/Startup.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Inspect
    {
        FOUNDATION_API void Initialize();
        FOUNDATION_API void Cleanup();

        struct EditFilePathArgs
        {
            tstring m_File;

            EditFilePathArgs( const tstring& file )
                : m_File( file )
            {

            }
        };
        typedef Helium::Signature< const EditFilePathArgs&> EditFilePathSignature;

        extern FOUNDATION_API EditFilePathSignature::Event g_EditFilePath;
    }
}