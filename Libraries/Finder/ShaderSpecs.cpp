#include "ShaderSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{

  //
  //  suffixes
  //

  const SuffixSpec Shader::LAMBERT_SUFFIX           ( "Shader::LAMBERT_SUFFIX",           "_lambert" );
  const SuffixSpec Shader::BLINN_SUFFIX             ( "Shader::BLINN_SUFFIX",             "_blinn" );

  const SuffixSpec Shader::TEXTURE_NODE_SUFFIX      ( "Shader::TEXTURE_NODE_SUFFIX",      "_texturenode" );
  const SuffixSpec Shader::NORMAL_MAP_NODE_SUFFIX   ( "Shader::NORMAL_MAP_NODE_SUFFIX",   "_normalmapnode" );

  const SuffixSpec Shader::NORMAL_MAP_SUFFIX        ( "Shader::NORMAL_MAP_SUFFIX",        "_normal" );
  const SuffixSpec Shader::COLOR_MAP_SUFFIX         ( "Shader::COLOR_MAP_SUFFIX",         "_color" );
  const SuffixSpec Shader::EXPENSIVE_MAP_SUFFIX     ( "Shader::EXPENSIVE_MAP_SUFFIX",     "_expensive" );


  //
  //  files
  //
  const FileSpec Shader::CACHE_FILE       ( "Shader::CACHE_FILE",       "shadercache",               Extension::DATA,            "5 - mlee" );

  const FileSpec Shader::BUILT_FILE                     ( "Shader::BUILT_FILE",                     "built",          Extension::DATA,            "77 - mlee");

  const FileSpec Shader::TEX_RAW_FILE     ( "Shader::TEX_RAW_FILE",     "texture",                   Extension::DATA,            "28 - cedwards fix ProcessMip" );
  const FileSpec Shader::TEXTURE_FILE     ( "Shader::TEXTURE_FILE",     "",                          Extension::TGA,             "5 - mlee" );
}
