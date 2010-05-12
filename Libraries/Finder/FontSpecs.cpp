#include "FontSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const ExtensionSpec Font::OPEN_TYPE_EXTENSION( "Font::OPEN_TYPE_EXTENSION", "Open Type Font File",  "otf" );
  const ExtensionSpec Font::TRUE_TYPE_EXTENSION( "Font::TRUE_TYPE_EXTENSION", "True Type Font File",  "ttf" );
  const ExtensionSpec Font::TRUE_TYPE_COLLECTION_EXTENSION( "Font::TRUE_TYPE_COLLECTION_EXTENSION", "True Type Font Collection", "ttc" );
  
  const FileSpec Font::OPEN_TYPE_FILE( "Font::OPEN_TYPE_FILE", "" );
  const FileSpec Font::TRUE_TYPE_FILE( "Font::TRUE_TYPE_FILE", "" );
  const FileSpec Font::TRUE_TYPE_COLLECTION_FILE( "Font::TRUE_TYPE_COLLECTION_FILE", "" );
  const FileSpec Font::CHARACTER_SET_FILE( "Font::CHARACTER_SET_FILE", "" );
  const FileSpec Font::METRICS_FILE( "Font::METRICS_FILE", "font", Extension::DATA, "8 - cedwards fix ProcessMip" );
  const FileSpec Font::TEXTURE_DATA_FILE( "Font::TEXTURE_DATA_FILE", "font_texel", Extension::DATA, "8 - cedwards fix ProcessMip" );
  const FileSpec Font::TEXTURE_HEADER_FILE( "Font::TEXTURE_HEADER_FILE", "font_header", Extension::DATA, "8 - cedwards fix ProcessMip" );
  
  const FileSpec Font::TEMP_TEXTURE_FILE( "Font::TEMP_TEXTURE_FILE", "font_page_%02d", Extension::TGA );

  FilterSpec Font::FONT_FILTER( "Font::FONT_FILTER", "Font Files" );

  void Font::Init()
  {
    FONT_FILTER.AddSpec( OPEN_TYPE_EXTENSION );
    FONT_FILTER.AddSpec( TRUE_TYPE_EXTENSION );
    FONT_FILTER.AddSpec( TRUE_TYPE_COLLECTION_EXTENSION );
  }
}