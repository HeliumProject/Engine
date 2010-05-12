#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Sound
  {
  public:

    //
    //  folders
    //

    const static FolderSpec BASE_FOLDER;
    const static FolderSpec PRIVATE_FOLDER;
    const static FolderSpec CONFIG_FOLDER;
    const static FolderSpec GLOBAL_FOLDER;
    const static FolderSpec DIALOG_FOLDER; 
    const static FolderSpec MOVIE_FOLDER;
    const static FolderSpec REALTIME_FOLDER;
    const static FolderSpec SOUND_FOLDER;
    const static FolderSpec DIALOGUE_FOLDER;
    const static FolderSpec BUILT_SOUND_FOLDER; 

    //
    //  suffixes
    //
    
    const static SuffixSpec SOUND_SUFFIX;
    const static SuffixSpec DIALOGUE_SUFFIX;
    const static SuffixSpec SCREAM_SUFFIX;
    const static SuffixSpec SND_STREAM_SUFFIX;
    const static SuffixSpec THEME_A_SUFFIX;
    const static SuffixSpec THEME_B_SUFFIX;

    //
    //  decoration
    //

    const static DecorationSpec SCREAM_DECORATION;
    const static DecorationSpec SND_STREAM_DECORATION;

    //
    //  files
    //

    const static FileSpec SCREAM_SFX_FILE;
    const static FileSpec SCREAM_BANK_FILE;
    const static FileSpec BANKMERGE_FILE;
    const static FileSpec RESIDENT_FILE;    
    const static FileSpec STREAMING_FILE;
    const static FileSpec VAG_FILE;
    const static FileSpec XVAG_FILE;
    const static FileSpec MP3_FILE;
    const static FileSpec WAV_FILE;
    const static FileSpec THEME_A_FILE;
    const static FileSpec THEME_B_FILE;
    const static FileSpec GENERATED_DIALOGUE_FILE;

    const static FileSpec CONFIG_FILE;
    const static FileSpec LEVEL_CONFIG_FILE;
    const static FileSpec NONLOC_CONFIG_FILE;
    const static FileSpec NULL_SFX_FILE;
    const static FileSpec NULL_DIALOG_FILE;
    const static FileSpec DIALOG_GUIDS_FILE;
  };

}

