#pragma once

#include "Editor/API.h"

#include "Foundation/FilePath.h"
#include "Foundation/Event.h"

#include <wx/dnd.h>

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////
        struct FileDroppedArgs
        {
            Helium::Path m_Path;
            wxCoord m_X;
            wxCoord m_Y;
            wxDragResult m_DragResult;

            FileDroppedArgs( const tstring& path, wxCoord x = 0, wxCoord y = 0, wxDragResult result = wxDragNone )
                : m_Path( path )
                , m_X( x )
                , m_Y( y )
                , m_DragResult( result )
            {
            }
        };
        typedef Helium::Signature< const FileDroppedArgs& > FileDroppedSignature;

        //typedef Helium::Signature< const FileDroppedArgs& > FileDragEnterSignature;
        typedef Helium::Signature< FileDroppedArgs& > FileDragOverSignature;
        //typedef Helium::Signature< Helium::Void > FileDragLeaveSignature;
        //typedef Helium::Signature< const FileDroppedArgs& > FileDropSignature;


        ///////////////////////////////////////////////////////////////////////
        class FileDropTarget : public wxFileDropTarget
        {
        public:
            FileDropTarget( const std::set< tstring >& extensions );
            FileDropTarget( const tstring& extensions = TXT( "" ), const tstring& delims = TXT( "," ) );

            //void AddDragEnterListener( const FileDragEnterSignature::Delegate& listener );
            void AddDragOverListener( const FileDragOverSignature::Delegate& listener );
            //void AddDragLeaveListener( const FileDragLeaveSignature::Delegate& listener );
            void AddDroppedListener( FileDroppedSignature::Delegate& listener );

        protected:
            bool TestExtension( const tchar_t* testExt );

            //virtual wxDragResult OnEnter( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;
            virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;
            //virtual void OnLeave();
            //virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;
            virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames ) HELIUM_OVERRIDE;

        protected:
            std::set< tstring > m_FileExtensions;

            //FileDragEnterSignature::Event m_DragEnterEvent;
            FileDragOverSignature::Event m_DragOverEvent;
            //FileDragLeaveSignature::Event m_DragLeaveEvent;
            FileDroppedSignature::Event m_DroppedEvent;
        };
    }
}