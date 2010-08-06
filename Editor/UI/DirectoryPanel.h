#pragma once

#include "EditorGenerated.h"

#include "Scene/SceneManager.h"
#include "UI/EntityAssetOutliner.h"
#include "UI/HierarchyOutliner.h"
#include "UI/NodeTypeOutliner.h"
#include "UI/TreeMonitor.h"

namespace Helium
{
    namespace Editor
    {
        class DirectoryPanel : public DirectoryPanelGenerated
        {
        public:
            DirectoryPanel( SceneManager* manager, TreeMonitor* treeMonitor, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~DirectoryPanel();

            void SaveState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );
            void RestoreState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );

        private:
            SceneManager*        m_SceneManager;
            TreeMonitor*         m_TreeMonitor;
            EntityAssetOutliner* m_EntityOutline;
            HierarchyOutliner*   m_HierarchyOutline;
            NodeTypeOutliner*    m_TypeOutline;
        };
    }
}