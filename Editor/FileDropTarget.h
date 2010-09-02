#pragma once

#include "Editor/API.h"

#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

#include <wx/dnd.h>

namespace Helium
{
    namespace Editor
    {
        struct FileDroppedArgs
        {
            Helium::Path m_Path;

            FileDroppedArgs( const tstring& path = TXT( "" ) )
                : m_Path( path )
            {
            }
        };
        typedef Helium::Signature< const FileDroppedArgs& > FileDroppedSignature;
        typedef std::set< FileDroppedSignature::Delegate > S_FileDroppedSignature;

        class FileDropTarget : public wxFileDropTarget
        {
        public:
            FileDropTarget( const std::set< tstring >& extensions );
            FileDropTarget( const tstring& extensions, const tstring& delims = TXT( "," ) );

            void AddListener( FileDroppedSignature::Delegate& listener )
            {
                m_DropEvent.Add( listener );
            }

            void RemoveListener( FileDroppedSignature::Delegate& listener )
            {
                m_DropEvent.Remove( listener );
            }

        protected:
            virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames );

        protected:
            FileDroppedSignature::Event m_DropEvent;
            std::set< tstring >         m_FileExtensions;
        };
    }
}