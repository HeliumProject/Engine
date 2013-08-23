#pragma once

#include "Editor/Inspect/wxCanvas.h"
#include "Editor/Inspect/wxWidget.h"
#include "Editor/Controls/Tree/TreeWndCtrl.h"

namespace Helium
{
    namespace Editor
    {
        class TreeCanvas : public Canvas
        {
        public:
            HELIUM_DECLARE_CLASS( TreeCanvas, Canvas );

            TreeCanvas();
            ~TreeCanvas();

            TreeWndCtrl* GetTreeWndCtrl()
            {
                return m_TreeWndCtrl;
            }
            void SetTreeWndCtrl( TreeWndCtrl* ctrl );

            bool IsCollapsed( const std::string& path )
            {
                return m_Collapsed.find( path ) != m_Collapsed.end();
            }

            virtual void Realize( Inspect::Canvas* canvas) HELIUM_OVERRIDE;
            virtual void Clear();

        private:
            void OnSize(wxSizeEvent&);
            void OnToggle(wxTreeEvent&);

            TreeWndCtrl*        m_TreeWndCtrl;
            wxTreeItemId        m_RootId;
            std::set< std::string > m_Collapsed;
        };

        typedef Helium::StrongPtr< TreeCanvas > TreeCanvasPtr;
    }
}