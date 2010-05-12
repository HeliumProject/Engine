#include "CinematicSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  //input
  const FileSpec Cinematic::GLOBAL_CONFIG_FILE               ( "Cinematic::GLOBAL_CONFIG_FILE",               "globalconfig",         Extension::DATA, "1 - johnnyb" );
  const FileSpec Cinematic::CONFIG_FILE                      ( "Cinematic::CONFIG_FILE",                      "config",               Extension::DATA, "2 - johnnyb" );
  const FileSpec Cinematic::RTLIGHT_FILE                     ( "Cinematic::RTLIGHT_FILE",                     "rtlights",             Extension::DATA, "1 - johnnyb" );
  const FileSpec Cinematic::EVENTS_FILE                      ( "Cinematic::EVENTS_FILE",                      "",                     "1 - johnnyb" );
  const SuffixSpec Cinematic::EVENTS_SUFFIX                  ( "Cinematic::EVENTS_SUFFIX",                    "_events" );
  const DecorationSpec Cinematic::EVENTS_DECORATION          ( "Cinematic::EVENTS_DECORATION",                "Cinematic Envets File", EVENTS_SUFFIX,          Extension::REFLECT_BINARY );

  //output
  // NOTE: changing this spec probably requires that you change Animation::ANIMCLIPS_FILE as well
  const FileSpec Cinematic::BUILT_FILE                       ( "Cinematic::BUILT_FILE",         "cinematic",            Extension::DATA, "56 - phaile" );
  const FileSpec Cinematic::DEBUG_FILE                       ( "Cinematic::DEBUG_FILE",         "debug",                Extension::DATA, "10 - johnnyb" );
  
}