#include "SoundSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "ProjectSpecs.h" 

namespace FinderSpecs
{

  //  
  //  suffixes
  //

  const SuffixSpec      Sound::SOUND_SUFFIX           ( "Sound::SOUND_SUFFIX",            "_sound" );
  const SuffixSpec      Sound::DIALOGUE_SUFFIX        ( "Sound::DIALOGUE_SUFFIX",         "_dialogue" );
    
  //
  //  files
  //
                                                                                        
  const FileSpec    Sound::RESIDENT_FILE              ( "Sound::RESIDENT_FILE",             "resident",                 Extension::DATA,            "21_dthall" );
  const FileSpec    Sound::STREAMING_FILE             ( "Sound::STREAMING_FILE",            "streaming",                Extension::DATA,            "21_dthall" );
  const FileSpec    Sound::VAG_FILE                   ( "Sound::VAG_FILE",                  "",                         Extension::VAG,             "13_dthall" );
  const FileSpec    Sound::XVAG_FILE                  ( "Sound::XVAG_FILE",                 "",                         Extension::XVAG,            "13_dthall" );
  const FileSpec    Sound::MP3_FILE                   ( "Sound::MP3_FILE",                  "",                         Extension::MP3,             "13_dthall" );
  const FileSpec    Sound::WAV_FILE                   ( "Sound::WAV_FILE",                  "",                         Extension::WAV,             "13_dthall" );
  const FileSpec    Sound::GENERATED_DIALOGUE_FILE    ( "Sound::GENERATED_DIALOGUE_FILE",   "",                         Extension::WAV,             "4_dthall");   
 
  const FileSpec    Sound::CONFIG_FILE                ( "Sound::CONFIG_FILE",               "sound_config",             Extension::XML,             "");
  const FileSpec    Sound::LEVEL_CONFIG_FILE          ( "Sound::LEVEL_CONFIG_FILE",         "level_config",             Extension::XML,             "");
  const FileSpec    Sound::NONLOC_CONFIG_FILE         ( "Sound::NONLOC_CONFIG_FILE",        "nonloc_config",            Extension::XML,             "");
  const FileSpec    Sound::NULL_SFX_FILE              ( "Sound::NULL_SFX_FILE",             "empty",                    Extension::SFX,             "");
  const FileSpec    Sound::NULL_DIALOG_FILE           ( "Sound::NULL_DIALOG_FILE",          "empty",                    Extension::WAV,             "");
  const FileSpec    Sound::DIALOG_GUIDS_FILE          ( "Sound::DIALOG_GUIDS_FILE",         "dialogue",                 Extension::TXT,             "");
}
