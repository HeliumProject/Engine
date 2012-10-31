#pragma once

#include "Foundation/FilePath.h"
#include "Foundation/Event.h"

namespace Helium
{
    //
    // Define a simple event-driven interface for message dialogs and questions
    //

    namespace FileDialogTypes
    {
        enum FileDialogType
        {
            OpenFile,
            SaveFile,
        };
    }
    typedef FileDialogTypes::FileDialogType FileDialogType;

    struct FileDialogArgs
    {
        FileDialogArgs( FileDialogType type, const tstring& caption, const tstring& filters, const Helium::Path& defaultDir = TXT(""), const Helium::Path& defaultFile = TXT(""))
            : m_Type( type )
            , m_Caption( caption )
            , m_Filters( filters )
            , m_DefaultDirectory( defaultDir )
            , m_DefaultFile( defaultFile )
        {

        }

        const tstring&      m_Caption;
        const tstring&      m_Filters;
        const Helium::Path  m_DefaultDirectory;
        const Helium::Path  m_DefaultFile;
        FileDialogType      m_Type;
        mutable Path        m_Result;
    };

    typedef Helium::Signature< const FileDialogArgs& > FileDialogSignature;
}