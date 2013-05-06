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
        FileDialogArgs( FileDialogType type, const tstring& caption, const tstring& filters, const Helium::FilePath& defaultDir = Helium::FilePath( TXT("") ), const Helium::FilePath& defaultFile = Helium::FilePath ( TXT("") ))
            : m_Type( type )
            , m_Caption( caption )
            , m_Filters( filters )
            , m_DefaultDirectory( defaultDir )
            , m_DefaultFile( defaultFile )
        {

        }

        const tstring&      m_Caption;
        const tstring&      m_Filters;
        const Helium::FilePath  m_DefaultDirectory;
        const Helium::FilePath  m_DefaultFile;
        FileDialogType      m_Type;
        mutable FilePath        m_Result;
    };

    typedef Helium::Signature< const FileDialogArgs& > FileDialogSignature;
}