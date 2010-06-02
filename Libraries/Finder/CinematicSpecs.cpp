#include "CinematicSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
    //input

    //output
    // NOTE: changing this spec probably requires that you change Animation::ANIMCLIPS_FILE as well
    const FileSpec Cinematic::BUILT_FILE                       ( "Cinematic::BUILT_FILE",         "cinematic",            Extension::DATA, "56 - phaile" );
    const FileSpec Cinematic::DEBUG_FILE                       ( "Cinematic::DEBUG_FILE",         "debug",                Extension::DATA, "10 - johnnyb" );

}