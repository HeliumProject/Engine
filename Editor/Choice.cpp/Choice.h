#pragma once

#include "Editor/Inspect/Widget.h"

#include <wx/textctrl.h>
#include <wx/combobox.h>

namespace Helium
{
    namespace Editor
    {
        class ChoiceWidget;

        class ChoiceWindow : public wxComboBox
        {
        public:
            ChoiceWindow(wxWindow* parent, ChoiceWidget* choiceWidget, int flags);

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            void OnConfirm(wxCommandEvent& event);
            void OnTextEnter(wxCommandEvent& event);
            void OnSetFocus(wxFocusEvent& event);
            void OnKillFocus(wxFocusEvent& event);

            DECLARE_EVENT_TABLE();

        public:
            ChoiceWidget*   m_ChoiceWidget;
            bool            m_Override;
        };

        class ChoiceWidget : public Widget
        {
        public:
            ChoiceWidget( Inspect::Choice* choice );

            void Create( wxWindow* parent );
            void Destroy();

            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            void HighlightChanged( const Attribute<bool>::ChangeArgs& args );
            void ItemsChanged( const Attribute<Inspect::V_ChoiceItem>::ChangeArgs& args );

            tstring GetValue();
            void SetValue( const tstring& value );

        private:
            Inspect::Choice*    m_ChoiceControl;
            ChoiceWindow*       m_ChoiceWindow;
        };
    }
}