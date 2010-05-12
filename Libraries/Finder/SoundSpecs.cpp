#include "SoundSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "ProjectSpecs.h" 

namespace FinderSpecs
{
  // 
  //  folders
  //

  const FolderSpec      Sound::BASE_FOLDER            ( "Sound::BASE_FOLDER",             "sound",                FolderRoots::ProjectAssets );
  const FolderSpec      Sound::PRIVATE_FOLDER         ( "Sound::PRIVATE_FOLDER",          "private",              Sound::BASE_FOLDER );
  const FolderSpec      Sound::CONFIG_FOLDER          ( "Sound::CONFIG_FOLDER",           "config",               Sound::BASE_FOLDER );
  const FolderSpec      Sound::GLOBAL_FOLDER          ( "Sound::GLOBAL_FOLDER",           "global",               Sound::BASE_FOLDER );
  const FolderSpec      Sound::DIALOG_FOLDER          ( "Sound::DIALOG_FOLDER",           "dialogue",             Sound::BASE_FOLDER );
  const FolderSpec      Sound::MOVIE_FOLDER           ( "Sound::MOVIE_FOLDER",            "mpeg",                 Sound::DIALOG_FOLDER );
  const FolderSpec      Sound::REALTIME_FOLDER        ( "Sound::REALTIME_FOLDER",         "realtime",             Sound::DIALOG_FOLDER );
  
  const FolderSpec      Sound::SOUND_FOLDER           ( "Sound::SOUND_FOLDER",            "intermediate_sound");
  const FolderSpec      Sound::DIALOGUE_FOLDER        ( "Sound::DIALOGUE_FOLDER",         "intermediate_dialogue");

  const FolderSpec      Sound::BUILT_SOUND_FOLDER     ( "Sound::BUILT_SOUND_FOLDER",      "sound",                Project::BUILT_FOLDER);  

  //  
  //  suffixes
  //

  const SuffixSpec      Sound::SOUND_SUFFIX           ( "Sound::SOUND_SUFFIX",            "_sound" );
  const SuffixSpec      Sound::DIALOGUE_SUFFIX        ( "Sound::DIALOGUE_SUFFIX",         "_dialogue" );
  const SuffixSpec      Sound::SCREAM_SUFFIX          ( "Sound::SCREAM_SUFFIX",           "_scream" );
  const SuffixSpec      Sound::SND_STREAM_SUFFIX      ( "Sound::SND_STREAM_SUFFIX",       "_snd_stream" );
  const SuffixSpec      Sound::THEME_A_SUFFIX         ( "Sound::THEME_A_SUFFIX",          "_a" );
  const SuffixSpec      Sound::THEME_B_SUFFIX         ( "Sound::THEME_B_SUFFIX",          "_b" );

  //
  //  decorations
  //
    
  // .scream.bnk
  const DecorationSpec  Sound::SCREAM_DECORATION      ( "Sound::SCREAM_DECORATION",       "Scream Bank File",       SCREAM_SUFFIX,          Extension::BNK);
  // .snd_stream.dat
  const DecorationSpec  Sound::SND_STREAM_DECORATION  ( "Sound::SND_STREAM_DECORATION",   "Sound Stream Data File", SND_STREAM_SUFFIX,      Extension::DATA);

  //
  //  files
  //
                                                                                        
  const FileSpec    Sound::SCREAM_SFX_FILE            ( "Sound::SCREAM_SFX_FILE",           "",                         Extension::SFX,             "10_dthall" );
  const FileSpec    Sound::SCREAM_BANK_FILE           ( "Sound::SCREAM_BANK_FILE",          "",                         Extension::BANK,            "10_dthall" );  
  const FileSpec    Sound::BANKMERGE_FILE             ( "Sound::BANKMERGE_FILE",            "bankmerge",                Extension::TXT,             "21_dthall" );
  const FileSpec    Sound::RESIDENT_FILE              ( "Sound::RESIDENT_FILE",             "resident",                 Extension::DATA,            "21_dthall" );
  const FileSpec    Sound::STREAMING_FILE             ( "Sound::STREAMING_FILE",            "streaming",                Extension::DATA,            "21_dthall" );
  const FileSpec    Sound::VAG_FILE                   ( "Sound::VAG_FILE",                  "",                         Extension::VAG,             "13_dthall" );
  const FileSpec    Sound::XVAG_FILE                  ( "Sound::XVAG_FILE",                 "",                         Extension::XVAG,            "13_dthall" );
  const FileSpec    Sound::MP3_FILE                   ( "Sound::MP3_FILE",                  "",                         Extension::MP3,             "13_dthall" );
  const FileSpec    Sound::WAV_FILE                   ( "Sound::WAV_FILE",                  "",                         Extension::WAV,             "13_dthall" );
  const FileSpec    Sound::THEME_A_FILE               ( "Sound::THEME_A_FILE",              "a",                        Extension::WAV,             "13_dthall" );
  const FileSpec    Sound::THEME_B_FILE               ( "Sound::THEME_B_FILE",              "b",                        Extension::WAV,             "13_dthall" ); 
  const FileSpec    Sound::GENERATED_DIALOGUE_FILE    ( "Sound::GENERATED_DIALOGUE_FILE",   "",                         Extension::WAV,             "4_dthall");   
 
  const FileSpec    Sound::CONFIG_FILE                ( "Sound::CONFIG_FILE",               "sound_config",             Extension::XML,             "");
  const FileSpec    Sound::LEVEL_CONFIG_FILE          ( "Sound::LEVEL_CONFIG_FILE",         "level_config",             Extension::XML,             "");
  const FileSpec    Sound::NONLOC_CONFIG_FILE         ( "Sound::NONLOC_CONFIG_FILE",        "nonloc_config",            Extension::XML,             "");
  const FileSpec    Sound::NULL_SFX_FILE              ( "Sound::NULL_SFX_FILE",             "empty",                    Extension::SFX,             "");
  const FileSpec    Sound::NULL_DIALOG_FILE           ( "Sound::NULL_DIALOG_FILE",          "empty",                    Extension::WAV,             "");
  const FileSpec    Sound::DIALOG_GUIDS_FILE          ( "Sound::DIALOG_GUIDS_FILE",         "dialogue",                 Extension::TXT,             "");
}
