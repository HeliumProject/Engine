#include "Precompile.h"
#include "AttributeNode.h"

#include "AssetEditor.h"
#include "AttributeWrapper.h"

#include "Core/Enumerator.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AttributeNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AttributeNode::InitializeType()
{
  Reflect::RegisterClass<Luna::AttributeNode>( "Luna::AttributeNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AttributeNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::AttributeNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AttributeNode::AttributeNode( Luna::AttributeWrapper* attribute )
: Luna::AssetNode( attribute->GetAssetManager() )
, m_Attribute( attribute )
{
  SetName( m_Attribute->GetName() );
  SetIcon( m_Attribute->GetIcon() );

  // Add listeners
  m_Attribute->AddNameChangedListener( AttributeChangeSignature::Delegate ( this, &AttributeNode::AttributeNameChanged ) );
  m_Attribute->AddChildrenRefreshListener( AttributeChangeSignature::Delegate ( this, &AttributeNode::AttributeChildrenRefresh ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AttributeNode::~AttributeNode()
{
  // Remove listeners
  m_Attribute->RemoveNameChangedListener( AttributeChangeSignature::Delegate ( this, &AttributeNode::AttributeNameChanged ) );
  m_Attribute->RemoveChildrenRefreshListener( AttributeChangeSignature::Delegate ( this, &AttributeNode::AttributeChildrenRefresh ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the attribute that this node references.
// 
Luna::AttributeWrapper* AttributeNode::GetAttribute() 
{
  return m_Attribute;
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the children of this node (passing through to the Luna::AttributeWrapper).
// 
void AttributeNode::CreateChildren()
{
  m_Attribute->CreateChildren( this );
}

///////////////////////////////////////////////////////////////////////////////
// Update the property panel UI.
// 
void AttributeNode::ConnectProperties( EnumerateElementArgs& args )
{
  m_Attribute->ConnectProperties( args ); 
}

///////////////////////////////////////////////////////////////////////////////
// Called just before the context menu is shown.  Completely rebuilds the 
// context menu since some of the options added by attributes my be contextual
// based upon what is currently selected.
// 
void AttributeNode::PreShowContextMenu()
{
  // Allow the attribute to populate the context menu
  ContextMenuItemSet& contextMenu = GetContextMenu();
  contextMenu.Clear();
  ContextMenuItemPtr menuItem = new ContextMenuItem( "Remove Attribute" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( GetAssetManager()->GetAssetEditor(), &AssetEditor::RemoveSelectedAttributes ) );
  contextMenu.AppendItem( menuItem );

  m_Attribute->PopulateContextMenu( contextMenu );

  contextMenu.AppendSeparator();
  AddDefaultContextMenuItems( contextMenu );

}

///////////////////////////////////////////////////////////////////////////////
// Attributes can be copied.
// 
bool AttributeNode::CanBeCopied() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to get the clipboard data for this attribute.
// 
Inspect::ReflectClipboardDataPtr AttributeNode::GetClipboardData()
{
  return m_Attribute->GetClipboardData();
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to determine if the clipboard data is valid to be pasted or
// dropped on this node.
// 
bool AttributeNode::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  return m_Attribute->CanHandleClipboardData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to handle a paste operation.
// 
bool AttributeNode::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  return m_Attribute->HandleClipboardData( data, op, batch );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the attribute associated with this node has it's name changed.
// 
void AttributeNode::AttributeNameChanged( const AttributeChangeArgs& args )
{
  if ( args.m_Attribute == m_Attribute )
  {
    SetName( m_Attribute->GetName() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an attribute needs to have it's UI updated by clearing out
// all its children and recreating them.
// 
void AttributeNode::AttributeChildrenRefresh( const AttributeChangeArgs& args )
{
  if ( args.m_Attribute == m_Attribute )
  {
    DeleteChildren();
    CreateChildren();
  }
}
