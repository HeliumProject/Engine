#include "LevelAsset.h"

#include "AssetTemplate.h"
#include "DependenciesAttribute.h"
#include "WorldFileAttribute.h"

#include "Attribute/AttributeHandle.h"
#include "File/Manager.h"
#include "Finder/ExtensionSpecs.h"
#include "AllowedDirParser.h"

using namespace Asset;

extern AllowedDirParser g_AllowedDirParser;

REFLECT_DEFINE_CLASS( LevelAsset );

void LevelAsset::EnumerateClass( Reflect::Compositor<LevelAsset>& comp )
{
  comp.GetComposite().m_UIName = "Level";
  comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A level groups together various zones to make a level in the game.  The level asset will be associated with a world file (*.world.irb), which is the file that can be edited in the Scene Editor." );
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

  Reflect::Field* fieldSkyAssets = comp.AddField( &LevelAsset::m_SkyAssets, "m_SkyAssets", Reflect::FieldFlags::FileID | Asset::AssetFlags::ManageField | Asset::AssetFlags::PerformOperation );
  fieldSkyAssets->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Asset::SKY_DECORATION.GetName() );
  fieldSkyAssets->SetProperty( Asset::AssetProperties::SmallIcon, "enginetype_sky_16.png" );

  Reflect::Field* fieldDecalGeomMem = comp.AddField( &LevelAsset::m_DecalGeomMem, "m_DecalGeomMem" );

  Reflect::Field* fieldDefaultCubeMap = comp.AddField( &LevelAsset::m_DefaultCubeMap, "m_DefaultCubeMap", Reflect::FieldFlags::FileID | Asset::AssetFlags::Hierarchy | Asset::AssetFlags::ManageField | Asset::AssetFlags::PerformOperation );
  fieldDefaultCubeMap->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Asset::CUBEMAP_DECORATION.GetName() );
  fieldDefaultCubeMap->SetProperty( Asset::AssetProperties::SmallIcon, "enginetype_cubemap_16.png" );

  Reflect::Field* fieldWaterCubeMap = comp.AddField( &LevelAsset::m_WaterCubeMap, "m_WaterCubeMap", Reflect::FieldFlags::FileID | Asset::AssetFlags::Hierarchy | Asset::AssetFlags::ManageField | Asset::AssetFlags::PerformOperation );
  fieldWaterCubeMap->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Asset::CUBEMAP_DECORATION.GetName() );
  fieldWaterCubeMap->SetProperty( Asset::AssetProperties::SmallIcon, "water_cubemap_16.png" );

  Reflect::Field* fieldViewerStartingPosition = comp.AddField( &LevelAsset::m_ViewerStartingPosition, "m_ViewerStartingPosition" );
  Reflect::Field* fieldViewerStartingRotation = comp.AddField( &LevelAsset::m_ViewerStartingRotation, "m_ViewerStartingRotation" );

  Reflect::Field* fieldLightingZone = comp.AddField( &LevelAsset::m_LightingZone, "m_LightingZone", Reflect::FieldFlags::FileID );
  fieldLightingZone->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Asset::ZONE_DECORATION.GetName() );

  Reflect::Field* fieldCurveControl = comp.AddField( &LevelAsset::m_CurveControl, "m_CurveControl", Reflect::FieldFlags::FileID );
  fieldCurveControl->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Extension::ACV.GetName() );

  Reflect::Field* fieldCurveControl_CRT = comp.AddField( &LevelAsset::m_CurveControl_CRT, "m_CurveControl_CRT", Reflect::FieldFlags::FileID );
  fieldCurveControl_CRT->SetProperty( Asset::AssetProperties::ModifierSpec, FinderSpecs::Extension::ACV.GetName() );

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
  // Legacy support: if a single sky tuid was found, add it to the sky set
  if ( fieldName == "m_SkyAsset" )
  {
    tuid sky_asset;
    Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::U64Serializer>(element), sky_asset );

    m_SkyAssets.insert( sky_asset );

    return true;
  }

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

// levels are a special case (for now?)
std::string LevelAsset::GetBuiltDir() const
{
  std::string assetPath;
  File::GlobalManager().GetPath( m_AssetClassID, assetPath );

  return Finder::GetBuiltFolder( assetPath );
}

bool LevelAsset::IsBuildable() const
{
  return true;
}

bool LevelAsset::IsViewable() const
{
  return true;
}
