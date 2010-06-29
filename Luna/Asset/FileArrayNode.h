#pragma once

#include "ArrayNode.h"

namespace Luna
{
  class AssetManager;

  /////////////////////////////////////////////////////////////////////////////
  // Node referencing an array of files.
  // 
  class FileArrayNode : public Luna::ArrayNode
  {
  private:
    bool m_IgnoreChange;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FileArrayNode, Luna::ArrayNode );
    static void InitializeType();
    static void CleanupType();
    static bool IsFileArray( Reflect::Element* element, const Reflect::Field* field );

  public:
    FileArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~FileArrayNode();
    virtual void CreateChildren() NOC_OVERRIDE;
    virtual void HandleFieldChanged() NOC_OVERRIDE;

  protected:
    void DeleteSelectedChildren( const ContextMenuArgsPtr& args );
  };
}
