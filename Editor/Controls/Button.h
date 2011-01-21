#pragma once

#include "Platform/Types.h"
#include <wx/button.h>
#include <wx/tglbtn.h>

namespace Helium
{
    namespace Editor
    {
        namespace ButtonOptions
        {
            enum ButtonOption
            {
                Toggle = 1 << 0,
            };

            const uint32_t Default = 0;
        }
        typedef uint32_t ButtonOption;

        class Button: public wxButton
        {        
        public:
            Button();
            Button( wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr );
            virtual ~Button();

            ButtonOption GetButtonOptions() const;
            void SetButtonOptions( const ButtonOption option );

            // Gets the state of the toggle button.
            virtual bool GetValue() const;

            //Sets the toggle button to the given state.
            virtual void SetValue( bool value );

            //const wxBitmap& GetBitmapToggled() const;
            //void SetBitmapToggled( const wxBitmap& bitmap );

        protected:
            // Could we just override this instead:
            bool SendClickOrToggleEvent();
            virtual void Command( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual bool MSWCommand( WXUINT param, WXWORD id ) HELIUM_OVERRIDE;
            //void OnLeftClick( wxCommandEvent& event );
            //void OnToggle( wxCommandEvent& event );

            virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = NULL) const;

        public:
            // wxWidgets setup
            DECLARE_DYNAMIC_CLASS( Button )

        private:
            ButtonOption m_ButtonOption;
        };
    }
}