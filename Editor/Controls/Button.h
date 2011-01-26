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
                HideLabel = 1 << 0,       // Sets WX styles: wxBU_NOTEXT
                Toggle = 1 << 1,          // Sets MSW styles: BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP
                NoAutoDraw = 1 << 2,      // Sets MSW styles: wxBU_AUTODRAW
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

        protected:
            // Could we just override this instead:
            bool SendClickOrToggleEvent();
            virtual void Command( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual bool MSWCommand( WXUINT param, WXWORD id ) HELIUM_OVERRIDE;

            virtual WXDWORD MSWGetStyle( long flags, WXDWORD *exstyle = NULL ) const HELIUM_OVERRIDE;

            virtual void DoSetBitmap( const wxBitmap& bitmap, State which ) HELIUM_OVERRIDE;

        public:
            // wxWidgets setup
            DECLARE_DYNAMIC_CLASS( Button )

        private:
            ButtonOption m_ButtonOption;
            bool m_IsStateBitmapSetByUser[wxButton::State_Max];
        };
    }
}