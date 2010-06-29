#pragma once

#include "ContainerNode.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for a member variable that is an array of items.
  // 
  class ArrayNode : public Luna::ContainerNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ArrayNode, Luna::ContainerNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~ArrayNode();
    virtual void CreateChildren() NOC_OVERRIDE;

  protected:
    // translate array of TUIDs to file path and back
    //virtual void TranslateInputTUIDArray( Reflect::TranslateInputEventArgs& args );
    //virtual void TranslateOutputTUIDArray( Reflect::TranslateOutputEventArgs& args );
  };
}
