#include "ZoneSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Zone::BUILT_FILE             ( "Zone::BUILT_FILE",             "built",        Extension::DATA,           "151 - abezrati" );
  const FileSpec Zone::HELPER_FILE            ( "Zone::HELPER_FILE",            "helper",       Extension::DATA,           "45 - mlee" );
  const FileSpec Zone::LIGHTMAPS_SET1_FILE    ( "Zone::LIGHTMAPS_SET1_FILE",    "lightmapset1", Extension::DATA,           "25 - mlee" );
  const FileSpec Zone::LIGHTMAPS_SET2_FILE    ( "Zone::LIGHTMAPS_SET2_FILE",    "lightmapset2", Extension::DATA,           "22 - mlee" );
  const FileSpec Zone::VERTLIGHTING_FILE      ( "Zone::VERTLIGHTING_FILE",      "vertlighting", Extension::DATA,           "10 - mlee" );
  const FileSpec Zone::LIGHTING_FILE          ( "Zone::LIGHTING_FILE",          "lighting",     Extension::DATA,           "41 - mlee" );
  const FileSpec Zone::LIGHTING_HIGH_FILE     ( "Zone::LIGHTING_HIGH_FILE",     "highlight",    Extension::DATA,           "41 - mlee" );
  const FileSpec Zone::CONDUIT_DEPS_FILE      ( "Zone::CONDUIT_DEPS_FILE",      "conduit_deps", Extension::REFLECT_BINARY, "4 - geoff" );
  const FileSpec Zone::GAMEPLAY_FILE          ( "Zone::GAMEPLAY_FILE",          "gameplay",     Extension::DATA,           "27 - jvalenzu" );
  const FileSpec Zone::GP_SCENE_FILE          ( "Zone::GP_SCENE_FILE",          "gpscene",      Extension::REFLECT_BINARY, "14 - rsmith" );
  const FileSpec Zone::PLACED_DECALS_FILE     ( "Zone::PLACED_DECALS_FILE",     "decals",       Extension::DATA,           "1 - ahastings" );
  const FileSpec Zone::MOBY_LIST_FILE         ( "Zone::MOBY_LIST_FILE",         "moby_list",    Extension::DATA,           "4 - gveltri" );
  const FileSpec Zone::SHADER_LIST_FILE       ( "Zone::SHADER_LIST_FILE",       "shader_list",  Extension::DATA,           "5 - cedwards" );
  const FileSpec Zone::CUBEMAP_LIST_FILE      ( "Zone::CUBEMAP_LIST_FILE",      "cubemap_list", Extension::DATA,           "4 - gveltri" );
  const FileSpec Zone::INSTANCE_LIGHTING_FILE ( "Zone::INSTANCE_LIGHTING_FILE", "lighting",     Extension::DATA,           "5 - dreagan" );
  const FileSpec Zone::SERVERSIDE_FILE        ( "Zone::SERVERSIDE_FILE",        "serverside",   Extension::DATA,           "1 - rsmith" );

#pragma TODO("This seems wrong, do we need \"DataSpecs?\"")
  const FileSpec Zone::LIGHTING_INPUT_DATA    ( "Zone::LIGHTING_INPUT_DATA",    "",   Extension::DATA,           "1 - drreagan" );
}
