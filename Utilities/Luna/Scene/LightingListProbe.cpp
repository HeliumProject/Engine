#include "Precompile.h"
#include "LightingListProbe.h"

#include "SceneNodeType.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingListProbe::LightingListProbe( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: LightingList< Luna::CubeMapProbe >( parent, id, pos, size, style, validator, name )
{
  InsertColumn( ColumnName, "Name" );
  InsertColumn( ColumnZone, "Zone" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingListProbe::~LightingListProbe()
{
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the cube map probes from this list that are part of the specified
// scene.
// 
void LightingListProbe::RemoveSceneItems( Luna::Scene* scene )
{
  Freeze();
  
  HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = scene->GetNodeTypesByType().find( Reflect::GetType<Luna::CubeMapProbe>() );
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
        RemoveListItem( Reflect::ObjectCast< Luna::CubeMapProbe >( dependNode ) );
      }
    }
  }

  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Updates the UI to include a new list item for the specified cube map probe.
// 
void LightingListProbe::DoAddListItem( Luna::CubeMapProbe* item, const i32 itemID )
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

  const std::string zone = item->GetScene()->GetFileName();
  wxListItem zoneItem;
  zoneItem.SetMask( wxLIST_MASK_TEXT );
  zoneItem.SetText( zone.c_str() );
  zoneItem.SetId( row );
  zoneItem.SetColumn( ColumnZone );
  SetItem( zoneItem );
}
