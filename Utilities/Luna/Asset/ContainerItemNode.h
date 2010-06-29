#pragma once

#include "AssetNode.h"

namespace Luna
{
  class ContainerNode;

  class ContainerItemNode : public Luna::AssetNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ContainerItemNode, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  private:
    Luna::ContainerNode* m_Container;

  public:
    ContainerItemNode( Luna::AssetManager* assetManager, Luna::ContainerNode* container, const tstring& name );
    virtual ~ContainerItemNode();
  };
  typedef Nocturnal::SmartPtr< Luna::ContainerItemNode > ContainerItemNodePtr;
  typedef std::vector< Luna::ContainerItemNodePtr > V_ContainerItemNodeSmartPtr;
}
