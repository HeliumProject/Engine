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

            void OnSize(wxSizeEvent&);
            void OnToggle(wxTreeEvent&);

        private:
            TreeWndCtrl* m_TreeWndCtrl;
        };

        typedef Helium::SmartPtr< TreeCanvas > TreeCanvasPtr;
    }
}