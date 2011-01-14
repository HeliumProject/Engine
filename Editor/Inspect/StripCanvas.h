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
            REFLECT_DECLARE_OBJECT( StripCanvas, Canvas );

            // this is where tree-specific wx code happens
            StripCanvas();

            wxPanel* GetPanel()
            {
                return m_Panel;
            }
            void SetPanel( wxPanel* panel );

            virtual void Realize( Inspect::Canvas* canvas ) HELIUM_OVERRIDE;

        private:
            wxPanel*    m_Panel;
        };

        typedef Helium::SmartPtr< StripCanvas > StripCanvasPtr;
    }
}