#include "Precompile.h"
#include "LightingListVolume.h"

#include "LightingVolumeType.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingListVolume::LightingListVolume( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: LightingList< Luna::LightingVolume >( parent, id, pos, size, style, validator, name )
{
  InsertColumn( ColumnName, "Name" );
  InsertColumn( ColumnZone, "Zone" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingListVolume::~LightingListVolume()
{
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the lighting volumes that are part of the specified scene.
// 
void LightingListVolume::RemoveSceneItems( Luna::Scene* scene )
{
  Freeze();
  HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = scene->GetNodeTypesByName().begin();
  HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = scene->GetNodeTypesByName().end();
  for ( ; typeItr != typeEnd; ++typeItr )
  {
    const SceneNodeTypePtr& sceneType = typeItr->second;
    if ( sceneType->HasType( Reflect::GetType<Luna::LightingVolumeType>() ) )
    {
      Luna::LightingVolumeType* volumeType = Reflect::DangerousCast< Luna::LightingVolumeType >( sceneType );
      HM_SceneNodeSmartPtr::const_iterator instItr = volumeType->GetInstances().begin();
      HM_SceneNodeSmartPtr::const_iterator instEnd = volumeType->GetInstances().end();
      for ( ; instItr != instEnd; ++instItr )
      {
        const SceneNodePtr& dependNode = instItr->second;
        RemoveListItem( Reflect::ObjectCast< Luna::LightingVolume >( dependNode ) );
      }
    }
  }
  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new list item to represent the specified lighting volume.
// 
void LightingListVolume::DoAddListItem( Luna::LightingVolume* item, const i32 itemID )
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
