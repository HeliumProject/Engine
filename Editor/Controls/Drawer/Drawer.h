#pragma once

#include "Platform/Types.h"

#include "Foundation/Automation/Event.h"

#include <wx/tglbtn.h>

namespace Helium
{
    namespace Editor
    {
        class Drawer;

        /////////////////////////////////////////////////////////////////////////////
        class DrawerEventArgs
        {
        public:
            Drawer* m_Drawer;

            DrawerEventArgs( Drawer* drawer )
                : m_Drawer( drawer )
            {
            }
        };
        typedef Helium::Signature< const DrawerEventArgs& > DrawerEventSignature;

        /////////////////////////////////////////////////////////////////////////////
        namespace DrawerButtonStyles
        {
            enum Style
            {
                ClickToOpen         = 1 << 0,          // Drawers open when the button is clicked/toggled
                MouseOverToOpen     = 1 << 1,          // Drawers open when the button is moused over
            };

            const uint32_t Default = ClickToOpen | MouseOverToOpen;
        }
        typedef uint32_t DrawerButtonStyle;

        /////////////////////////////////////////////////////////////////////////////
        class Drawer : public wxEvtHandler
        {
        public:
            Drawer( wxWindow* parent, wxPanel* panel, const wxString& title, const wxBitmap& icon, const DrawerButtonStyle style = DrawerButtonStyles::Default );
            virtual ~Drawer();

            const wxString& GetTitle() const;
            const wxBitmap& GetIcon() const;

            wxToggleButton* GetButton();
            int32_t GetButtonID() const;

            void SetAuiManager( wxAuiManager* auiManager );

            void Open();
            void Close();
            bool IsOpen() const;

        public:
            mutable DrawerEventSignature::Event e_Opening;
            mutable DrawerEventSignature::Event e_Opened;
            mutable DrawerEventSignature::Event e_Closing;
            mutable DrawerEventSignature::Event e_Closed;

        protected:
            void OnMouseLeaveDrawer( wxMouseEvent& args );

            void OnButtonClicked( wxCommandEvent& args );
            void OnMouseEnterButton( wxMouseEvent& args );
            void OnMouseLeaveButton( wxMouseEvent& args );

            void OnMouseHoverTimer( wxTimerEvent& args );

            bool HasMouseFocus();
            void DestroyWindow();

        private:
            wxWindow* m_Parent;
            wxPanel* m_Panel;
            wxString m_Title;
            wxBitmap m_Icon;
            DrawerButtonStyle m_Style;

            wxToggleButton* m_Button;
            wxTimer m_MouseHoverTimer;

            wxPoint m_FloatingPosition;
            
            wxAuiManager* m_AuiManager;
            wxAuiPaneInfo* m_PaneInfo;

            wxWindow* m_DrawerWindow;
        };
    }
}
