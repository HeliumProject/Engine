#include "TexturePackSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{
  const FileSpec TexturePack::TEXEL_FILE       ( "TexturePack::TEXEL_FILE",      "texel",                     Extension::DATA,            "4 - cedwards fix ProcessMip" );
  const FileSpec TexturePack::HEADER_FILE      ( "TexturePack::HEADER_FILE",     "header",                    Extension::DATA,            "3 - cedwards fix ProcessMip" );
  const FileSpec TexturePack::DEBUG_FILE       ( "TexturePack::DEBUG_FILE",      "debug",                     Extension::TXT,             "2 - cedwards fix ProcessMip" );
  const FileSpec TexturePack::COMBINED_FILE    ( "TexturePack::COMBINED_FILE",   "combined",                  Extension::DATA,            "4 - jonny" );

  const FolderSpec TexturePack::DATA_FOLDER    ( "TexturePack::DATA_FOLDER",     "texturepacks",              FolderRoots::ProjectAssets );
}
