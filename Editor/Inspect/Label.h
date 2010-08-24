#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class LabelPanel;

        class LabelWidget : public Widget
        {
        public:
            LabelWidget( Inspect::Control* control );

            void Create( wxWindow* parent );

            virtual void ToolTipChanged( const Attribute<tstring>::ChangeArgs& args ) HELIUM_OVERRIDE;

            void TextChanged( const Attribute<tstring>::ChangeArgs& args );

        protected:
            Inspect::Label*     m_LabelControl;
            LabelPanel*         m_LabelWindow;
        };
    }
}