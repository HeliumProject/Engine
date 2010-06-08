#include "SceneAsset.h"

#include "AssetTemplate.h"
#include "DependenciesAttribute.h"
#include "WorldFileAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "Finder/ExtensionSpecs.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( SceneAsset );

void SceneAsset::EnumerateClass( Reflect::Compositor<SceneAsset>& comp )
{
  comp.GetComposite().m_UIName = "Level";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A level groups together various zones to make a level in the game.  The level asset will be associated with a world file (*.world.rb), which is the file that can be edited in the Scene Editor." );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::LEVEL_DECORATION.GetName() );

  Reflect::Field* fieldNearClipDist = comp.AddField( &SceneAsset::m_NearClipDist, "m_NearClipDist" );
  Reflect::Field* fieldFarClipDist = comp.AddField( &SceneAsset::m_FarClipDist, "m_FarClipDist" );
  Reflect::Field* fieldSpatialGridSize = comp.AddField( &SceneAsset::m_SpatialGridSize, "m_SpatialGridSize" );
  Reflect::Field* fieldOcclTestDownwardColl = comp.AddField( &SceneAsset::m_OcclTestDownwardColl, "m_OcclTestDownwardColl" );
  Reflect::Field* fieldOcclTestDownwardVis = comp.AddField( &SceneAsset::m_OcclTestDownwardVis, "m_OcclTestDownwardVis" );
  Reflect::Field* fieldOcclVisDistAdjust = comp.AddField( &SceneAsset::m_OcclVisDistAdjust, "m_OcclVisDistAdjust" );
  Reflect::Field* fieldDecalGeomMem = comp.AddField( &SceneAsset::m_DecalGeomMem, "m_DecalGeomMem" );

  // asset creation template
  Reflect::V_Element assetTemplates;

  AssetTemplatePtr classTemplate = new AssetTemplate( &comp.GetComposite() );

  classTemplate->m_DefaultAddSubDir = true;
  classTemplate->m_ShowSubDirCheckbox = false;
  
  classTemplate->AddRequiredAttribute( Reflect::GetType< Asset::WorldFileAttribute >() );
  classTemplate->AddOptionalAttribute( Reflect::GetType< Asset::DependenciesAttribute >() );
  assetTemplates.push_back( classTemplate );

  std::stringstream stream;
  Reflect::Archive::ToStream( assetTemplates, stream, Reflect::ArchiveTypes::Binary );
  comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}


bool SceneAsset::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
{
  return __super::ProcessComponent( element, fieldName );
}

bool SceneAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
{
  if ( attr->HasType( Reflect::GetType<DependenciesAttribute>() ) )
  {
    return true;
  }
  else if ( attr->HasType( Reflect::GetType<WorldFileAttribute>() ) )
  {
    return true;
  }

  return __super::ValidateCompatible( attr, error );
}

void SceneAsset::MakeDefault()
{
  Clear();

  WorldFileAttributePtr worldFile = new WorldFileAttribute();

  SetAttribute( worldFile );
}

bool SceneAsset::IsBuildable() const
{
  return true;
}

bool SceneAsset::IsViewable() const
{
  return true;
}
