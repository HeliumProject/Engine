#pragma once

#include "Foundation/Event.h"

#include <wx/dnd.h>

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class ClipboardDataObject;

        /////////////////////////////////////////////////////////////////////////////
        // Arguments for drag operations.
        // 
        struct DragArgs
        {
            DragArgs( wxCoord x, wxCoord y, ClipboardDataObject* clipboardData, wxDragResult def )
                : m_X( x )
                , m_Y( y )
                , m_ClipboardData( clipboardData )
                , m_Default( def )
                , m_Result( wxDragCancel )
            {
            }

            wxCoord m_X;
            wxCoord m_Y;
            ClipboardDataObject* m_ClipboardData;
            wxDragResult m_Default;
            mutable wxDragResult m_Result;
        };

        typedef Helium::Signature< const DragArgs& > DragEnterCallback;
        typedef Helium::Signature< const DragArgs& > DragOverCallback;
        typedef Helium::Signature< Helium::Void > DragLeaveCallback;
        typedef Helium::Signature< const DragArgs& > DropCallback;

        /////////////////////////////////////////////////////////////////////////////
        // MetaClass managing callbacks for when drag/drop operations occur.
        // 
        class DropTarget : public wxDropTarget
        {
        private:
            DragEnterCallback::Event    m_DragEnter;
            DragOverCallback::Event     m_DragOver;
            DragLeaveCallback::Event    m_DragLeave;
            DropCallback::Event         m_Drop;

        public:
            DropTarget();
            virtual ~DropTarget();

            void SetDragEnterCallback( const DragEnterCallback::Delegate& func );
            void SetDragOverCallback( const DragOverCallback::Delegate& func );
            void SetDragLeaveCallback( const DragLeaveCallback::Delegate& func );
            void SetDropCallback( const DropCallback::Delegate& func );

            virtual wxDragResult OnEnter( wxCoord x, wxCoord y, wxDragResult def ) override;
            virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def ) override;
            virtual void OnLeave() override;
            virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def ) override;
        };
    }
}