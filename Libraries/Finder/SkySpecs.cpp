#include "SkySpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Sky::MAIN_FILE                 ( "Sky::MAIN_FILE",                 "main",  Extension::DATA, "16 - abezrati" );
  const FileSpec Sky::VRAM_FILE                 ( "Sky::VRAM_FILE",                 "vram",  Extension::DATA, "14 - abezrati" );

  const FileSpec Sky::LF_CLOUD_TEXTURE_NAME     ( "Sky::LF_CLOUD_TEXTURE_NAME",     "" );
  const FileSpec Sky::HF_CLOUD_TEXTURE_NAME     ( "Sky::HF_CLOUD_TEXTURE_NAME",     "" );
  const FileSpec Sky::TURBULANCE_TEXTURE_NAME   ( "Sky::TURBULANCE_TEXTURE_NAME",   "" );
  const FileSpec Sky::NORMAL_MAP_NAME           ( "Sky::NORMAL_MAP_NAME",           "" );
  const FileSpec Sky::SHELL_TEXTURE_NAME        ( "Sky::SHELL_TEXTURE_NAME",        "" );
}
