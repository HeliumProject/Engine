#pragma once

#include "Inspect/Container.h"
#include "Editor/Controls/Drawer/Drawer.h"
#include "Editor/Inspect/StripCanvas.h"
#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {

        class DrawerWidget : public Widget
        {
        public:
            HELIUM_DECLARE_CLASS( DrawerWidget, Widget );

            DrawerWidget();
            DrawerWidget( Inspect::Container* container );

            virtual void CreateWindow( wxWindow* parent ) override;
            virtual void DestroyWindow() override;

            virtual void Read() override {}
            virtual bool Write() override { return true; }

            Drawer* GetDrawer() const;
            Canvas* GetCanvas() const;

            void SetLabel( const std::string& label );
            void SetIcon( const std::string& icon );

        protected:
            void OnIconChanged( const Attribute< std::string >::ChangeArgs& args );
            void OnLabelChanged( const Attribute< std::string >::ChangeArgs& args );

            Inspect::Container* m_ContainerControl;
            Drawer* m_Drawer;
            StripCanvas* m_StripCanvas;
        };

    }
}
