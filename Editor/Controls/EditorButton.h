#pragma once

#include "PanelButton.h"

namespace Helium
{
    namespace Editor
    {
        class EditorButton : public PanelButton
        {        
        public:
            EditorButton();
            EditorButton( wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBORDER_THEME,
                const wxString& name = wxT( "EditorButton" ) );
            virtual ~EditorButton();

        protected:
            void OnUpdateUI( wxUpdateUIEvent& event );

        private:
            bool m_ShowText;
            bool m_ShowIcons;

        public:
            DECLARE_DYNAMIC_CLASS( EditorButton )
        };
    }
}