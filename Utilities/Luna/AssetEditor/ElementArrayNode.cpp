#include "Precompile.h"
#include "ElementArrayNode.h"
#include "ArrayItemNode.h"
#include "AssetManager.h"
#include "HierarchyChangeToken.h"
#include "PersistentNode.h"

#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"
#include "Reflect/ElementArraySerializer.h"
#include "Foundation/TUID.h"
#include "Luna/UI/ImageManager.h"

using namespace Luna;


// Definition
LUNA_DEFINE_TYPE( Luna::ElementArrayNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ElementArrayNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ElementArrayNode>( "Luna::ElementArrayNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ElementArrayNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ElementArrayNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ElementArrayNode::ElementArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::ArrayNode( assetManager, element, field )
{
  if ( field->m_Create )
  {
    ContextMenuItemSet& contextMenu = GetContextMenu();
    ContextMenuItemPtr menuItem = new ContextMenuItem( "Add" );
    menuItem->AddCallback( ContextMenuSignature::Delegate::Create<ElementArrayNode, void (ElementArrayNode::*)( const ContextMenuArgsPtr& )>( this, &ElementArrayNode::AddNewArrayElement ) );
    contextMenu.AppendSeparator();
    contextMenu.AppendItem( menuItem );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ElementArrayNode::~ElementArrayNode()
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the children of this node.
// 
void ElementArrayNode::CreateChildren()
{
  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  Reflect::V_Element::iterator itr = serializer->m_Data->begin();
  Reflect::V_Element::iterator end = serializer->m_Data->end();
  for ( ; itr != end; ++itr )
  {
    const Reflect::ElementPtr& element = *itr;
    NewChild( element );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified child.
// Overridden to track any Luna::PersistentNode children.
// 
bool ElementArrayNode::AddChild( AssetNodePtr child, Luna::AssetNode* beforeSibling )
{
  if ( __super::AddChild( child, beforeSibling ) )
  {
    Luna::PersistentNode* persist = Reflect::ObjectCast< Luna::PersistentNode >( child );
    if ( persist )
    {
      m_PersistentLookup.insert( M_PersistentNodeDumbPtr::value_type( persist->GetPersistentData< Luna::PersistentData >()->GetPackage< Reflect::Element >(), persist ) );
    }

    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified child.
// Overridden to track any Luna::PersistentNode children.
// 
bool ElementArrayNode::RemoveChild( AssetNodePtr child )
{
  if ( __super::RemoveChild( child ) )
  {
    Luna::PersistentNode* persist = Reflect::ObjectCast< Luna::PersistentNode >( child );
    if ( persist )
    {
      m_PersistentLookup.erase( persist->GetPersistentData< Luna::PersistentData >()->GetPackage< Reflect::Element >() );
    }
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Called whenever the field associated with this array node is changed.  Reads
// the data from the field and updates this node's children accordingly: Adds 
// and new children, removes any children that are no longer present, and 
// rearranges all items to match the field data.
// 
void ElementArrayNode::HandleFieldChanged()
{
  // Hold on to this pointer for the duration of this operation to freeze the
  // outliner until we are finished adding and removing items.  Makes the
  // tree control more efficient.
  LHierarchyChangeTokenPtr hierarchyChangeToken = GetAssetManager()->GetHierarchyChangeToken();

  // Build out a new list of child nodes, in the correct order.
  V_AssetNodeSmartPtr newChildList;
  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  newChildList.reserve( serializer->m_Data->size() );
  Reflect::V_Element::iterator serializerItr = serializer->m_Data->begin();
  Reflect::V_Element::iterator serializerEnd = serializer->m_Data->end();
  for ( ; serializerItr != serializerEnd; ++serializerItr )
  {
    const Reflect::ElementPtr& element = *serializerItr;
    M_PersistentNodeDumbPtr::const_iterator found = m_PersistentLookup.find( element.Ptr() );
    if ( found == m_PersistentLookup.end() )
    {
      // If this element is not in our child data, add it
      newChildList.push_back( NewChild( element ) );
    }
    else
    {
      // If this element is already one of our children, just add it to the list
      newChildList.push_back( found->second );
    }
  }

  // Remove all the children (we are holding smart pointers to the ones we need).
  DeleteChildren();

  // Rebuild our child list.
  V_AssetNodeSmartPtr::const_iterator childItr = newChildList.begin();
  V_AssetNodeSmartPtr::const_iterator childEnd = newChildList.end();
  for ( ; childItr != childEnd; ++childItr )
  {
    AddChild( *childItr );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command that moves all the selected children of this node up in
// the hierarchy.  Returns NULL if there are no children selected, or if none
// of the selected children can be moved any higher.
// 
Undo::CommandPtr ElementArrayNode::MoveSelectedChildrenUp()
{
  // Prompt for checkout
  if ( !GetAssetManager()->IsEditable( GetAssetClass() ) )
  {
    return NULL;
  }

  // Iterate over all the items.  Move the first selected item up to the spot
  // above its current location.  Move all subsequent selected items up as long
  // as they will not pass another selected item.

  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  const Reflect::V_Element& currentArray = serializer->m_Data.Get();

  Reflect::V_Element newArray;
  newArray.reserve( currentArray.size() );

  Reflect::V_Element::const_iterator itr = currentArray.begin();
  Reflect::V_Element::const_iterator end = currentArray.end();
  for ( ; itr != end; ++itr )
  {
    const Reflect::ElementPtr& element = *itr;
    M_PersistentNodeDumbPtr::const_iterator found = m_PersistentLookup.find( element.Ptr() );
    if ( found != m_PersistentLookup.end() )
    {
      Luna::PersistentNode* child = found->second;
      // If the child node is selected, add it to the list in the appropriate location.
      if ( child->IsSelected() )
      {
        if ( newArray.empty() )
        {
          newArray.push_back( element );
        }
        else
        {
          Reflect::V_Element::iterator previousItr = newArray.end();
          previousItr--;

          if ( m_PersistentLookup.find( *previousItr )->second->IsSelected() )
          {
            newArray.push_back( element );
          }
          else
          {
            newArray.insert( previousItr, element );
          }
        }
      }
      else
      {
        newArray.push_back( element );
      }
    }
    else
    {
      // How did that happen?
      NOC_BREAK();
    }
  }

  if ( newArray != currentArray )
  {
    // Update the persistent data
    return new Undo::PropertyCommand< Reflect::V_Element >( new Nocturnal::MemberProperty< ElementArrayNode, Reflect::V_Element >( this, &ElementArrayNode::GetArray, &ElementArrayNode::SetArray ), newArray );
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command that moves all the currently selected children of this
// node down in the hierarchy.  Returns NULL if there are no children selected,
// or if none of the children can be moved.
// 
Undo::CommandPtr ElementArrayNode::MoveSelectedChildrenDown()
{
  // Prompt for checkout
  if ( !GetAssetManager()->IsEditable( GetAssetClass() ) )
  {
    return NULL;
  }

  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  const Reflect::V_Element& currentArray = serializer->m_Data.Get();

  Reflect::V_Element newArray;
  newArray.reserve( currentArray.size() );

  // Reverse iterate over the items and insert each one into a list.  If the 
  // item is selected, move it down in the list as long as it will not pass
  // another item that is selected.

  Reflect::V_Element::const_reverse_iterator itr = currentArray.rbegin();
  Reflect::V_Element::const_reverse_iterator end = currentArray.rend();
  for ( ; itr != end; ++itr )
  {
    const Reflect::ElementPtr& element = *itr;
    M_PersistentNodeDumbPtr::const_iterator found = m_PersistentLookup.find( element.Ptr() );
    if ( found != m_PersistentLookup.end() )
    {
      Luna::PersistentNode* child = found->second;

      // If the child node is selected, add it to the list in the appropriate location.
      if ( child->IsSelected() )
      {
        if ( newArray.empty() )
        {
          newArray.push_back( element );
        }
        else
        {
          Reflect::V_Element::iterator previousItr = newArray.begin();
          previousItr++;

          bool isFirstItemSelected = m_PersistentLookup.find( *newArray.begin() )->second->IsSelected();

          if ( isFirstItemSelected )
          {
            newArray.insert( newArray.begin(), element );
          }
          else if ( previousItr != newArray.end() )
          {
            newArray.insert( previousItr, element );
          }
          else
          {
            newArray.push_back( element );
          }
        }
      }
      else
      {
        newArray.insert( newArray.begin(), element );
      }
    }
    else
    {
      // How did that happen?
      NOC_BREAK();
    }
  }

  if ( newArray != currentArray )
  {
    // Update the persistent data
    return new Undo::PropertyCommand< Reflect::V_Element >( new Nocturnal::MemberProperty< ElementArrayNode, Reflect::V_Element >( this, &ElementArrayNode::GetArray, &ElementArrayNode::SetArray ), newArray );
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Deletes all of the currently selected children of this node.
// 
Undo::CommandPtr ElementArrayNode::DeleteSelectedChildren()
{
  // Prompt for checkout
  if ( !GetAssetManager()->IsEditable( GetAssetClass() ) )
  {
    return NULL;
  }

  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  // Iterate over all the data items.
  OS_SelectableDumbPtr selection = GetAssetManager()->GetSelection().GetItems();
  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  Reflect::V_Element newData( serializer->m_Data.Get() );
  Reflect::V_Element::iterator itr = newData.begin();
  while ( itr != newData.end() )
  {
    // Find the corresponding child node.
    const Reflect::ElementPtr& element = *itr;
    M_PersistentNodeDumbPtr::const_iterator found = m_PersistentLookup.find( element.Ptr() );
    if ( found != m_PersistentLookup.end() )
    {
      // If the child node is selected, erase the item from the data.
      Luna::PersistentNode* child = found->second;
      if ( child->IsSelected() )
      {
        itr = newData.erase( itr );
        selection.Remove( child );
        continue;
      }
    }

    ++itr;
  }

  if ( newData.size() != serializer->m_Data->size() )
  {
    // Update the selection to not include the deleted items
    batch->Push( GetAssetManager()->GetSelection().SetItems( selection ) );
    
    // Update the persistent data
    batch->Push( new Undo::PropertyCommand< Reflect::V_Element >( new Nocturnal::MemberProperty< ElementArrayNode, Reflect::V_Element >( this, &ElementArrayNode::GetArray, &ElementArrayNode::SetArray ), newData ) );
  }

  if ( !batch->IsEmpty() )
  {
    return batch;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback to move the selected children up in the hierarchy.
// 
void ElementArrayNode::MoveSelectedChildrenUp( const ContextMenuArgsPtr& args )
{
  args->GetBatch()->Push( MoveSelectedChildrenUp() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback to move the selected children down in the hierarchy.
// 
void ElementArrayNode::MoveSelectedChildrenDown( const ContextMenuArgsPtr& args )
{
  args->GetBatch()->Push( MoveSelectedChildrenDown() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for deleting the selected children of this node.
// 
void ElementArrayNode::DeleteSelectedChildren( const ContextMenuArgsPtr& args )
{
  args->GetBatch()->Push( DeleteSelectedChildren() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for adding a new element to the array.
// 
void ElementArrayNode::AddNewArrayElement( const ContextMenuArgsPtr& args )
{
  if ( m_Field->m_Create )
  {
    Reflect::ElementPtr newElement = Reflect::AssertCast< Reflect::Element >( m_Field->m_Create() );
    if ( newElement.ReferencesObject() )
    {
      Reflect::V_Element newArray = GetArray();
      newArray.push_back( newElement );
      args->GetBatch()->Push( new Undo::PropertyCommand< Reflect::V_Element >( new Nocturnal::MemberProperty< ElementArrayNode, Reflect::V_Element >( this, &ElementArrayNode::GetArray, &ElementArrayNode::SetArray ), newArray ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the persistent data array managed by this node and notifies interested 
// listeners.
// 
void ElementArrayNode::SetArray( const Reflect::V_Element& data )
{
  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  serializer->m_Data.Set( data );
  m_Element->RaiseChanged( m_Field );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the element array contained by this node.
// 
const Reflect::V_Element& ElementArrayNode::GetArray() const
{
  Reflect::ElementArraySerializerPtr serializer = Reflect::AssertCast< Reflect::ElementArraySerializer >( m_Serializer );
  return serializer->m_Data.Get();
}

///////////////////////////////////////////////////////////////////////////////
// Creates a new child node to wrap the specified element.
// 
Luna::PersistentNode* ElementArrayNode::NewChild( Reflect::Element* element )
{
  Luna::PersistentNodePtr node = new Luna::PersistentNode( element, GetAssetManager() );

  std::string icon;
  if ( m_Field->GetProperty( Asset::AssetProperties::SmallIcon, icon ) && !icon.empty() )
  {
    node->SetIcon( icon );
  }

  node->CreateChildren();
  AddChild( node );

  ContextMenuItemSet& contextMenu = node->GetContextMenu();

  contextMenu.AppendSeparator();

  ContextMenuItemPtr menuItemMoveUp = new ContextMenuItem( "Move Up\tAlt-UP", "Move the selected item(s) up in the list", Luna::GlobalImageManager().GetBitmap( "arrow_up_16.png" ) );
  menuItemMoveUp->AddCallback( ContextMenuSignature::Delegate::Create<Luna::ElementArrayNode, void (Luna::ElementArrayNode::*)( const ContextMenuArgsPtr& )>( this, &Luna::ElementArrayNode::MoveSelectedChildrenUp ) );
  contextMenu.AppendItem( menuItemMoveUp );

  ContextMenuItemPtr menuItemMoveDown = new ContextMenuItem( "Move Down\tAlt-DOWN", "Move the selected item(s) down in the list", Luna::GlobalImageManager().GetBitmap( "arrow_down_16.png" ) );
  menuItemMoveDown->AddCallback( ContextMenuSignature::Delegate::Create<Luna::ElementArrayNode, void (Luna::ElementArrayNode::*)( const ContextMenuArgsPtr& )>( this, &Luna::ElementArrayNode::MoveSelectedChildrenDown ) );
  contextMenu.AppendItem( menuItemMoveDown );

  contextMenu.AppendSeparator();

  ContextMenuItemPtr menuItemDelete = new ContextMenuItem( "Delete" );
  menuItemDelete->AddCallback( ContextMenuSignature::Delegate::Create<Luna::ElementArrayNode, void (Luna::ElementArrayNode::*)( const ContextMenuArgsPtr& )>( this, &Luna::ElementArrayNode::DeleteSelectedChildren ) );
  contextMenu.AppendItem( menuItemDelete );

  return node;
}
