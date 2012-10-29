#pragma once

#include "Inspect/Canvas.h"

#include "Editor/Inspect/wxWidget.h"

#include "Editor/Controls/Drawer/DrawerManager.h"

namespace Helium
{
    namespace Editor
    {
        template< class WidgetT, class ControlT >
        WidgetPtr CreateWidget( ControlT* control )
        {
            return new WidgetT( control );
        }

        typedef Helium::StrongPtr< Widget >    (*WidgetCreator)( Inspect::Control* control );
        typedef std::map< const Reflect::Class*, WidgetCreator >  WidgetCreators;

        class Canvas : public Inspect::Canvas, public wxEvtHandler
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Canvas, Inspect::Canvas );

            Canvas();
            ~Canvas();

            wxWindow* GetWindow()
            {
                return m_Window;
            }
            void SetWindow( wxWindow* window );

            DrawerManager* GetDrawerManager() const;
            void SetDrawerManager( DrawerManager* drawerManager );

            // callbacks from the window
            virtual void OnShow(wxShowEvent&);
            virtual void OnClick(wxMouseEvent&);

            // widget construction and teardown
            virtual void RealizeControl( Inspect::Control* control ) HELIUM_OVERRIDE;
            virtual void UnrealizeControl( Inspect::Control* control ) HELIUM_OVERRIDE;

            // associate a widget to a control
            template< class WidgetT, class ControlT >
            void SetWidgetCreator()
            {
                m_WidgetCreators[ Reflect::GetClass< ControlT >() ] = (WidgetCreator)( &CreateWidget< WidgetT, ControlT > );
            }

        private:
            wxWindow* m_Window;
            WidgetCreators m_WidgetCreators;
            DrawerManager* m_DrawerManager;
        };
    }
}