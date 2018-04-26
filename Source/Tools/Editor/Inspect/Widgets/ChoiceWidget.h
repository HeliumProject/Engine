#pragma once

#include "Editor/Inspect/wxWidget.h"

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
            HELIUM_DECLARE_CLASS( ChoiceWidget, Widget );

            ChoiceWidget()
                : m_ChoiceControl( NULL )
                , m_ChoiceWindow( NULL )
            {

            }

            ChoiceWidget( Inspect::Choice* choice );

            virtual void CreateWindow( wxWindow* parent ) override;
            virtual void DestroyWindow() override;

            virtual void Read() override;
            virtual bool Write() override;

            void HighlightChanged( const Attribute<bool>::ChangeArgs& args );
            void ItemsChanged( const Attribute< std::vector< Inspect::ChoiceItem > >::ChangeArgs& args );

            std::string GetValue();
            void SetValue( const std::string& value );

        private:
            Inspect::Choice*    m_ChoiceControl;
            ChoiceWindow*       m_ChoiceWindow;
        };
    }
}