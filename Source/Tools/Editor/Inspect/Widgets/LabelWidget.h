#pragma once

#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {
        class LabelWidget;

        class LabelWindow : public wxStaticText
        {
        public:
            LabelWindow(wxWindow* parent, LabelWidget* labelWidget, bool ellipsize);

        private:
            LabelWidget*    m_LabelWidget;
        };

        class LabelWidget : public Widget
        {
        public:
            HELIUM_DECLARE_CLASS( LabelWidget, Widget );

            LabelWidget()
                : m_LabelControl( NULL )
                , m_LabelWindow( NULL )
            {

            }

            LabelWidget( Inspect::Label* label );

            virtual void CreateWindow( wxWindow* parent ) override;
            virtual void DestroyWindow() override;

            // Inspect::Widget
            virtual void Read() override;
            virtual bool Write() override;

            // Editor::Widget
            virtual void HelpTextChanged( const Attribute<std::string>::ChangeArgs& args ) override;

        protected:
            Inspect::Label*     m_LabelControl;
            LabelWindow*        m_LabelWindow;
        };
    }
}