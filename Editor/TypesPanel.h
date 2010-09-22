#pragma once

#include "Editor/EditorGenerated.h"

#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/SceneManager.h"
#include "Core/SceneGraph/HierarchyNodeType.h"

#include "GridWithIcons.h"

namespace Helium
{
    namespace Editor
    {
        class TypesPanel : public TypesPanelGenerated
        {
        public:
            TypesPanel( SceneGraph::SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~TypesPanel();

            bool AddType( SceneGraph::HierarchyNodeType* type );
            bool RemoveType( SceneGraph::HierarchyNodeType* type );

        private:
            void AddNodeType( const SceneGraph::NodeTypeExistenceArgs& args );
            void RemoveNodeType( const SceneGraph::NodeTypeExistenceArgs& args );

        private:
            void VisibilityChanged( const GridRowChangeArgs& args );
            void SelectabilityChanged( const GridRowChangeArgs& args );
            void CurrentSceneChanging( const SceneGraph::SceneChangeArgs& args );
            void CurrentSceneChanged( const SceneGraph::SceneChangeArgs& args );

        private:
            SceneGraph::SceneManager* m_SceneManager;
            SceneGraph::Scene* m_Scene;
            SceneGraph::M_HierarchyNodeTypeDumbPtr m_NodeTypes;
            GridWithIcons* m_Grid;
        };
    }
}