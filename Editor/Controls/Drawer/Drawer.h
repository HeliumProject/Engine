#pragma once

#include "Platform/Types.h"

#include "ApplicationUI/ArtProvider.h"
#include "Editor/Controls/PanelButton.h"
#include "Foundation/Event.h"

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

            const uint32_t Default = ( ClickToOpen  );
        }
        typedef uint32_t DrawerButtonStyle;

        /////////////////////////////////////////////////////////////////////////////
        class Drawer : public wxPanel
        {
        public:
            Drawer( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
            virtual ~Drawer();

            virtual void SetLabel( const wxString& label ) HELIUM_OVERRIDE;
            void SetIcon( const std::string& icon );

            PanelButton* GetButton();
            int32_t GetButtonID() const;

            wxPanel* GetPanel();
            void SetPanel( wxPanel* panel );

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
            void OnButtonClicked( wxCommandEvent& args );
            void OnMouseEnterButton( wxMouseEvent& args );
            void OnMouseLeaveButton( wxMouseEvent& args );

            void OnMouseHoverTimer( wxTimerEvent& args );
            void OnMouseLocationTimer( wxTimerEvent& args );

            bool HasMouseFocus();
            void DestroyWindow();

        private:
            wxWindow* m_Parent;

            wxString m_Icon;

            PanelButton* m_Button;
            wxStaticText* m_ButtonText;
            wxStaticBitmap* m_ButtonBitmap;

            DrawerButtonStyle m_ButtonStyle;
            wxTimer m_MouseHoverTimer;
            wxTimer m_MouseLocationTimer;

            wxPanel* m_Panel;
            wxPoint m_FloatingPosition;
            
            // Optional AuiManager
            wxAuiManager* m_AuiManager;
            wxAuiPaneInfo* m_PaneInfo;

            wxWindow* m_CurrentFrame;
        };
    }
}
