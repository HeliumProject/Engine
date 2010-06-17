#pragma once

#include "AssetNode.h"

namespace Luna
{
  // Forwards
  struct ComponentChangeArgs;
  class Enumerator;
  class ComponentWrapper;

  /////////////////////////////////////////////////////////////////////////////
  // Manages a tree node that represents an attribute.
  // 
  class ComponentNode : public Luna::AssetNode
  {
  private:
    Luna::ComponentWrapper* m_Component;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ComponentNode, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ComponentNode( Luna::ComponentWrapper* component );
    virtual ~ComponentNode();
    Luna::ComponentWrapper* GetComponent();
    virtual void CreateChildren() NOC_OVERRIDE;
    virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
    virtual void PreShowContextMenu() NOC_OVERRIDE;
    virtual bool CanBeCopied() const NOC_OVERRIDE;
    virtual Inspect::ReflectClipboardDataPtr GetClipboardData() NOC_OVERRIDE;
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;

  protected:
    void ComponentNameChanged( const ComponentChangeArgs& args );
    void ComponentChildrenRefresh( const ComponentChangeArgs& args );
  };
  typedef Nocturnal::SmartPtr< Luna::ComponentNode > ComponentNodePtr;
}
