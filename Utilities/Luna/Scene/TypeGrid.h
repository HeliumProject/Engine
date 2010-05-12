#pragma once

#include <map>

namespace Luna
{
  // Forwards
  class Scene;
  class SceneManager;
  class GridWithIcons;
  class HierarchyNodeType;
  struct GridRowChangeArgs;
  struct SceneChangeArgs;
  struct NodeTypeExistenceArgs;

  // Typedefs
  typedef std::map< std::string, Luna::HierarchyNodeType* > M_HierarchyNodeTypeDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Provides a UI for displaying the different Hierarchy Node Types in a scene
  // and changing the selectability/visibility of all instances of each node
  // type.
  // 
  class TypeGrid
  {
  private:
    Luna::SceneManager* m_SceneManager;
    wxPanel* m_Panel;
    GridWithIcons* m_Grid;
    Luna::Scene* m_Scene;
    M_HierarchyNodeTypeDumbPtr m_NodeTypes;

  public:
    TypeGrid( wxWindow* parent, Luna::SceneManager* sceneManager );
    virtual ~TypeGrid();
    wxPanel* GetPanel() const;
    
    bool AddType( Luna::HierarchyNodeType* type );
    bool RemoveType( Luna::HierarchyNodeType* type );

  private:
    void AddNodeType( const NodeTypeExistenceArgs& args );
    void RemoveNodeType( const NodeTypeExistenceArgs& args );

  private:
    void VisibilityChanged( const GridRowChangeArgs& args );
    void SelectabilityChanged( const GridRowChangeArgs& args );
    void CurrentSceneChanging( const SceneChangeArgs& args );
    void CurrentSceneChanged( const SceneChangeArgs& args );
  };
}
