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
            TypesPanel( Core::SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~TypesPanel();

            bool AddType( Core::HierarchyNodeType* type );
            bool RemoveType( Core::HierarchyNodeType* type );

        private:
            void AddNodeType( const Core::NodeTypeExistenceArgs& args );
            void RemoveNodeType( const Core::NodeTypeExistenceArgs& args );

        private:
            void VisibilityChanged( const GridRowChangeArgs& args );
            void SelectabilityChanged( const GridRowChangeArgs& args );
            void CurrentSceneChanging( const Core::SceneChangeArgs& args );
            void CurrentSceneChanged( const Core::SceneChangeArgs& args );

        private:
            Core::SceneManager* m_SceneManager;
            Core::Scene* m_Scene;
            Core::M_HierarchyNodeTypeDumbPtr m_NodeTypes;
            GridWithIcons* m_Grid;
        };
    }
}