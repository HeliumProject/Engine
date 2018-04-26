#pragma once

#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {
        class ValueWidget;

        class ValueWindow : public wxTextCtrl
        {
        public:
            ValueWindow( wxWindow* parent, ValueWidget* valueWidget );

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            void OnConfirm(wxCommandEvent& e);
            void OnSetFocus(wxFocusEvent& e);
            void OnKillFocus(wxFocusEvent& e);
            void OnKeyDown(wxKeyEvent& e);

            DECLARE_EVENT_TABLE();

        private:
            ValueWidget*    m_ValueWidget;
            bool            m_Override;
        };

        class ValueWidget : public Widget
        {
        public:
            HELIUM_DECLARE_CLASS( ValueWidget, Widget );

            ValueWidget()
                : m_ValueControl( NULL )
                , m_ValueWindow( NULL )
            {

            }

            ValueWidget( Inspect::Value* value );

            virtual void CreateWindow( wxWindow* parent ) override;
            virtual void DestroyWindow() override;

            // Inspect::Widget
            virtual void Read() override;
            virtual bool Write() override;

            // Editor::Widget
            virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args ) override;

            // Listeners
            void JustificationChanged( const Attribute<Inspect::Justification>::ChangeArgs& args );
            void HighlightChanged( const Attribute<bool>::ChangeArgs& args );

        protected:
            Inspect::Value*     m_ValueControl;
            ValueWindow*        m_ValueWindow;
        };
    }
}