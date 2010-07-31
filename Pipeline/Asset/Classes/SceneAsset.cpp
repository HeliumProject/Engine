#include "SceneAsset.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Asset/Components/DependenciesComponent.h"
#include "Foundation/Component/ComponentHandle.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( SceneAsset );

void SceneAsset::EnumerateClass( Reflect::Compositor<SceneAsset>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Scene" );
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A level groups together various zones to make a level in the game.  The level asset will be associated with a world file (*.world.nrb), which is the file that can be edited in the Scene Editor." );
  comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.level.*" );

  Reflect::Field* fieldNearClipDist = comp.AddField( &SceneAsset::m_NearClipDist, "m_NearClipDist" );
  Reflect::Field* fieldFarClipDist = comp.AddField( &SceneAsset::m_FarClipDist, "m_FarClipDist" );
  Reflect::Field* fieldSpatialGridSize = comp.AddField( &SceneAsset::m_SpatialGridSize, "m_SpatialGridSize" );
  Reflect::Field* fieldOcclTestDownwardColl = comp.AddField( &SceneAsset::m_OcclTestDownwardColl, "m_OcclTestDownwardColl" );
  Reflect::Field* fieldOcclTestDownwardVis = comp.AddField( &SceneAsset::m_OcclTestDownwardVis, "m_OcclTestDownwardVis" );
  Reflect::Field* fieldOcclVisDistAdjust = comp.AddField( &SceneAsset::m_OcclVisDistAdjust, "m_OcclVisDistAdjust" );
  Reflect::Field* fieldDecalGeomMem = comp.AddField( &SceneAsset::m_DecalGeomMem, "m_DecalGeomMem" );
  Reflect::Field* fieldZones = comp.AddField( &SceneAsset::m_Zones, "m_Zones" );
}


bool SceneAsset::ProcessComponent(Reflect::ElementPtr element, const tstring& fieldName)
{
  return __super::ProcessComponent( element, fieldName );
}

bool SceneAsset::ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const
{
  if ( attr->HasType( Reflect::GetType<DependenciesComponent>() ) )
  {
    return true;
  }

  return __super::ValidateCompatible( attr, error );
}
