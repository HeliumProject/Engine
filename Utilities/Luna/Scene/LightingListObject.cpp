#include "Precompile.h"
#include "LightingListObject.h"

#include "EntityType.h"
#include "EntityAssetSet.h"
#include "LightingJob.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/BakedLightingAttribute.h"
#include "Asset/TexelsPerMeterAttribute.h"
#include "Editor/SharedFileManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingListObject::LightingListObject( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: LightingList< Luna::Entity >( parent, id, pos, size, style, validator, name )
{
  InsertColumn( ColumnName, "Name" );
  InsertColumn( ColumnZone, "Zone" );
  InsertColumn( ColumnLit, "Lit?" );
  InsertColumn( ColumnLightmap, "Lightmap" );
  InsertColumn( ColumnTexelsPerMeter, "TexelsPerMeter" );
  InsertColumn( ColumnLightmap1, "LM1" );
  InsertColumn( ColumnLightmap2, "LM2" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingListObject::~LightingListObject()
{
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the entities from this list that are part of the specified
// scene.
// 
void LightingListObject::RemoveSceneItems( Luna::Scene* scene )
{
  Freeze();
  HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = scene->GetNodeTypesByName().begin();
  HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = scene->GetNodeTypesByName().end();
  for ( ; typeItr != typeEnd; ++typeItr )
  {
    const SceneNodeTypePtr& sceneType = typeItr->second;
    if ( sceneType->HasType( Reflect::GetType<Luna::EntityType>() ) )
    {
      Luna::EntityType* entityType = Reflect::DangerousCast< Luna::EntityType >( sceneType );
      HM_SceneNodeSmartPtr::const_iterator instItr = entityType->GetInstances().begin();
      HM_SceneNodeSmartPtr::const_iterator instEnd = entityType->GetInstances().end();
      for ( ; instItr != instEnd; ++instItr )
      {
        const SceneNodePtr& dependNode = instItr->second;
        RemoveListItem( Reflect::ObjectCast< Luna::Entity >( dependNode ) );
      }
    }
  }
  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to provide some additional listeners.
// 
void LightingListObject::SceneAdded( const SceneChangeArgs& args )
{
  __super::SceneAdded( args );

  args.m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingListObject::EntitySetAdded ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to provide some additional listeners.
// 
void LightingListObject::SceneRemoved( const SceneChangeArgs& args )
{
  __super::SceneRemoved( args );

  args.m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingListObject::EntitySetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds an item to the UI to represent the specified entity (it will have the
// specified id so that the item can be fetched back later from just a list
// index).
// 
void LightingListObject::DoAddListItem( Luna::Entity* item, const i32 itemID )
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

  const std::string lit( "Unknown" );
  wxListItem litItem;
  litItem.SetMask( wxLIST_MASK_TEXT );
  litItem.SetText( lit.c_str() );
  litItem.SetId( row );
  litItem.SetColumn( ColumnLit );
  SetItem( litItem );

  UpdateLightMapSetting( item, row );

  UpdateTPM( item, row );

  // Connect listener
  item->AddChangedListener( ObjectChangeSignature::Delegate ( this, &LightingListObject::ObjectChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the item is lightable.
// 
bool LightingListObject::ShouldAddItem( Luna::Entity* item )
{
  return item && LightingJob::IsLightable( item );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a class set is added to an entity node.  If the set is an
// entity set, we add ourselves as a listener for when it is reloaded.
// 
void LightingListObject::EntitySetAdded( const NodeChangeArgs& args )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Node );
  if( entity )
  {
    Luna::EntityAssetSet* entitySet = entity->GetClassSet();

    if ( entitySet && entitySet->GetEntityAssetID() != TUID::Null )
    {
      entitySet->AddClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &LightingListObject::EntityAssetSetReloaded ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a class set is added to an entity node.  If the set is an
// entity set, we add ourselves as a listener for when it is reloaded.
// 
void LightingListObject::EntitySetRemoved( const NodeChangeArgs& args )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Node );
  if( entity )
  {
    Luna::EntityAssetSet* entitySet = entity->GetClassSet();

    if ( entitySet && entitySet->GetEntityAssetID() != TUID::Null )
    {
      entitySet->RemoveClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &LightingListObject::EntityAssetSetReloaded ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity class set is reloaded.  When the entity class set
// is reloaded, it may not be valid for display in the entity panel any more (or
// it may need to be added to the lighting panel if it's not there).  This 
// function updates the lighting panel accordingly.
// 
void LightingListObject::EntityAssetSetReloaded( const EntityAssetSetChangeArgs& args )
{
  Freeze();

  // Iterate over all entities in all scenes
  M_SceneSmartPtr::const_iterator sceneItr = m_SceneEditor->GetSceneManager()->GetScenes().begin();
  M_SceneSmartPtr::const_iterator sceneEnd = m_SceneEditor->GetSceneManager()->GetScenes().end();
  for ( ; sceneItr != sceneEnd; ++sceneItr )
  {
    Luna::Scene* scene = sceneItr->second;
    HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = scene->GetNodeTypesByName().begin();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = scene->GetNodeTypesByName().end();
    for ( ; typeItr != typeEnd; ++typeItr )
    {
      const SceneNodeTypePtr& sceneType = typeItr->second;
      if ( sceneType->HasType( Reflect::GetType< Luna::EntityType >() ) )
      {
        Luna::EntityType* entityType = Reflect::DangerousCast< Luna::EntityType >( sceneType );
        HM_SceneNodeSmartPtr::const_iterator instItr = entityType->GetInstances().begin();
        HM_SceneNodeSmartPtr::const_iterator instEnd = entityType->GetInstances().end();
        for ( ; instItr != instEnd; ++instItr )
        {
          const SceneNodePtr& dependNode = instItr->second;
          Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( dependNode );

          // If the entity's class set matches the file that was reloaded...
          if ( entity && entity->GetClassSet() && entity->GetClassSet()->GetEntityAssetID() == args.m_EntityAssetSet->GetEntityAssetID() )
          {
            if ( ShouldAddItem( entity ) )
            {
              // If the entity is lightable, make sure it is already in our list
              AddListItem( entity );
            }
            else
            {
              // Else, if the entity is not lightable, make sure it's not in our list
              RemoveListItem( entity );
            }
          }
        }
      }
    }
  }

  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an object has changed.  If the item that was changed is 
// an entity, the UI updates that entity's lightmap setting to Reflect any 
// change that may have been made.
// 
void LightingListObject::ObjectChanged( const ObjectChangeArgs& args )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Object );
  if ( entity )
  {
    RM_ObjToId::M_AToB::const_iterator found = m_Map.AToB().find( entity );
    if ( found != m_Map.AToB().end() )
    {
      i32 data = *found->second;
      long row = FindItem( -1, data );

      if ( row >= 0 )
      {
        UpdateLightMapSetting( entity, row );
        UpdateTPM( entity, row );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Updates the lightmap setting in the specified row with the value from
// the entity that is passed in.
// 
void LightingListObject::UpdateLightMapSetting( Luna::Entity* item, long row )
{
  std::string lightMap( "Invalid" );
  std::string lm1( "-" );
  std::string lm2( "-" );
  Attribute::AttributeCollectionPtr collection = item->GetPackage< Attribute::AttributeCollection >();
  Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( collection );
  if ( bakedLighting.Valid() )
  {
    lightMap = bakedLighting->GetLightmapSettingString( bakedLighting->m_LightmapSetting );

    if( bakedLighting->m_LightmapSetting != Asset::LightmapSettings::VertexOnly )
    {
      lm1 = bakedLighting->GetLightmapSettingString( (Asset::LightmapSetting)bakedLighting->m_RuntimeLMSize1 );
      lm2 = bakedLighting->GetLightmapSettingString( (Asset::LightmapSetting)bakedLighting->m_RuntimeLMSize2 );
    }  
  }
  wxListItem lightMapItem;
  lightMapItem.SetMask( wxLIST_MASK_TEXT );
  lightMapItem.SetText( lightMap.c_str() );
  lightMapItem.SetId( row );
  lightMapItem.SetColumn( ColumnLightmap );
  SetItem( lightMapItem );

  wxListItem lm1Item;
  lm1Item.SetMask( wxLIST_MASK_TEXT );
  lm1Item.SetText( lm1.c_str() );
  lm1Item.SetId( row );
  lm1Item.SetColumn( ColumnLightmap1 );
  SetItem( lm1Item );

  wxListItem lm2Item;
  lm2Item.SetMask( wxLIST_MASK_TEXT );
  lm2Item.SetText( lm2.c_str() );
  lm2Item.SetId( row );
  lm2Item.SetColumn( ColumnLightmap2 );
  SetItem( lm2Item );
}


///////////////////////////////////////////////////////////////////////////////
// Updates the "lit" column in the specified row with the value from
// the entity that is passed in.
// 
void LightingListObject::UpdateLit( Luna::Entity* item, bool lit )
{
  if ( item )
  {
    RM_ObjToId::M_AToB::const_iterator found = m_Map.AToB().find( item );
    if ( found != m_Map.AToB().end() )
    {
      i32 data = *found->second;
      long row = FindItem( -1, data );

      if ( row >= 0 )
      {
        std::string litStr;
        if( lit )
          litStr = "yes";
        else
          litStr = "no";
        wxListItem lightMapItem;
        lightMapItem.SetMask( wxLIST_MASK_TEXT );
        lightMapItem.SetText( litStr.c_str() );
        lightMapItem.SetId( row );
        lightMapItem.SetColumn( ColumnLit );
        SetItem( lightMapItem );    
      }
    }
  }  
}

void LightingListObject::UpdateTPM( Luna::Entity* item, long row )
{
  Attribute::AttributeCollectionPtr collection = item->GetPackage< Attribute::AttributeCollection >();
  Attribute::AttributeViewer< Asset::TexelsPerMeterAttribute > tpm( collection );

  std::stringstream stream;
  if ( tpm.Valid() )
  {
    stream << tpm->m_LightmapTexelsPerMeter;
  }
  wxListItem tpmItem;
  tpmItem.SetMask( wxLIST_MASK_TEXT );
  tpmItem.SetText( stream.str().c_str() );
  tpmItem.SetId( row );
  tpmItem.SetColumn( ColumnTexelsPerMeter );
  SetItem( tpmItem ); 
}
