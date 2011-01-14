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
            REFLECT_DECLARE_OBJECT( TreeCanvas, Canvas );

            TreeCanvas();
            ~TreeCanvas();

            TreeWndCtrl* GetTreeWndCtrl()
            {
                return m_TreeWndCtrl;
            }
            void SetTreeWndCtrl( TreeWndCtrl* ctrl );

            bool IsCollapsed( const tstring& path )
            {
                return m_Collapsed.find( path ) != m_Collapsed.end();
            }

            virtual void Realize( Inspect::Canvas* canvas) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

        private:
            void OnSize(wxSizeEvent&);
            void OnToggle(wxTreeEvent&);

            TreeWndCtrl*        m_TreeWndCtrl;
            wxTreeItemId        m_RootId;
            std::set< tstring > m_Collapsed;
        };

        typedef Helium::StrongPtr< TreeCanvas > TreeCanvasPtr;
    }
}