#include "Precompile.h"
#include "Region.h"
#include "RegionPanel.h"
#include "Scene.h"

#include "Application/UI/ImageManager.h"
#include "Core/Enumerator.h"

#include <algorithm>

using namespace Luna; 

LUNA_DEFINE_TYPE( Luna::Region ); 

void Region::InitializeType()
{
  Reflect::RegisterClass< Luna::Region >( TXT( "Luna::Region" ) ); 
  Enumerator::InitializePanel( TXT( "Region" ), CreatePanelSignature::Delegate( &Region::CreatePanel ) );
}

void Region::CleanupType()
{
  Reflect::UnregisterClass< Luna::Region >();
}

Region::Region(Luna::Scene* scene, Content::Region* region)
  : Luna::SceneNode(scene, region)
{
  m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate(this, &Region::NodeAdded)); 
  m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate(this, &Region::NodeRemoved)); 
}

Region::~Region()
{
  m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate(this, &Region::NodeAdded)); 
  m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate(this, &Region::NodeRemoved)); 
}

i32 Region::GetImageIndex() const
{
  return Nocturnal::GlobalImageManager().GetImageIndex( TXT( "region.png" ) );
}

tstring Region::GetApplicationTypeName() const
{
  return TXT( "Region" );
}

void Region::SetName(const tstring& value)
{
  Content::RegionPtr region = GetPackage<Content::Region>();

  const S_ZoneDumbPtr& zones = m_Scene->GetZones(); 

  S_ZoneDumbPtr::const_iterator itr = zones.begin(); 
  S_ZoneDumbPtr::const_iterator end = zones.end(); 

  for(itr = zones.begin() ; itr != end; ++itr)
  {
    if(region->HasZone( (*itr)->GetID() ) )
    {
      RemoveSelfFromZone( *itr ); 
    }
  }

  __super::SetName(value); 

  for(itr = zones.begin() ; itr != end; ++itr)
  {
    if(region->HasZone( (*itr)->GetID() ) )
    {
      AddSelfToZone( *itr ); 
    }
  }

}

void Region::RemoveZone( const ZonePtr& zone )
{
  GetPackage<Content::Region>()->RemoveZone( zone->GetID() ); 

  RemoveSelfFromZone( zone ); 
}

void Region::AddZone( const ZonePtr& zone )
{
  GetPackage<Content::Region>()->AddZone( zone->GetID() ); 

  AddSelfToZone( zone ); 
}


void Region::NodeAdded( const NodeChangeArgs& args )
{
#if 0 
  ZonePtr zoneWrapper = Refelect::ObjectCast<Zone>(args.m_Node); 
  if(zoneWrapper)
  {
    Content::RegionPtr region = GetPackage<Content::Region>(); 
    if(region->HasZone( zoneWrapper->GetID() ) )
    {
      // make sure that we are added to the zone array stuff
      // this is harmless while we are loading stuff up
      
    }
  }
#endif
}

void Region::NodeRemoved( const NodeChangeArgs& args )
{
  if(args.m_Node == this)
  {
    // we get this call when we are being removed from the scene
    // when that happens, we need to make sure that all of our 
    // zones are updated to let them know not to be members of us
    // 
    // this should basically go away when we start using the regions
    // in the builders
    // 

    const S_ZoneDumbPtr& zones = m_Scene->GetZones(); 
    
    S_ZoneDumbPtr::const_iterator itr = zones.begin(); 
    S_ZoneDumbPtr::const_iterator end = zones.end(); 

    for( ; itr != end; ++itr)
    {
      RemoveSelfFromZone( *itr ); 
    }

  }
  
  ZonePtr zoneWrapper = Reflect::ObjectCast<Zone>(args.m_Node); 
  if(zoneWrapper)
  {
    Content::RegionPtr region = GetPackage<Content::Region>(); 

    // this zone is being deleted. 
    // remove it from our list! 
    // 
    // yes, this HasZone check is redundant
    if(region->HasZone( zoneWrapper->GetID() ) )
    {
      region->RemoveZone( zoneWrapper->GetID() ); 
    }
  }
}

std::vector< tstring >::iterator insensitive_find(std::vector< tstring >& strings, const tstring& query)
{
  for(std::vector< tstring >::iterator itr = strings.begin(); itr != strings.end(); ++itr)
  {
    if(!_tcsicmp(query.c_str(), (*itr).c_str()))
    {
      return itr; 
    }
  }
  return strings.end(); 
}

void Region::RemoveSelfFromZone( const ZonePtr& zoneWrapper )
{
  Content::ZonePtr zone = zoneWrapper->GetPackage<Content::Zone>(); 
  std::vector< tstring >::iterator found = insensitive_find(zone->m_Regions, GetName()); 
  
  if(found != zone->m_Regions.end())
  {
    zone->m_Regions.erase(found); 
    zone->RaiseChanged(); 
  }

}

void Region::AddSelfToZone( const ZonePtr& zoneWrapper )
{
  Content::ZonePtr zone = zoneWrapper->GetPackage<Content::Zone>(); 
  std::vector< tstring >::iterator found = insensitive_find(zone->m_Regions, GetName()); 
  
  if(found == zone->m_Regions.end())
  {
    zone->m_Regions.push_back( GetName() ); 
    zone->RaiseChanged(); 
  }
}


bool Region::ValidatePanel(const tstring& name)
{
  if ( name == TXT( "Region" ) )
    return true;

  return __super::ValidatePanel( name );
}

void Region::CreatePanel( CreatePanelArgs& args )
{
  RegionPanel* panel = new RegionPanel ( args.m_Enumerator, args.m_Selection );

  args.m_Enumerator->Push( panel );
  {
    panel->SetCanvas( args.m_Enumerator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Enumerator->Pop();
}