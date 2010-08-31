#pragma once

#include "Foundation/Inspect/InspectControls.h"

namespace Helium
{
    namespace Editor
    {
        class Canvas : public Inspect::Canvas, public wxEvtHandler
        {
        public:
            Canvas( wxWindow* window );

            // callbacks from the window
            virtual void OnShow(wxShowEvent&);
            virtual void OnClick(wxMouseEvent&);

            // widget construction
            virtual void RealizeControl( Inspect::Control* control, Inspect::Control* parent ) HELIUM_OVERRIDE;

        private:
            wxWindow*   m_Window;
        };
    }
}