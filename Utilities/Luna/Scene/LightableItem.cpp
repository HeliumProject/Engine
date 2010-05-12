#include "Precompile.h"
#include "LightableItem.h"

#include "Entity.h"
#include "LightingJob.h"
#include "LightingJobCommand.h"
#include "Scene.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightableItem::LightableItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, Luna::Entity* entity )
: SceneNodeItem( tree, job, entity, TypeInstance )
, m_Entity( entity )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightableItem::~LightableItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the entity that this class is wrapping.
// 
Luna::Entity* LightableItem::GetEntity() const
{
  return m_Entity;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the other selected objects are also lightable items.
// 
bool LightableItem::IsSelectable( const OS_TreeItemIds& currentSelection ) const
{
  bool isSelectable = false;

  if ( currentSelection.Size() == 0 )
  {
    isSelectable = true;
  }
  else
  {
    LightingTreeItem* firstSelected = reinterpret_cast< LightingTreeItem* >( GetTree()->GetItemData( *currentSelection.Begin() ) );
    isSelectable = ( firstSelected->GetType() == TypeInstance );
  }

  return isSelectable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command to remove the entity wrapped by this tree item from the
// lighting job.
// 
Undo::CommandPtr LightableItem::GetRemoveCommand( const OS_SelectableDumbPtr& selection )
{
  Undo::CommandPtr command;

  if ( selection.Contains( m_Entity ) )
  {
    wxTreeItemId parentItem = GetTree()->GetItemParent( GetId() );
    if ( parentItem.IsOk() )
    {
      LightingTreeItem* parentData = static_cast< LightingTreeItem* >( GetTree()->GetItemData( parentItem ) );
      if ( parentData )
      {
        // Yuck...
        switch ( parentData->GetType() )
        {
        case TypeRenderGroup:
          command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Render, GetLightingJob(), m_Entity, false );
          break;

        case TypeShadowGroup:
          command = new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Shadow, GetLightingJob(), m_Entity, false );
          break;

        default:
          NOC_BREAK(); // Shouldn't happen
          break;
        }
      }
    }
  }

  return command;
}
