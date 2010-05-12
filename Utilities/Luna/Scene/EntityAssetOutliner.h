#pragma once

#include "API.h"
#include "SceneOutliner.h"
#include "Core/Object.h"

namespace Luna
{
  // Forwards
  class Entity;
  class EntityType;
  class EntityAssetSet;
  struct NodeTypeExistenceArgs;
  struct InstanceSetChangeArgs;
  struct InstanceTypeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Provides a tree representation of all the entity classes in the scene, 
  // grouped according to their entity class set.
  // 
  class EntityAssetOutliner : public SceneOutliner
  {
    // Member variables
  private:
    // Needed to simulate multiple root items in the tree
    wxTreeItemId m_InvisibleRoot;

    // Public functions
  public:
    EntityAssetOutliner( Luna::SceneManager* sceneManager );
    virtual ~EntityAssetOutliner();
    void AddEntityTypes();

    // Helpers
  private:
    void AddEntityType( Luna::EntityType* entityType );
    void RemoveEntityType( Luna::EntityType* entityType );
    void AddEntityAssetSet( Luna::EntityAssetSet* classSet );
    void RemoveEntityAssetSet( Luna::EntityAssetSet* classSet );
    void AddEntity( Luna::Entity* entity );
    void RemoveEntity( Luna::Entity* entity );

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
    void EntityAdded( const InstanceSetChangeArgs& args );
    void EntityRemoved( const InstanceSetChangeArgs& args );
    void NodeTypeAdded( const NodeTypeExistenceArgs& args );
    void NodeTypeRemoved( const NodeTypeExistenceArgs& args );

  private:
    // GUI callbacks
    void OnBeginLabelEdit( wxTreeEvent& args );
  };
}
