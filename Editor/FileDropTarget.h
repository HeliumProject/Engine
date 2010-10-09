#pragma once

#include "Editor/API.h"

#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

#include "Platform/Compiler.h"

#include <wx/dnd.h>

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////
        struct FileDroppedArgs
        {
            wxCoord m_X;
            wxCoord m_Y;
            wxDragResult m_DragResult;
            Helium::Path m_Path;

            FileDroppedArgs( wxCoord x, wxCoord y, wxDragResult result, const tstring& path = TXT( "" ) )
                : m_X( x )
                , m_Y( y )
                , m_DragResult( result )
                , m_Path( path )
            {
            }
        };
        typedef Helium::Signature< const FileDroppedArgs& > FileDroppedSignature;

        typedef Helium::Signature< const FileDroppedArgs& > FileDragEnterSignature;
        typedef Helium::Signature< const FileDroppedArgs& > FileDragOverSignature;
        typedef Helium::Signature< Helium::Void > FileDragLeaveSignature;
        typedef Helium::Signature< const FileDroppedArgs& > FileDropSignature;


        ///////////////////////////////////////////////////////////////////////
        class FileDropTarget : public wxFileDropTarget
        {
        public:
            FileDropTarget( const std::set< tstring >& extensions );
            FileDropTarget( const tstring& extensions, const tstring& delims = TXT( "," ) );

            void AddDroppedListener( FileDroppedSignature::Delegate& listener );
            void RemoveDroppedListener( FileDroppedSignature::Delegate& listener );

            //void AddDragEnterListener( const FileDragEnterSignature::Delegate& listener );
            void AddDragOverListener( const FileDragOverSignature::Delegate& listener );
            //void AddDragLeaveListener( const FileDragLeaveSignature::Delegate& listener );
            //void AddDropListener( const FileDropSignature::Delegate& listener );

        protected:
            bool TestExtension( const tchar* testExt );

            virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames ) HELIUM_OVERRIDE;

            //virtual wxDragResult OnEnter( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;
            virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;
            //virtual void OnLeave();
            //virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def ) HELIUM_OVERRIDE;

        protected:
            std::set< tstring > m_FileExtensions;

            FileDroppedSignature::Event m_DroppedEvent;

            //FileDragEnterSignature::Event m_DragEnterEvent;
            FileDragOverSignature::Event m_DragOverEvent;
            //FileDragLeaveSignature::Event m_DragLeaveEvent;
            //FileDropSignature::Event m_DropEvent;
        };
    }
}