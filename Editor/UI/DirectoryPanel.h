#pragma once

#include "Editor/UI/EditorGenerated.h"

#include "Editor/Scene/SceneManager.h"
#include "Editor/UI/EntityAssetOutliner.h"
#include "Editor/UI/HierarchyOutliner.h"
#include "Editor/UI/NodeTypeOutliner.h"
#include "Editor/UI/TreeMonitor.h"

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