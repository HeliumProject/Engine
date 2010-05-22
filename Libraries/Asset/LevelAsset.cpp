#include "LevelAsset.h"

#include "AssetTemplate.h"
#include "DependenciesAttribute.h"
#include "WorldFileAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "Finder/ExtensionSpecs.h"
#include "AllowedDirParser.h"

using namespace Asset;

extern AllowedDirParser g_AllowedDirParser;

REFLECT_DEFINE_CLASS( LevelAsset );

void LevelAsset::EnumerateClass( Reflect::Compositor<LevelAsset>& comp )
{
  comp.GetComposite().m_UIName = "Level";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A level groups together various zones to make a level in the game.  The level asset will be associated with a world file (*.world.rb), which is the file that can be edited in the Scene Editor." );
  comp.GetComposite().SetProperty( AssetProperties::ModifierSpec, FinderSpecs::Asset::LEVEL_DECORATION.GetName() );
  comp.GetComposite().SetProperty( AssetProperties::RootFolderSpec, FinderSpecs::Asset::LEVEL_FOLDER.GetName() );

  Reflect::Field* fieldNearClipDist = comp.AddField( &LevelAsset::m_NearClipDist, "m_NearClipDist" );
  Reflect::Field* fieldFarClipDist = comp.AddField( &LevelAsset::m_FarClipDist, "m_FarClipDist" );
  Reflect::Field* fieldSpatialGridSize = comp.AddField( &LevelAsset::m_SpatialGridSize, "m_SpatialGridSize" );
  Reflect::Field* fieldIsLevelFoliageHeavy = comp.AddField( &LevelAsset::m_IsLevelFoliageHeavy, "m_IsLevelFoliageHeavy" );
  Reflect::Field* fieldOcclTestDownwardColl = comp.AddField( &LevelAsset::m_OcclTestDownwardColl, "m_OcclTestDownwardColl" );
  Reflect::Field* fieldOcclTestDownwardVis = comp.AddField( &LevelAsset::m_OcclTestDownwardVis, "m_OcclTestDownwardVis" );
  Reflect::Field* fieldOcclVisDistAdjust = comp.AddField( &LevelAsset::m_OcclVisDistAdjust, "m_OcclVisDistAdjust" );
  Reflect::Field* fieldMultiplayer = comp.AddField( &LevelAsset::m_Multiplayer, "m_Multiplayer" );
  Reflect::Field* fieldAutoBuildDefaultRegion = comp.AddField( &LevelAsset::m_AutoBuildDefaultRegion, "m_EnableDefaultRegion" );
  Reflect::Field* fieldIncludeGlobalReqs = comp.AddField( &LevelAsset::m_IncludeGlobalReqs, "m_IncludeGlobalReqs" );

  Reflect::Field* fieldDecalGeomMem = comp.AddField( &LevelAsset::m_DecalGeomMem, "m_DecalGeomMem" );

  Reflect::Field* fieldViewerStartingPosition = comp.AddField( &LevelAsset::m_ViewerStartingPosition, "m_ViewerStartingPosition" );
  Reflect::Field* fieldViewerStartingRotation = comp.AddField( &LevelAsset::m_ViewerStartingRotation, "m_ViewerStartingRotation" );

  // asset creation template
  Reflect::V_Element assetTemplates;

  AssetTemplatePtr classTemplate = new AssetTemplate( &comp.GetComposite() );

  classTemplate->m_DefaultAddSubDir = true;
  classTemplate->m_ShowSubDirCheckbox = false;
  classTemplate->m_AboutDirSettings = g_AllowedDirParser.GetAboutDirSettings( classTemplate->m_Name );
  classTemplate->m_DefaultRoot = FinderSpecs::Asset::LEVEL_FOLDER.GetRelativeFolder();
  classTemplate->m_DirectoryPatterns = g_AllowedDirParser.GetPatterns( classTemplate->m_Name );

  classTemplate->AddRequiredAttribute( Reflect::GetType< Asset::WorldFileAttribute >() );
  classTemplate->AddOptionalAttribute( Reflect::GetType< Asset::DependenciesAttribute >() );
  assetTemplates.push_back( classTemplate );

  std::stringstream stream;
  Reflect::Archive::ToStream( assetTemplates, stream, Reflect::ArchiveTypes::Binary );
  comp.GetComposite().SetProperty( AssetProperties::AssetTemplates, stream.str() );
}


bool LevelAsset::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
{
  return __super::ProcessComponent( element, fieldName );
}

bool LevelAsset::ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const
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

void LevelAsset::MakeDefault()
{
  Clear();

  WorldFileAttributePtr worldFile = new WorldFileAttribute();

  SetAttribute( worldFile );
}

bool LevelAsset::IsBuildable() const
{
  return true;
}

bool LevelAsset::IsViewable() const
{
  return true;
}
