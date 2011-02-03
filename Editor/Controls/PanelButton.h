#pragma once

#include "Platform/Types.h"
#include <wx/button.h>
#include <wx/tglbtn.h>

namespace Helium
{
    namespace Editor
    {
        namespace PanelButtonOptions
        {
            enum PanelButtonOption
            {
                Toggle = 1 << 0,
            };

            const uint32_t Default = 0;
        }
        typedef uint32_t PanelButtonOption;

        class PanelButton: public wxPanel
        {        
        public:
            PanelButton();
            PanelButton( wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBORDER_THEME,
                const wxString& name = wxT( "PanelButton" ) );
            virtual ~PanelButton();

            PanelButtonOption GetOptions() const;
            void SetOptions( const PanelButtonOption option );

            // Gets the state of the toggle button.
            virtual bool GetValue() const;

            //Sets the toggle button to the given state.
            virtual void SetValue( bool value );

        protected:
            void OnMouseEnter( wxMouseEvent& event );
            void OnMouseLeave( wxMouseEvent& event );
            void OnRightMouseDown( wxMouseEvent& event );
            void OnLeftMouseDown( wxMouseEvent& event );
            void OnMouseTimer( wxTimerEvent& event );

            bool SendClickOrToggleEvent();

            virtual void AddChild( wxWindowBase *child ) HELIUM_OVERRIDE;
            virtual void RemoveChild( wxWindowBase *child ) HELIUM_OVERRIDE;

            virtual void Command( wxCommandEvent& event );

        public:
            // wxWidgets setup
            DECLARE_DYNAMIC_CLASS( PanelButton )

        private:
            wxTimer m_MouseTimer;
            PanelButtonOption m_Options;
            bool m_Toggled;
        };
    }
}