#pragma once

#include "Inspect/Controls.h"

namespace Helium
{
    namespace Editor
    {
        class Widget : public Inspect::Widget, public wxEvtHandler
        {
        public:
            HELIUM_DECLARE_ABSTRACT( Widget, Inspect::Widget );

            Widget()
                : m_Window( NULL )
            {

            }

            Widget( Inspect::Control* control );
            ~Widget();

            virtual wxWindow* GetWindow()
            {
                return m_Window;
            }

            int GetStringWidth(const std::string& str);
            bool EllipsizeString(std::string& str, int width);

            virtual void CreateWindow( wxWindow* parent ) = 0;
            virtual void DestroyWindow() = 0;

            // initialize state
            virtual void SetWindow( wxWindow* window );

            // callbacks from m_Control
            virtual void IsEnabledChanged( const Attribute<bool>::ChangeArgs& args );
            virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args );
            virtual void IsFrozenChanged( const Attribute<bool>::ChangeArgs& args );
            virtual void IsHiddenChanged( const Attribute<bool>::ChangeArgs& args );
            virtual void ForegroundColorChanged( const Attribute<uint32_t>::ChangeArgs& args );
            virtual void BackgroundColorChanged( const Attribute<uint32_t>::ChangeArgs& args );
            virtual void HelpTextChanged( const Attribute<std::string>::ChangeArgs& args );

            void OnContextMenu( wxContextMenuEvent& event );
            void OnContextMenuItem( wxCommandEvent& event );

        protected:
            wxWindow*           m_Window;
        };

        typedef StrongPtr<Widget> WidgetPtr;
    }
}