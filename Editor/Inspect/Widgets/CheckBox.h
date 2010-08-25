#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class CheckBoxWidget;

        class CheckBoxPanel : public wxPanel
        {
        public:
            CheckBoxPanel( wxWindow* parent, CheckBoxWidget* checkBoxWidget, int width = 16, int height = 22 );

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            void SetValue( bool value )
            {
                HELIUM_ASSERT( m_CheckBox );
                m_CheckBox->SetValue( value );
            }

            bool GetValue()
            {
                HELIUM_ASSERT( m_CheckBox );
                return m_CheckBox->GetValue();
            }

            void OnChecked( wxCommandEvent& );

        private:
            wxCheckBox*     m_CheckBox;
            CheckBoxWidget* m_CheckBoxWidget;
            bool            m_Override;
        };

        class CheckBoxWidget : public Widget
        {
        public:
            CheckBoxWidget( Inspect::Control* control );

            void Create( wxWindow* parent );
            void Destroy();

            // Inspect::Widget
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            // Editor::Widget
            virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args ) HELIUM_OVERRIDE;
            
            // Listeners
            void HighlightChanged( const Attribute< bool >::ChangeArgs& args );

        protected:
            Inspect::CheckBox*  m_CheckBoxControl;
            CheckBoxPanel*      m_CheckBoxPanel;
        };

    }
}