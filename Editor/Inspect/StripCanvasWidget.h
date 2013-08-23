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
            HELIUM_DECLARE_CLASS( StripCanvasWidget, Widget );

            StripCanvasWidget( Inspect::Container* container  = NULL, int orientation = wxHORIZONTAL );

            wxPanel* GetPanel() const;
            void SetPanel( wxPanel* panel );

            virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void DestroyWindow() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE {}
            virtual bool Write() HELIUM_OVERRIDE { return true; }

            virtual void NameChanged( const Attribute<std::string>::ChangeArgs& text );
            virtual void IconChanged( const Attribute<std::string>::ChangeArgs& icon );

        private:
            Inspect::Container* m_ContainerControl;
            int32_t m_Orientation;
            wxPanel* m_ContainerWindow;
            wxStaticText* m_StaticText;
        };
    }
}