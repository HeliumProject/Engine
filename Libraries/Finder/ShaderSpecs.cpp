#include "ShaderSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{

  //
  //  suffixes
  //

  const SuffixSpec Shader::LAMBERT_SUFFIX           ( "Shader::LAMBERT_SUFFIX",           "_lam" );
  const SuffixSpec Shader::BLINN_SUFFIX             ( "Shader::BLINN_SUFFIX",             "_bli" );

  const SuffixSpec Shader::TEXTURE_NODE_SUFFIX      ( "Shader::TEXTURE_NODE_SUFFIX",      "_tex" );
  const SuffixSpec Shader::NORMAL_MAP_NODE_SUFFIX   ( "Shader::NORMAL_MAP_NODE_SUFFIX",   "_nrml" );

  const SuffixSpec Shader::NORMAL_MAP_SUFFIX        ( "Shader::NORMAL_MAP_SUFFIX",        "_n" );
  const SuffixSpec Shader::COLOR_MAP_SUFFIX         ( "Shader::COLOR_MAP_SUFFIX",         "_c" );
  const SuffixSpec Shader::EXPENSIVE_MAP_SUFFIX     ( "Shader::EXPENSIVE_MAP_SUFFIX",     "_e" );


  //
  //  files
  //
  const FileSpec Shader::CACHE_FILE       ( "Shader::CACHE_FILE",       "shadercache",               Extension::DATA,            "5 - mlee" );

  const FileSpec Shader::BUILT_FILE                     ( "Shader::BUILT_FILE",                     "built",          Extension::DATA,            "77 - mlee");

  // file specs for the various custom .shader.irb files
  const FileSpec Shader::WATER_BUILT_FILE               ( "Shader::WATER_BUILT_FILE",               "built",          Extension::DATA,            "22 - mlee" );
  const FileSpec Shader::FUR_BUILT_FILE                 ( "Shader::FUR_BUILT_FILE",                 "built",          Extension::DATA,            "17 - mlee" );
  const FileSpec Shader::REFRACTION_BUILT_FILE          ( "Shader::REFRACTION_BUILT_FILE",          "built",          Extension::DATA,            "17 - mlee" );
  const FileSpec Shader::GROUNDFOG_BUILT_FILE           ( "Shader::GROUNDFOG_BUILT_FILE",           "built",          Extension::DATA,            "17 - mlee" );
  const FileSpec Shader::FURFRAGMENT_BUILT_FILE         ( "Shader::FURFRAGMENT_BUILT_FILE",         "built",          Extension::DATA,            "18 - mlee" );
  const FileSpec Shader::ANISOTROPIC_BUILT_FILE         ( "Shader::ANISOTROPIC_BUILT_FILE",         "built",          Extension::DATA,            "23 - mlee" );
  const FileSpec Shader::AUDIOVIS_BUILT_FILE            ( "Shader::AUDIOVIS_BUILT_FILE",            "built",          Extension::DATA,            "18 - mlee" );
  const FileSpec Shader::OFF_SCREEN_BUILT_FILE          ( "Shader::OFF_SCREEN_BUILT_FILE",          "built",          Extension::DATA,            "19 - mlee" );
  const FileSpec Shader::BRDF_BUILT_FILE                ( "Shader::BRDF_BUILT_FILE",                "built",          Extension::DATA,            "22 - mlee" );
  const FileSpec Shader::FOLIAGE_SHADER_BUILT_FILE      ( "Shader::FOLIAGE_SHADER_BUILT_FILE",      "built",          Extension::DATA,            "26 - mlee" );
  const FileSpec Shader::GRAPH_SHADER_BUILT_FILE        ( "Shader::GRAPH_SHADER_BUILT_FILE",        "built",          Extension::DATA,            "34 - abezrati" );

  // built file for .graphshaders
  const FileSpec Shader::GRAPH_SHADER_GRAPH_BUILT_FILE  ( "Shader::GRAPH_SHADER_GRAPH_BUILT_FILE",  "graphshader",    Extension::DATA,            "18 - abezrati" );

  const FileSpec Shader::TEX_RAW_FILE     ( "Shader::TEX_RAW_FILE",     "texture",                   Extension::DATA,            "28 - cedwards fix ProcessMip" );
  const FileSpec Shader::MENTALRAY_FILE   ( "Shader::MENTALRAY_FILE",   "shader",                    Extension::MENTALRAY,       "28 - drr" );
  const FileSpec Shader::TEXTURE_FILE     ( "Shader::TEXTURE_FILE",     "",                          Extension::TGA,             "5 - mlee" );
  const FileSpec Shader::MENTALRAY_COLOR_TEX( "Shader::MENTALRAY_COLOR_TEX",   "colormap",       Extension::TGA,             "2 - drr" );

  const FolderSpec Shader::DATA_FOLDER                  ( "Shader::DATA_FOLDER",                    "shaders",        FolderRoots::ProjectAssets );
}
