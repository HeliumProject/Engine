#pragma once

#include "AssetNode.h"

namespace Luna
{
  class AssetManager;

  /////////////////////////////////////////////////////////////////////////////
  // Factory for creating Luna::FieldNode-derived classes and their children.
  // 
  class FieldFactory
  {
  private:
    FieldFactory();
  public:
    virtual ~FieldFactory();
    static Luna::FieldFactory* GetInstance();
    void CreateChildFieldNodes( Luna::AssetNode* parent, Reflect::Element* element, Luna::AssetManager* assetManager );

  private:
    AssetNodePtr CreateFieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
  };
}
