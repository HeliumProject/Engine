#pragma once

#include "Application/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Editor
    {
        class InspectWindow : public wxClientData
        {
        public:
            InspectWindow( Inspect::Control* control, wxWindow* window );

            void Unrealized( Inspect::Control* control );

            void IsEnabledChanged( const Attribute<bool>::ChangeArgs& args );
            void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args );
            void IsFrozenChanged( const Attribute<bool>::ChangeArgs& args );
            void IsHiddenChanged( const Attribute<bool>::ChangeArgs& args );
            void ForegroundColorChanged( const Attribute<u32>::ChangeArgs& args );
            void BackgroundColorChanged( const Attribute<u32>::ChangeArgs& args );
            void ToolTipChanged( const Attribute<tstring>::ChangeArgs& args );

        protected:
            Inspect::Control*   m_Control;
            wxWindow*           m_Window;
        };

        class InspectContainer : public InspectWindow
        {

        };
    }
}