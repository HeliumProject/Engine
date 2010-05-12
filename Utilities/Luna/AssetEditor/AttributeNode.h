#pragma once

#include "AssetNode.h"

namespace Luna
{
  // Forwards
  struct AttributeChangeArgs;
  class Enumerator;
  class AttributeWrapper;

  /////////////////////////////////////////////////////////////////////////////
  // Manages a tree node that represents an attribute.
  // 
  class AttributeNode : public Luna::AssetNode
  {
  private:
    Luna::AttributeWrapper* m_Attribute;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AttributeNode, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  public:
    AttributeNode( Luna::AttributeWrapper* attribute );
    virtual ~AttributeNode();
    Luna::AttributeWrapper* GetAttribute();
    virtual void CreateChildren() NOC_OVERRIDE;
    virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
    virtual void PreShowContextMenu() NOC_OVERRIDE;
    virtual bool CanBeCopied() const NOC_OVERRIDE;
    virtual Inspect::ReflectClipboardDataPtr GetClipboardData() NOC_OVERRIDE;
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;

  protected:
    void AttributeNameChanged( const AttributeChangeArgs& args );
    void AttributeChildrenRefresh( const AttributeChangeArgs& args );
  };
  typedef Nocturnal::SmartPtr< Luna::AttributeNode > AttributeNodePtr;
}
