#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class CheckBoxWidget;

        class CheckBoxWindow : public wxPanel
        {
        public:
            CheckBoxWindow( wxWindow* parent, CheckBoxWidget* checkBoxWidget );

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            void SetValue( bool value )
            {
                HELIUM_ASSERT( m_CheckBox );
                m_CheckBox->SetValue( value );
            }

            void SetUndetermined()
            {
                HELIUM_ASSERT( m_CheckBox );
                m_CheckBox->Set3StateValue( wxCHK_UNDETERMINED );
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

        class CheckBoxWidget : public Reflect::ConcreteInheritor< CheckBoxWidget, Widget >
        {
        public:
            CheckBoxWidget()
                : m_CheckBoxControl( NULL )
                , m_CheckBoxWindow( NULL )
            {

            }

            CheckBoxWidget( Inspect::CheckBox* control );

            virtual void Create( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void Destroy() HELIUM_OVERRIDE;

            // Inspect::Widget
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            // Editor::Widget
            virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args ) HELIUM_OVERRIDE;
            
            // Listeners
            void HighlightChanged( const Attribute< bool >::ChangeArgs& args );

        protected:
            Inspect::CheckBox*  m_CheckBoxControl;
            CheckBoxWindow*     m_CheckBoxWindow;
        };
    }
}