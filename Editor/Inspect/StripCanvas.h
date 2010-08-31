#pragma once

#include "Editor/Inspect/Canvas.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvas : public Editor::Canvas
        {
        public:
            StripCanvas( wxPanel* panel );

        private:
            wxPanel*    m_Panel;
        };
    }
}