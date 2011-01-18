#pragma once

#include "Editor/Controls/Drawer/DrawerManager.h"
#include "Editor/Inspect/Canvas.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class StripCanvas : public Canvas
        {
        public:
            REFLECT_DECLARE_OBJECT( StripCanvas, Canvas );

            StripCanvas( int orientation = wxHORIZONTAL );

            wxPanel* GetPanel() const;
            void SetPanel( wxPanel* panel );

            virtual void Realize( Inspect::Canvas* canvas ) HELIUM_OVERRIDE;

        private:
            wxPanel* m_Panel;
            int32_t m_Orientation;
        };

        typedef Helium::SmartPtr< StripCanvas > StripCanvasPtr;
    }
}