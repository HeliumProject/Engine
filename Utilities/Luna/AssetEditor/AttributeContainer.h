#pragma once

#include "AssetNode.h"

namespace Luna
{
  // Forwards
  struct AttributeExistenceArgs;
  class AttributeNode;
  typedef std::map< i32, Luna::AttributeNode* > M_AttributeNodeDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Attribute::Attribute.
  // 
  class AttributeContainer : public Luna::AssetNode
  {
  private:
    Luna::AssetClass* m_Asset;
    M_AttributeNodeDumbPtr m_Attributes;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AttributeContainer, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  public:
    AttributeContainer( Luna::AssetManager* assetManager, Luna::AssetClass* assetClass );
    virtual ~AttributeContainer();

    virtual bool AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling = NULL ) NOC_OVERRIDE;
    virtual bool RemoveChild( AssetNodePtr child ) NOC_OVERRIDE;
    const M_AttributeNodeDumbPtr& GetAttributes() const;
    virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
  };
}
