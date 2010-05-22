#include "BuilderOptions.h"

using namespace Reflect;

namespace AssetBuilder
{
  REFLECT_DEFINE_CLASS( BuilderOptions );

void BuilderOptions::EnumerateClass( Reflect::Compositor<BuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Base Builder Options";

  Reflect::Field* fieldSkipNested = comp.AddField( &BuilderOptions::m_SkipNested, "m_SkipNested" );
  fieldSkipNested->m_UIName = "Skip Nested";
}

  REFLECT_DEFINE_CLASS( BuildRequest );

void BuildRequest::EnumerateClass( Reflect::Compositor<BuildRequest>& comp )
{
  Reflect::Field* fieldAssets = comp.AddField( &BuildRequest::m_Assets, "m_Assets" );
  Reflect::ElementField* elemOptions = comp.AddField( &BuildRequest::m_Options, "m_Options" );
}

  REFLECT_DEFINE_CLASS( ZoneBuilderOptions );

void ZoneBuilderOptions::EnumerateClass( Reflect::Compositor<ZoneBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Zone Builder Options";
  Reflect::ElementField* elemZone = comp.AddField( &ZoneBuilderOptions::m_Zone, "m_Zone" );
  Reflect::Field* fieldGameplayOnly = comp.AddField( &ZoneBuilderOptions::m_GameplayOnly, "m_GameplayOnly" );
}

  REFLECT_DEFINE_CLASS( LevelBuilderOptions );

void LevelBuilderOptions::EnumerateClass( Reflect::Compositor<LevelBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Level Builder Options";

  Reflect::Field* fieldNoMobys = comp.AddField( &LevelBuilderOptions::m_NoMobys, "m_NoMobys" );
  Reflect::Field* fieldNoConduit = comp.AddField( &LevelBuilderOptions::m_NoConduit, "m_NoConduit" );
  Reflect::Field* fieldNoCinematics = comp.AddField( &LevelBuilderOptions::m_NoCinematics, "m_NoCinematics" );
  Reflect::Field* fieldNoFonts = comp.AddField( &LevelBuilderOptions::m_NoFonts, "m_NoFonts" );
  Reflect::Field* fieldNoTexStream = comp.AddField( &LevelBuilderOptions::m_NoTexStream, "m_NoTexStream" );
  Reflect::Field* fieldNoNewNav = comp.AddField( &LevelBuilderOptions::m_NoNewNav, "m_NoNewNav" );
  Reflect::Field* fieldNoReqs = comp.AddField( &LevelBuilderOptions::m_NoReqs, "m_NoReqs", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldNoWad = comp.AddField( &LevelBuilderOptions::m_NoWad, "m_NoWad", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldNoLocalization = comp.AddField( &LevelBuilderOptions::m_NoLocalization, "m_NoLocalization", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldViewer = comp.AddField( &LevelBuilderOptions::m_Viewer, "m_Viewer", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldLightmapTweak = comp.AddField( &LevelBuilderOptions::m_LightmapTweak, "m_LightmapTweak" );
  Reflect::Field* fieldLightmapHigh = comp.AddField( &LevelBuilderOptions::m_LightmapHigh, "m_LightmapHigh" );

  Reflect::Field* fieldOcclusion = comp.AddField( &LevelBuilderOptions::m_Occlusion, "m_Occlusion" );
  Reflect::Field* fieldCollPatch = comp.AddField( &LevelBuilderOptions::m_CollPatch, "m_CollPatch", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldPackOnly = comp.AddField( &LevelBuilderOptions::m_PackOnly, "m_PackOnly", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldBuildOnly = comp.AddField( &LevelBuilderOptions::m_BuildOnly, "m_BuildOnly", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldSelectZones = comp.AddField( &LevelBuilderOptions::m_SelectZones, "m_SelectZones" );
  fieldSelectZones->m_UIName = "Select Specific Zones";

  Reflect::Field* fieldLanguage = comp.AddField( &LevelBuilderOptions::m_Language, "m_Language" );
  fieldLanguage->m_UIName = "Select Language";

  Reflect::Field* fieldZones = comp.AddField( &LevelBuilderOptions::m_Zones, "m_Zones", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldZoneList = comp.AddField( &LevelBuilderOptions::m_ZoneList, "m_ZoneList", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldRegions = comp.AddField( &LevelBuilderOptions::m_Regions, "m_Regions", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldGameplayOnly = comp.AddField( &LevelBuilderOptions::m_GameplayOnly, "m_GameplayOnly", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldPackAllRegions = comp.AddField( &LevelBuilderOptions::m_PackAllRegions, "m_PackAllRegions", Reflect::FieldFlags::Hide );

  Reflect::Field* fieldLightmapTweakSubset = comp.AddField( &LevelBuilderOptions::m_LightmapTweakSubset, "m_LightmapTweakSubset" );
}

  REFLECT_DEFINE_CLASS( RegionBuilderOptions );

void RegionBuilderOptions::EnumerateClass( Reflect::Compositor<RegionBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Zone Builder Options";
  Reflect::Field* fieldRegionName = comp.AddField( &RegionBuilderOptions::m_RegionName, "m_RegionName" );
  Reflect::ElementField* elemRegion = comp.AddField( &RegionBuilderOptions::m_Region, "m_Region" );
  Reflect::Field* fieldZones = comp.AddField( &RegionBuilderOptions::m_Zones, "m_Zones" );
  Reflect::ElementField* elemLevelBuilderOptions = comp.AddField( &RegionBuilderOptions::m_LevelBuilderOptions, "m_LevelBuilderOptions" );
}

  REFLECT_DEFINE_CLASS( ShrubBuilderOptions );

void ShrubBuilderOptions::EnumerateClass( Reflect::Compositor<ShrubBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Shrub Builder Options";
}

  REFLECT_DEFINE_CLASS( TieBuilderOptions );

void TieBuilderOptions::EnumerateClass( Reflect::Compositor<TieBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Tie Builder Options";
}

  REFLECT_DEFINE_CLASS( UfragBuilderOptions );

void UfragBuilderOptions::EnumerateClass( Reflect::Compositor<UfragBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Ufrag Builder Options";
}

  REFLECT_DEFINE_CLASS( ShaderBuilderOptions );

void ShaderBuilderOptions::EnumerateClass( Reflect::Compositor<ShaderBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Shader Builder Options";

  Reflect::Field* fieldDumpPath = comp.AddField( &ShaderBuilderOptions::m_DumpPath, "m_DumpPath" );
  fieldDumpPath->m_UIName = "Dump Path";

  Reflect::Field* fieldVerbose = comp.AddField( &ShaderBuilderOptions::m_Verbose, "m_Verbose" );
  fieldVerbose->m_UIName = "Verbose";
}

  REFLECT_DEFINE_CLASS( FoliageBuilderOptions );

void FoliageBuilderOptions::EnumerateClass( Reflect::Compositor<FoliageBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Foliage Builder Options";
}

  REFLECT_DEFINE_CLASS( FontBuilderOptions );

void FontBuilderOptions::EnumerateClass( Reflect::Compositor<FontBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Font Builder Options";
}

  REFLECT_DEFINE_CLASS( MobyBuilderOptions );

void MobyBuilderOptions::EnumerateClass( Reflect::Compositor<MobyBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Moby Builder Options";

  Reflect::Field* fieldConduit = comp.AddField( &MobyBuilderOptions::m_Conduit, "m_Conduit" );
  Reflect::Field* fieldDumpJoints = comp.AddField( &MobyBuilderOptions::m_DumpJoints, "m_DumpJoints" );
}

  REFLECT_DEFINE_CLASS( MovieBuilderOptions );

void MovieBuilderOptions::EnumerateClass( Reflect::Compositor<MovieBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Movie Builder Options";
}

  REFLECT_DEFINE_CLASS( AnimationBuilderOptions );

void AnimationBuilderOptions::EnumerateClass( Reflect::Compositor<AnimationBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Animation Builder Options";

  Reflect::Field* fieldDumpAnims = comp.AddField( &AnimationBuilderOptions::m_DumpAnims, "m_DumpAnims" );
  Reflect::Field* fieldDumpJoints = comp.AddField( &AnimationBuilderOptions::m_DumpJoints, "m_DumpJoints" );
}

  REFLECT_DEFINE_CLASS( SkyBuilderOptions );

void SkyBuilderOptions::EnumerateClass( Reflect::Compositor<SkyBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Sky Builder Options";
}

  REFLECT_DEFINE_CLASS( CinematicBuilderOptions );

void CinematicBuilderOptions::EnumerateClass( Reflect::Compositor<CinematicBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Cinematic Builder Options";
}

  REFLECT_DEFINE_CLASS( CubeMapBuilderOptions );

void CubeMapBuilderOptions::EnumerateClass( Reflect::Compositor<CubeMapBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "CubeMap Builder Options";
}

  REFLECT_DEFINE_CLASS( TexturePackBuilderOptions );

void TexturePackBuilderOptions::EnumerateClass( Reflect::Compositor<TexturePackBuilderOptions>& comp )
{
  comp.GetComposite().m_UIName = "Texture Pack Builder Options";
}

  REFLECT_DEFINE_CLASS( SymbolEnumAutoFixupOptions );

void SymbolEnumAutoFixupOptions::EnumerateClass( Reflect::Compositor<SymbolEnumAutoFixupOptions>& comp )
{
  Reflect::Field* fieldInputFile = comp.AddField( &SymbolEnumAutoFixupOptions::m_InputFile, "m_InputFile" );
  Reflect::Field* fieldInputFileSpecName = comp.AddField( &SymbolEnumAutoFixupOptions::m_InputFileSpecName, "m_InputFileSpecName" );

  Reflect::Field* fieldManifestFile = comp.AddField( &SymbolEnumAutoFixupOptions::m_ManifestFile, "m_ManifestFile" );
  Reflect::Field* fieldManifestFileSpecName = comp.AddField( &SymbolEnumAutoFixupOptions::m_ManifestFileSpecName, "m_ManifestFileSpecName" );
  
  Reflect::Field* fieldOutputFile = comp.AddField( &SymbolEnumAutoFixupOptions::m_OutputFile, "m_OutputFile" );
}

}