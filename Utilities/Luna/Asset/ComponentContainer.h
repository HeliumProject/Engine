#pragma once

#include "AssetNode.h"

namespace Luna
{
  // Forwards
  struct ComponentExistenceArgs;
  class ComponentNode;
  typedef std::map< i32, Luna::ComponentNode* > M_ComponentNodeDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Component::Component.
  // 
  class ComponentContainer : public Luna::AssetNode
  {
  private:
    Luna::AssetClass* m_Asset;
    M_ComponentNodeDumbPtr m_Components;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ComponentContainer, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ComponentContainer( Luna::AssetManager* assetManager, Luna::AssetClass* assetClass );
    virtual ~ComponentContainer();

    virtual bool AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling = NULL ) NOC_OVERRIDE;
    virtual bool RemoveChild( AssetNodePtr child ) NOC_OVERRIDE;
    const M_ComponentNodeDumbPtr& GetComponents() const;
    virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
  };
}
