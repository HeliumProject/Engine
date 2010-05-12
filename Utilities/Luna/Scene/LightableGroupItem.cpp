#include "Precompile.h"
#include "LightableGroupItem.h"

#include "Entity.h"
#include "LightableItem.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"

#include "Console/Console.h"
#include "Core/Object.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightableGroupItem::LightableGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, ItemType type )
: GroupItem( tree, job, type )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightableGroupItem::~LightableGroupItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified entity as a child item.
// 
void LightableGroupItem::AddEntity( Luna::Entity* entity )
{
  // Add the child tree item
  GetTree()->Freeze();
  LightableItem* itemData = new LightableItem( GetTree(), GetLightingJob(), entity );
  wxTreeItemId childItem = GetTree()->AppendItem( GetId(), itemData->GetLabel().c_str(), -1, -1, itemData );
  GetTree()->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Removes the tree item that represents the specified entity.
// 
void LightableGroupItem::RemoveEntity( Luna::Entity* entity )
{
  wxTreeItemIdValue cookie;
  wxTreeCtrlBase* tree = GetTree();
  const wxTreeItemId id = GetId();
  wxTreeItemId child = tree->GetFirstChild( id, cookie );
  while ( child.IsOk() )
  {
    LightableItem* data = Cast< LightableItem >( tree->GetItemData( child ), TypeInstance );
    if ( data )
    {
      if ( data->GetEntity() == entity )
      {
        tree->Delete( child );
        break;
      }
    }
    child = tree->GetNextChild( id, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this item is allowed to be selected (determined by the
// current selection which is passed in).
// 
bool LightableGroupItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = reinterpret_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeShadowGroup || firstSelected->GetType() == TypeRenderGroup );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified object can be added as an item in this group.
// 
bool LightableGroupItem::CanAddChild( Object* object ) const
{
  bool canAdd = false;
  if ( object->HasType( Reflect::GetType<Luna::Entity>() ) )
  {
    // Only lightable entities can be part of this group
    canAdd = LightingJob::IsLightable( Reflect::DangerousCast< Luna::Entity >( object ) );
  }
  return canAdd;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified object as a child item under this group (if allowed).
// 
Undo::CommandPtr LightableGroupItem::AddChild( Object* object )
{
  Undo::Command* command = NULL;

  if ( CanAddChild( object ) )
  {
    Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( object );

    tuid zoneID = entity->GetScene()->GetFileID();
    if ( zoneID != TUID::Null )
    {
      // Yuck, special case derived classes because I was too lazy to implement such similar code in
      // the derived classes.
      switch ( GetType() )
      {
      case TypeRenderGroup:
        command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Render, GetLightingJob(), entity, true );
        break;

      case TypeShadowGroup:
        command = new LightingJobCommand( LightingJobCommandActions::Add, LightingJobCommandGroups::Shadow, GetLightingJob(), entity, true );
        break;

      default:
        // This should not happen.  We are trying to add an object that is not of a recognized type.
        Console::Error( "Could not add '%s' to lighting job '%s'\n", entity->GetName().c_str(), GetLightingJob()->GetName().c_str() );
        NOC_BREAK();
        break;
      }
    }
    else
    {
#pragma TODO("Need better UI feedback for the user")
      Console::Error( "Zone '%s' is not in the file resolver yet. Unable to add '%s' to lighting job '%s'.\n", entity->GetScene()->GetFileName().c_str(), entity->GetName().c_str(), GetLightingJob()->GetName().c_str() );
    }
  }

  return command;
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class.  Adds a child item that represents the specified 
// instance.
// 
void LightableGroupItem::AddChildNode( Luna::SceneNode* instance )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( instance );
  if ( entity )
  {
    AddEntity( entity );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Required by base class.  Removes the child tree item that represents the
// specified instance.
// 
void LightableGroupItem::RemoveChildNode( Luna::SceneNode* instance )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( instance );
  if ( entity )
  {
    RemoveEntity( entity );
  }
}
