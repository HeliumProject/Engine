#include "Precompile.h"
#include "ComponentNode.h"

#include "AssetEditor.h"
#include "ComponentWrapper.h"

#include "Core/Enumerator.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ComponentNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ComponentNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ComponentNode>( TXT( "Luna::ComponentNode" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ComponentNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ComponentNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentNode::ComponentNode( Luna::ComponentWrapper* component )
: Luna::AssetNode( component->GetAssetManager() )
, m_Component( component )
{
  SetName( m_Component->GetName() );
  SetIcon( m_Component->GetIcon() );

  // Add listeners
  m_Component->AddNameChangedListener( ComponentChangeSignature::Delegate ( this, &ComponentNode::ComponentNameChanged ) );
  m_Component->AddChildrenRefreshListener( ComponentChangeSignature::Delegate ( this, &ComponentNode::ComponentChildrenRefresh ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ComponentNode::~ComponentNode()
{
  // Remove listeners
  m_Component->RemoveNameChangedListener( ComponentChangeSignature::Delegate ( this, &ComponentNode::ComponentNameChanged ) );
  m_Component->RemoveChildrenRefreshListener( ComponentChangeSignature::Delegate ( this, &ComponentNode::ComponentChildrenRefresh ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the attribute that this node references.
// 
Luna::ComponentWrapper* ComponentNode::GetComponent() 
{
  return m_Component;
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the children of this node (passing through to the Luna::ComponentWrapper).
// 
void ComponentNode::CreateChildren()
{
  m_Component->CreateChildren( this );
}

///////////////////////////////////////////////////////////////////////////////
// Update the property panel UI.
// 
void ComponentNode::ConnectProperties( EnumerateElementArgs& args )
{
  m_Component->ConnectProperties( args ); 
}

///////////////////////////////////////////////////////////////////////////////
// Called just before the context menu is shown.  Completely rebuilds the 
// context menu since some of the options added by attributes my be contextual
// based upon what is currently selected.
// 
void ComponentNode::PreShowContextMenu()
{
  // Allow the attribute to populate the context menu
  ContextMenuItemSet& contextMenu = GetContextMenu();
  contextMenu.Clear();
  ContextMenuItemPtr menuItem = new ContextMenuItem( TXT( "Remove Component" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( GetAssetManager()->GetAssetEditor(), &AssetEditor::RemoveSelectedComponents ) );
  contextMenu.AppendItem( menuItem );

  m_Component->PopulateContextMenu( contextMenu );

  contextMenu.AppendSeparator();
  AddDefaultContextMenuItems( contextMenu );

}

///////////////////////////////////////////////////////////////////////////////
// Components can be copied.
// 
bool ComponentNode::CanBeCopied() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to get the clipboard data for this attribute.
// 
Inspect::ReflectClipboardDataPtr ComponentNode::GetClipboardData()
{
  return m_Component->GetClipboardData();
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to determine if the clipboard data is valid to be pasted or
// dropped on this node.
// 
bool ComponentNode::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  return m_Component->CanHandleClipboardData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Pass through to handle a paste operation.
// 
bool ComponentNode::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  return m_Component->HandleClipboardData( data, op, batch );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the attribute associated with this node has it's name changed.
// 
void ComponentNode::ComponentNameChanged( const ComponentChangeArgs& args )
{
  if ( args.m_Component == m_Component )
  {
    SetName( m_Component->GetName() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an attribute needs to have it's UI updated by clearing out
// all its children and recreating them.
// 
void ComponentNode::ComponentChildrenRefresh( const ComponentChangeArgs& args )
{
  if ( args.m_Component == m_Component )
  {
    DeleteChildren();
    CreateChildren();
  }
}
