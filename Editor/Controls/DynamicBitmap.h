#pragma once

#include "Platform/Types.h"
#include <wx/bitmap.h>
#include <wx/button.h>

namespace Helium
{
    namespace Editor
    {
        class DynamicBitmap: public wxStaticBitmap
        {        
        public:
            DynamicBitmap();

            DynamicBitmap(
                wxWindow *parent,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0 );

            DynamicBitmap(
                wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBitmapNameStr );

            virtual ~DynamicBitmap();

            // returns true if window was enabled/disabled, false if nothing done
            virtual bool Enable( bool enable = true ) HELIUM_OVERRIDE;

            void SetState( wxButtonBase::State state );
            wxButtonBase::State GetState() const;

            // Methods for setting individual images for different states: normal,
            // selected (meaning pushed or pressed), focused (meaning normal state for
            // a focused button), disabled or hover (a.k.a. hot or current).
            //
            // Remember that SetBitmap() itself must be called before any other
            // SetBitmapXXX() methods (except for SetBitmapLabel() state is a synonym
            // for it anyhow) and that all bitmaps passed to these functions should be
            // of the same size.
            virtual void SetBitmap( const wxBitmap& bitmap ) HELIUM_OVERRIDE;
            //virtual void SetIcon( const wxIcon& icon ) HELIUM_OVERRIDE;

            void SetBitmap( const wxBitmap& bitmap, wxButtonBase::State state );
            
            void SetBitmapNormal( const wxBitmap& bitmap ) { SetBitmap( bitmap, wxButtonBase::State_Normal ); }
            void SetBitmapPressed( const wxBitmap& bitmap ) { SetBitmap( bitmap, wxButtonBase::State_Pressed ); }
            void SetBitmapDisabled( const wxBitmap& bitmap ) { SetBitmap( bitmap, wxButtonBase::State_Disabled ); }
            void SetBitmapCurrent( const wxBitmap& bitmap ) { SetBitmap( bitmap, wxButtonBase::State_Current ); }
            void SetBitmapFocus( const wxBitmap& bitmap ) { SetBitmap( bitmap, wxButtonBase::State_Focused ); }

            wxBitmap GetBitmap( wxButtonBase::State state = wxButtonBase::State_Normal ) const;

            wxBitmap GetBitmapNormal() const { return GetBitmap(wxButtonBase::State_Normal); }
            wxBitmap GetBitmapPressed() const { return GetBitmap(wxButtonBase::State_Pressed); }
            wxBitmap GetBitmapDisabled() const { return GetBitmap(wxButtonBase::State_Disabled); }
            wxBitmap GetBitmapCurrent() const { return GetBitmap(wxButtonBase::State_Current); }
            wxBitmap GetBitmapFocus() const { return GetBitmap(wxButtonBase::State_Focused); }
        

            // OTHER FUNCTIONS WE MIGHT NEED TO OVERRIDE:
            
            //bool wxButton::MSWOnDraw(WXDRAWITEMSTRUCT *wxdis);
            //wxSize wxButton::DoGetBestSize() const;
            //WXLRESULT wxButton::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)

        public:
            // wxWidgets setup
            DECLARE_DYNAMIC_CLASS( Button )

        protected:
            void Initialize();
            void Cleanup();

            void RefreshBitmapFromState();
            void DoSetBitmap( const wxBitmap& bitmap, wxButtonBase::State state );

        private:
            wxButtonBase::State m_CurrentState;
            wxBitmap* m_Bitmaps[wxButtonBase::State_Max];
            bool m_WasStateSetByUser[wxButtonBase::State_Max];
        };
    }
}
