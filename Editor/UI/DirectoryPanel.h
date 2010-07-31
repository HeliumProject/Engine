#pragma once

#include "EditorGenerated.h"

#include "Scene/SceneManager.h"
#include "Scene/TreeMonitor.h"
#include "Scene/EntityAssetOutliner.h"
#include "Scene/HierarchyOutliner.h"
#include "Scene/NodeTypeOutliner.h"

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