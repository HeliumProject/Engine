#pragma once

#include "API.h"
#include "SceneOutliner.h"
#include "Core/Object.h"

namespace Luna
{
  // Forwards
  class Instance;
  class InstanceType;
  class InstanceCodeSet;
  struct NodeChangeArgs;
  struct NodeTypeExistenceArgs;
  struct InstanceSetChangeArgs;
  struct InstanceTypeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Provides a tree view of all the Runtime Classes (aka code sets) of all the 
  // Instance Types in the scene.
  // 
  class RuntimeClassOutliner : public SceneOutliner
  {
    // Member variables
  private:
    // Needed to simulate multiple root items in the tree
    wxTreeItemId m_InvisibleRoot;

    // Public functions
  public:
    RuntimeClassOutliner( Luna::SceneManager* sceneManager );
    virtual ~RuntimeClassOutliner();
    void AddInstanceTypes();

    // Helpers
  private:
    void AddInstanceType( Luna::InstanceType* entityType );
    void RemoveInstanceType( Luna::InstanceType* entityType );
    void AddRuntimeClassSet( Luna::InstanceCodeSet* classSet );
    void RemoveRuntimeClassSet( Luna::InstanceCodeSet* classSet );
    void AddInstance( Luna::Instance* entity );
    void RemoveInstance( Luna::Instance* entity );

    // Overrides from SceneOutliner
    UIToolKit::SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;
    virtual void CurrentSceneChanged( Luna::Scene* oldScene ) NOC_OVERRIDE;
    virtual void ConnectSceneListeners() NOC_OVERRIDE;
    virtual void DisconnectSceneListeners() NOC_OVERRIDE;

    // Application callbacks
  private:
    void SetAdded( const InstanceTypeChangeArgs& args );
    void SetRemoved( const InstanceTypeChangeArgs& args );
    void InstanceAdded( const InstanceSetChangeArgs& args );
    void InstanceRemoved( const InstanceSetChangeArgs& args );
    void NodeTypeAdded( const NodeTypeExistenceArgs& args );
    void NodeTypeRemoved( const NodeTypeExistenceArgs& args );
    void NodeAdded( const NodeChangeArgs& args );
    void NodeRemoved( const NodeChangeArgs& args );

  private:
    // GUI callbacks
    void OnBeginLabelEdit( wxTreeEvent& args );
  };
}
