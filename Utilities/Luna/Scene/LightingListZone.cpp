#include "Precompile.h"
#include "LightingListZone.h"

#include "SceneNodeType.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingListZone::LightingListZone( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: LightingList< Zone >( parent, id, pos, size, style, validator, name )
{
  InsertColumn( ColumnName, "Name" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingListZone::~LightingListZone()
{
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the zones that are in this list and part of the specified
// scene.
// 
void LightingListZone::RemoveSceneItems( Luna::Scene* scene )
{
  Freeze();
  
  HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = scene->GetNodeTypesByType().find( Reflect::GetType<Zone>() );
  if ( found != scene->GetNodeTypesByType().end() )
  {
    S_SceneNodeTypeDumbPtr::const_iterator typeItr = found->second.begin();
    S_SceneNodeTypeDumbPtr::const_iterator typeEnd = found->second.end();
    for ( ; typeItr != typeEnd; ++typeItr )
    {
      Luna::SceneNodeType* nodeType = *typeItr;
      HM_SceneNodeSmartPtr::const_iterator instItr = nodeType->GetInstances().begin();
      HM_SceneNodeSmartPtr::const_iterator instEnd = nodeType->GetInstances().end();
      for ( ; instItr != instEnd; ++instItr )
      {
        const SceneNodePtr& dependNode = instItr->second;
        RemoveListItem( Reflect::ObjectCast< Zone >( dependNode ) );
      }
    }
  }

  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Updates the UI to include a new item with the specified id.
// 
void LightingListZone::DoAddListItem( Zone* item, const i32 itemID )
{
  i32 row = GetItemCount();

  const std::string& name = item->GetName();
  wxListItem nameItem;
  nameItem.SetMask( wxLIST_MASK_TEXT );
  nameItem.SetText( name.c_str() );
  nameItem.SetId( row );
  nameItem.SetData( itemID );
  nameItem.SetColumn( ColumnName );
  InsertItem( nameItem );
}
