#include "MovieSpecs.h"
#include "SoundSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const ExtensionSpec Movie::AUDIO_EXTENSION    ( "Movie::AUDIO_EXTENSION",     "WAV File",     "wav" );
  const ExtensionSpec Movie::SUBTITLE_EXTENSION ( "Movie::SUBTITLE_EXTENSION",  "Package file", "pkg" );
  
  const FolderSpec Movie::BUILT_FOLDER          ( "Movie::BUILT_FOLDER",        "built/movies", FolderRoots::ProjectAssets );
  const FolderSpec Movie::AUDIO_FOLDER          ( "Movie::AUDIO_FOLDER",        "mpeg",         FinderSpecs::Sound::DIALOG_FOLDER );
  const FolderSpec Movie::VIDEO_FOLDER          ( "Movie::VIDEO_FOLDER",        "video",        FolderRoots::ProjectProcessed );
  
  const FileSpec Movie::GENERIC_INPUT_FILE      ( "Movie::GENERIC_INPUT_FILE",  "" );
  const FileSpec Movie::GENERIC_OUTPUT_FILE     ( "Movie::GENERIC_OUTPUT_FILE", "built",        Extension::BIK, "4_gveltri" );
}