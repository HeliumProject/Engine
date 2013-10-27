#pragma once

#include "EditorScene/SceneManager.h"

#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/HierarchyOutliner.h"
#include "Editor/TreeMonitor.h"

namespace Helium
{
    namespace Editor
    {
        class DirectoryPanel : public DirectoryPanelGenerated
        {
        public:
            DirectoryPanel( Editor::SceneManager* manager, TreeMonitor* treeMonitor, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~DirectoryPanel();

            void SaveState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );
            void RestoreState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );

        private:
            Editor::SceneManager*	m_SceneManager;
            TreeMonitor*				m_TreeMonitor;
            HierarchyOutliner*			m_HierarchyOutline;
        };
    }
}