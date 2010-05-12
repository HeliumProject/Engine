#include "Region.h"
#include <algorithm>

#include "Attribute/AttributeHandle.h"

using namespace Content; 
using namespace Attribute; 

REFLECT_DEFINE_CLASS(Region)

void Region::EnumerateClass( Reflect::Compositor<Region>& comp )
{
  Reflect::Field* fieldZoneIds = comp.AddField( &Region::m_ZoneIds, "m_ZoneIds", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldOverrideOcclusionSettings = comp.AddField( &Region::m_OverrideOcclusionSettings, "m_OverrideOcclusionSettings" );
  Reflect::Field* fieldSpatialGridSize = comp.AddField( &Region::m_SpatialGridSize, "m_SpatialGridSize" );
  Reflect::Field* fieldOcclTestDownwardColl = comp.AddField( &Region::m_OcclTestDownwardColl, "m_OcclTestDownwardColl" );
  Reflect::Field* fieldOcclTestDownwardVis = comp.AddField( &Region::m_OcclTestDownwardVis, "m_OcclTestDownwardVis" );
  Reflect::Field* fieldOcclVisDistAdjust = comp.AddField( &Region::m_OcclVisDistAdjust, "m_OcclVisDistAdjust" );
}

Region::Region()
: SceneNode()
, m_SpatialGridSize( 2.0f )
, m_OcclTestDownwardColl( true )
, m_OcclTestDownwardVis( true )
, m_OcclVisDistAdjust( 0.0f )
, m_OverrideOcclusionSettings(false)
{

}


bool Region::HasZone(UniqueID::TUID zoneId) const
{
  return std::find(m_ZoneIds.begin(), m_ZoneIds.end(), zoneId) != m_ZoneIds.end(); 
}

void Region::AddZone(UniqueID::TUID zoneId)
{
  if(!HasZone(zoneId))
  {
    m_ZoneIds.push_back(zoneId); 
    RaiseChanged( ZonesField() ); 
  }
}

void Region::RemoveZone(UniqueID::TUID zoneId)
{
  if(HasZone(zoneId))
  {
    m_ZoneIds.erase( std::find(m_ZoneIds.begin(), m_ZoneIds.end(), zoneId) ); 
    RaiseChanged( ZonesField() ); 
  }
}
