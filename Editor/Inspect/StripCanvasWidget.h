#pragma once

#include "Inspect/Container.h"

#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvasWidget;

        class StripCanvasWidget : public Widget
        {
        public:
            REFLECT_DECLARE_OBJECT( StripCanvasWidget, Widget );

            StripCanvasWidget( Inspect::Container* container  = NULL, int orientation = wxHORIZONTAL );

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
            int32_t m_Orientation;
            wxPanel* m_ContainerWindow;
            wxStaticText* m_StaticText;
        };
    }
}