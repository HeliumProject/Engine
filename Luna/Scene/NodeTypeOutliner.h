#pragma once

#include "Luna/API.h"
#include "SceneOutliner.h"
#include "SceneNodeType.h"

namespace Luna
{
  // Forwards
  class SceneNode;
  struct NodeChangeArgs;
  struct NodeTypeExistenceArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for a tree control that displays all of the scene nodes, grouped
  // by NodeType.
  // 
  class NodeTypeOutliner : public SceneOutliner
  {
    // Member variables
  private:
    wxTreeItemId m_InvisibleRoot;

    // Public functions
  public:
    NodeTypeOutliner( Luna::SceneManager* sceneManager );
    virtual ~NodeTypeOutliner();
    void AddNodeTypes();

    // Helpers
  private:
    void AddNodeType( Luna::SceneNodeType* nodeType );
    void AddInstance( Luna::SceneNode* instance );
    void RemoveNodeType( Luna::SceneNodeType* nodeType );
    void RemoveInstance( Luna::SceneNode* instance );

    // Overrides from SceneOutliner
    SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;
    virtual void CurrentSceneChanged( Luna::Scene* oldScene ) NOC_OVERRIDE;
    virtual void ConnectSceneListeners() NOC_OVERRIDE;
    virtual void DisconnectSceneListeners() NOC_OVERRIDE;

    // Application callbacks
  private:
    void NodeTypeAdded( const NodeTypeExistenceArgs& args );
    void NodeTypeRemoved( const NodeTypeExistenceArgs& args );
    void NodeAddedToType( const NodeTypeChangeArgs& args );
    void NodeRemovedFromType( const NodeTypeChangeArgs& args );

  private:
    // GUI callbacks
    void OnBeginLabelEdit( wxTreeEvent& args );
  };
}
