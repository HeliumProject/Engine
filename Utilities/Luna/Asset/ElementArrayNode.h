#pragma once

#include "ArrayNode.h"

namespace Luna
{
  class AssetManager;
  class PersistentNode;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for a member variable that is an array of items.
  // 
  class ElementArrayNode : public Luna::ArrayNode
  {
  private:
    typedef std::map< Reflect::Element*, Luna::PersistentNode* > M_PersistentNodeDumbPtr;

  private:
    M_PersistentNodeDumbPtr m_PersistentLookup;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ElementArrayNode, Luna::ArrayNode );
    static void InitializeType();
    static void CleanupType();

  public:
    ElementArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~ElementArrayNode();
    virtual void CreateChildren() NOC_OVERRIDE;
    virtual bool AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling = NULL ) NOC_OVERRIDE;
    virtual bool RemoveChild( AssetNodePtr child ) NOC_OVERRIDE;
    virtual void HandleFieldChanged() NOC_OVERRIDE;
    virtual Undo::CommandPtr MoveSelectedChildrenUp();
    virtual Undo::CommandPtr MoveSelectedChildrenDown();
    virtual Undo::CommandPtr DeleteSelectedChildren();

  protected:
    void AddNewArrayElement( const ContextMenuArgsPtr& args );
    void MoveSelectedChildrenUp( const ContextMenuArgsPtr& args );
    void MoveSelectedChildrenDown( const ContextMenuArgsPtr& args );
    void DeleteSelectedChildren( const ContextMenuArgsPtr& args );
    void SetArray( const Reflect::V_Element& data );
    const Reflect::V_Element& GetArray() const;
    Luna::PersistentNode* NewChild( Reflect::Element* element );
  };
}
