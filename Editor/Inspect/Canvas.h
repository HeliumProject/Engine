#pragma once

#include "Foundation/Inspect/Controls.h"

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        typedef Helium::SmartPtr< Widget > (*WidgetCreator)( Inspect::Control* control );

        template< class WidgetT, class ControlT >
        WidgetPtr CreateWidget( ControlT* control )
        {
            return new WidgetT( control );
        }

        typedef std::map< i32, WidgetCreator > WidgetCreators;

        class Canvas : public Inspect::Canvas, public wxEvtHandler
        {
        public:
            Canvas( wxWindow* window );

            virtual wxWindow* GetWindow()
            {
                return m_Window;
            }

            // callbacks from the window
            virtual void OnShow(wxShowEvent&);
            virtual void OnClick(wxMouseEvent&);

            // widget construction
            virtual void RealizeControl( Inspect::Control* control, Inspect::Control* parent ) HELIUM_OVERRIDE;

            template< class WidgetT, class ControlT >
            void SetWidgetCreator()
            {
                m_WidgetCreators[ Reflect::GetType< ControlT >() ] = (WidgetCreator)( &CreateWidget< WidgetT, ControlT > );
            }

        private:
            wxWindow*       m_Window;
            WidgetCreators  m_WidgetCreators;
        };
    }
}