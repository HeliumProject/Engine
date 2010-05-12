#include "CubeMapSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{
  const FileSpec CubeMap::BUILT_FILE     ( "CubeMap::BUILT_FILE",       "built",                     Extension::DATA,            "20 - mlee" );
  const FileSpec CubeMap::TEXTURE_FILE   ( "CubeMap::TEXTURE_FILE",     "",                          Extension::HDR,             "8 - ahastings" );

  const FolderSpec CubeMap::DATA_FOLDER  ( "CubeMap::DATA_FOLDER",      "cubemaps",                  FolderRoots::ProjectAssets );
}
