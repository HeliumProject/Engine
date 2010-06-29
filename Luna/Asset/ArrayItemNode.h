#pragma once

#include "Luna/API.h"
#include "ContainerItemNode.h"

namespace Luna
{
  class ArrayNode;

  /////////////////////////////////////////////////////////////////////////////
  // Node for a single item within an array.
  // 
  class ArrayItemNode : public Luna::ContainerItemNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ArrayItemNode, Luna::ContainerItemNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ArrayItemNode( Luna::AssetManager* assetManager, Luna::ArrayNode* container, const tstring& name );
    virtual ~ArrayItemNode();
  };
}
