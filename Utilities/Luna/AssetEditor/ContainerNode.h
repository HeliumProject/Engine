#pragma once

#include "API.h"
#include "FieldNode.h"
#include "ContainerItemNode.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for a field variable that is an array of items.
  // 
  class ContainerNode NOC_ABSTRACT : public Luna::FieldNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ContainerNode, Luna::FieldNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ContainerNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~ContainerNode();
  };
}
