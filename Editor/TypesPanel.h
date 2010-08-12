#pragma once

#include "Editor/EditorGenerated.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/HierarchyNodeType.h"

#include "GridWithIcons.h"

namespace Helium
{
    namespace Editor
    {
        class TypesPanel : public TypesPanelGenerated
        {
        public:
            TypesPanel( SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~TypesPanel();

            bool AddType( Editor::HierarchyNodeType* type );
            bool RemoveType( Editor::HierarchyNodeType* type );

        private:
            void AddNodeType( const NodeTypeExistenceArgs& args );
            void RemoveNodeType( const NodeTypeExistenceArgs& args );

        private:
            void VisibilityChanged( const GridRowChangeArgs& args );
            void SelectabilityChanged( const GridRowChangeArgs& args );
            void CurrentSceneChanging( const SceneChangeArgs& args );
            void CurrentSceneChanged( const SceneChangeArgs& args );

        private:
            SceneManager* m_SceneManager;
            GridWithIcons* m_Grid;
            Scene* m_Scene;
            M_HierarchyNodeTypeDumbPtr m_NodeTypes;
        };
    }
}