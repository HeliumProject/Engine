#pragma once

#include "Editor/API.h"
#include "CommandQueue.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/UndoQueue.h"

#include <wx/timer.h>

namespace Helium
{
    namespace Editor
    {
        typedef std::map< int32_t, wxWindow* > M_MenuIdToPanel;

        class Frame HELIUM_ABSTRACT : public wxFrame
        {
        public:
            Frame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT( "frame" ) );
            virtual ~Frame();

            virtual void SetHelpText( const tchar_t* text );

            // Undo/redo support
            virtual bool CanUndo();
            virtual bool CanRedo();
            virtual void Undo();
            virtual void Redo();
            virtual void Push( const UndoCommandPtr& command );

        protected:
            uint32_t CreatePanelsMenu( wxMenu* menu );
            void UpdatePanelsMenu( wxMenu* menu );

            // UI event handlers
        private:
            void OnShowPanel( wxCommandEvent& args );
            void OnExiting( wxCloseEvent& args );
            void OnHelpTimer( wxTimerEvent& evt );

        private:
            M_MenuIdToPanel m_Panels;

        protected:
            UndoQueue     m_UndoQueue;
            wxAuiManager    m_FrameManager;

            std::set<wxString> m_ExcludeFromPanelsMenu;

            wxTimer*        m_HelpTimer;
            wxWindow*       m_HelpLastWindow;

            DECLARE_EVENT_TABLE();
        };
    }
}