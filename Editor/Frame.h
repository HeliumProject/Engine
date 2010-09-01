#pragma once

#include "Editor/API.h"
#include "CommandQueue.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/Undo/Queue.h"

#include <wx/timer.h>

namespace Helium
{
    namespace Editor
    {
        typedef std::map< i32, wxWindow* > M_MenuIdToPanel;

        class Frame HELIUM_ABSTRACT : public wxFrame
        {
        public:
            Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT( "frame" ) );
            virtual ~Frame();

            virtual void SetHelpText( const tchar* text );

            // Undo/redo support
            virtual bool CanUndo();
            virtual bool CanRedo();
            virtual void Undo();
            virtual void Redo();
            virtual void Push( const Undo::CommandPtr& command );

        protected:
            u32 CreatePanelsMenu( wxMenu* menu );
            void UpdatePanelsMenu( wxMenu* menu );

            // UI event handlers
        private:
            void OnSetFocus( wxFocusEvent& args );
            void OnShowPanel( wxCommandEvent& args );
            void OnExiting( wxCloseEvent& args );
            void OnHelpTimer( wxTimerEvent& evt );

        private:
            M_MenuIdToPanel m_Panels;

        protected:
            Undo::Queue     m_UndoQueue;
            wxAuiManager    m_FrameManager;

            wxTimer*        m_HelpTimer;
            wxWindow*       m_HelpLastWindow;

            DECLARE_EVENT_TABLE();
        };
    }
}