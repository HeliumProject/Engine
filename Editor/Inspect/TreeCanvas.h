#pragma once

#include "Editor/Inspect/Canvas.h"
#include "Editor/Inspect/Widget.h"
#include "Editor/Controls/Tree/TreeWndCtrl.h"

namespace Helium
{
    namespace Editor
    {
        class TreeCanvas : public Canvas
        {
        public:
            // this is where tree-specific wx code happens
            TreeCanvas( TreeWndCtrl* treeWndCtrl );

            TreeWndCtrl* GetControl()
            {
                return m_TreeWndCtrl;
            }

            void OnSize(wxSizeEvent&);
            void OnToggle(wxTreeEvent&);

            virtual void Realize( Inspect::Canvas* canvas) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

        private:
            TreeWndCtrl*    m_TreeWndCtrl;
            wxTreeItemId    m_RootId;
        };

        typedef Helium::SmartPtr< TreeCanvas > TreeCanvasPtr;
    }
}