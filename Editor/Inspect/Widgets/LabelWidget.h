#pragma once

#include "Editor/Inspect/Widget.h"

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
            REFLECT_DECLARE_CLASS( LabelWidget, Widget );

            LabelWidget()
                : m_LabelControl( NULL )
                , m_LabelWindow( NULL )
            {

            }

            LabelWidget( Inspect::Label* label );

            virtual void Create( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void Destroy() HELIUM_OVERRIDE;

            // Inspect::Widget
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            // Editor::Widget
            virtual void HelpTextChanged( const Attribute<tstring>::ChangeArgs& args ) HELIUM_OVERRIDE;

        protected:
            Inspect::Label*     m_LabelControl;
            LabelWindow*        m_LabelWindow;
        };
    }
}