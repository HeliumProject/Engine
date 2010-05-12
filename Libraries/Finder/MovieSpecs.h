#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Movie
  {
  public:
    const static ExtensionSpec AUDIO_EXTENSION;
    const static ExtensionSpec BINK_EXTENSION;
    const static ExtensionSpec SUBTITLE_EXTENSION;
    
    const static FolderSpec BUILT_FOLDER;
    const static FolderSpec AUDIO_FOLDER;
    const static FolderSpec VIDEO_FOLDER;
    
    const static FileSpec GENERIC_INPUT_FILE;
    const static FileSpec GENERIC_OUTPUT_FILE;
  };
}
