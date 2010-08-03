#pragma once

#include "Editor/API.h"
#include "CommandQueue.h"
#include "Foundation/Memory/SmartPtr.h"

#include <wx/timer.h>

namespace Helium
{
    namespace Editor
    {
        typedef std::map< i32, wxWindow* > M_MenuIdToPanel;

        class EDITOR_EDITOR_API Frame HELIUM_ABSTRACT : public wxFrame
        {
        public:
            Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT( "frame" ) );
            virtual ~Frame();

            virtual void PostCommand( const Undo::CommandPtr& command );
            virtual void SetHelpText( const tchar* text );

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
            CommandQueue m_CommandQueue;
            wxAuiManager m_FrameManager;

            wxTimer*  m_HelpTimer;
            wxWindow* m_HelpLastWindow;

            DECLARE_EVENT_TABLE();
        };
    }
}