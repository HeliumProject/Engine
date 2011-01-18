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

            // this is where tree-specific wx code happens
            StripCanvas();

            wxPanel* GetPanel() const;
            void SetPanel( wxPanel* panel );

            DrawerManager* GetDrawerManager() const;
            void SetDrawerManager( DrawerManager* drawerManager );

            virtual void Realize( Inspect::Canvas* canvas ) HELIUM_OVERRIDE;

        private:
            wxPanel*    m_Panel;
            DrawerManager* m_DrawerManager;
        };

        typedef Helium::SmartPtr< StripCanvas > StripCanvasPtr;
    }
}