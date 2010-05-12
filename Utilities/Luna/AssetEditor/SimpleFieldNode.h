#pragma once

#include "API.h"
#include "FieldNode.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around a non-container field on an element.
  // 
  class SimpleFieldNode : public Luna::FieldNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::SimpleFieldNode, Luna::FieldNode );
    static void InitializeType();
    static void CleanupType();

  public:
    SimpleFieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~SimpleFieldNode();

  protected:
    // translate TUIDs to file path and back
    //virtual void TranslateInputTUID( Reflect::TranslateInputEventArgs& args );
    //virtual void TranslateOutputTUID( Reflect::TranslateOutputEventArgs& args );
  };
}
