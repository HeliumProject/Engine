#pragma once

#include "Editor/API.h"
#include "CommandQueue.h"
#include "Foundation/Memory/SmartPtr.h"

#include <wx/timer.h>

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Base class for top level windows in Editor.
        // 
        class EDITOR_EDITOR_API Frame HELIUM_ABSTRACT : public wxFrame
        {
            // 
            // Member variables
            //
        private:
            typedef std::map< i32, wxWindow* > M_MenuIdToPanel;
            M_MenuIdToPanel m_Panels;

        protected:
            CommandQueue m_CommandQueue;
            wxAuiManager m_FrameManager;

            wxTimer*  m_HelpTimer;
            wxWindow* m_HelpLastWindow;
            // 
            // General member functions
            // 
        public:
            Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT( "frame" ) );
            virtual ~Frame();

            virtual void SaveWindowState() = 0;

            void PostCommand( const Undo::CommandPtr& command );

            virtual const tstring& GetPreferencePrefix() const = 0;
            virtual tstring PrefixPreferenceKey( const tstring& key ) const;

            virtual void SetHelpText( const tchar* text )
            {
            }

        protected:
            u32 CreatePanelsMenu( wxMenu* menu );
            void UpdatePanelsMenu( wxMenu* menu );

            // 
            // UI event handlers
            // 
        private:
            void OnSetFocus( wxFocusEvent& args );
            void OnShowPanel( wxCommandEvent& args );
            void OnExiting( wxCloseEvent& args );
            void OnHelpTimer( wxTimerEvent& evt );

        protected:
            DECLARE_EVENT_TABLE();
        };

    }
}