#pragma once

#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {
        class CheckBoxWidget;

        class CheckBoxWindow : public wxCheckBox
        {
        public:
            CheckBoxWindow( wxWindow* parent, CheckBoxWidget* checkBoxWidget );

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            void SetUndetermined()
            {
                Set3StateValue( wxCHK_UNDETERMINED );
            }

            void OnChecked( wxCommandEvent& );

        private:
            CheckBoxWidget* m_CheckBoxWidget;
            bool            m_Override;
        };

        class CheckBoxWidget : public Widget
        {
        public:
            HELIUM_DECLARE_CLASS( CheckBoxWidget, Widget );

            CheckBoxWidget()
                : m_CheckBoxControl( NULL )
                , m_CheckBoxWindow( NULL )
            {

            }

            CheckBoxWidget( Inspect::CheckBox* control );

            virtual void CreateWindow( wxWindow* parent ) override;
            virtual void DestroyWindow() override;

            // Inspect::Widget
            virtual void Read() override;
            virtual bool Write() override;

            // Editor::Widget
            virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args ) override;
            
            // Listeners
            void HighlightChanged( const Attribute< bool >::ChangeArgs& args );

        protected:
            Inspect::CheckBox*  m_CheckBoxControl;
            CheckBoxWindow*     m_CheckBoxWindow;
        };
    }
}