#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Sound
  {
  public:

    //
    //  suffixes
    //
    
    const static SuffixSpec SOUND_SUFFIX;
    const static SuffixSpec DIALOGUE_SUFFIX;

    //
    //  files
    //
    const static FileSpec RESIDENT_FILE;    
    const static FileSpec STREAMING_FILE;
    const static FileSpec VAG_FILE;
    const static FileSpec XVAG_FILE;
    const static FileSpec MP3_FILE;
    const static FileSpec WAV_FILE;
    const static FileSpec GENERATED_DIALOGUE_FILE;

    const static FileSpec CONFIG_FILE;
    const static FileSpec LEVEL_CONFIG_FILE;
    const static FileSpec NONLOC_CONFIG_FILE;
    const static FileSpec NULL_SFX_FILE;
    const static FileSpec NULL_DIALOG_FILE;
    const static FileSpec DIALOG_GUIDS_FILE;
  };

}

