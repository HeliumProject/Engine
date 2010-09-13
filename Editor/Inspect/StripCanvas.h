#pragma once

#include "Editor/Inspect/Canvas.h"
#include "Editor/Inspect/Widget.h"
#include "Editor/Controls/Tree/TreeWndCtrl.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvas : public Canvas
        {
        public:
            // this is where tree-specific wx code happens
            StripCanvas( wxPanel* panel );

            virtual void Realize( Inspect::Canvas* canvas ) HELIUM_OVERRIDE;

        private:
            wxPanel*    m_Panel;
        };

        typedef Helium::SmartPtr< StripCanvas > StripCanvasPtr;
    }
}