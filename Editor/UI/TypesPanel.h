#pragma once

#include "EditorGenerated.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/HierarchyNodeType.h"

#include "GridWithIcons.h"

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