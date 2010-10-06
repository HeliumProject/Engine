#pragma once

#include "Foundation/Inspect/Container.h"

#include "Editor/Controls/Tree/TreeWndCtrl.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvasWidget;

        class StripCanvasWidget : public Widget
        {
        public:
            REFLECT_DECLARE_CLASS( StripCanvasWidget, Widget );

            StripCanvasWidget()
                : m_ContainerControl( NULL )
                , m_ContainerWindow( NULL )
                , m_StaticText( NULL )
            {

            }

            StripCanvasWidget( Inspect::Container* container );

            void SetPanel( wxPanel* panel )
            {
                SetWindow( m_ContainerWindow = panel );
            }

            virtual void Create( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void Destroy() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE {}
            virtual bool Write() HELIUM_OVERRIDE { return true; }

            virtual void NameChanged( const Attribute<tstring>::ChangeArgs& text );

        private:
            Inspect::Container*     m_ContainerControl;
            wxPanel*                m_ContainerWindow;
            wxStaticText*           m_StaticText;
        };
    }
}