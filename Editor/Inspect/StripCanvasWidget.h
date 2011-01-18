#pragma once

#include "Foundation/Inspect/Container.h"

#include "Editor/Controls/Drawer/DrawerManager.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvasWidget;

        class StripCanvasWidget : public Widget
        {
        public:
            REFLECT_DECLARE_OBJECT( StripCanvasWidget, Widget );

            StripCanvasWidget()
                : m_ContainerControl( NULL )
                , m_ContainerWindow( NULL )
                , m_StaticText( NULL )
            {

            }

            StripCanvasWidget( Inspect::Container* container );

            wxPanel* GetPanel() const;
            void SetPanel( wxPanel* panel );

            virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void DestroyWindow() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE {}
            virtual bool Write() HELIUM_OVERRIDE { return true; }

            virtual void NameChanged( const Attribute<tstring>::ChangeArgs& text );
            virtual void IconChanged( const Attribute<tstring>::ChangeArgs& icon );

        private:
            Inspect::Container* m_ContainerControl;
            wxPanel* m_ContainerWindow;
            wxStaticText* m_StaticText;
        };
    }
}