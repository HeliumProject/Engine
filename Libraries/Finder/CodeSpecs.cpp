#include "CodeSpecs.h"

#include "Common/Environment.h"
#include "Common/Exception.h"

namespace FinderSpecs
{
  const FolderSpec Code::PACKAGES     ( "Code::Packages",         "packages",   FolderRoots::ProjectCode );
  const FolderSpec Code::CONFIG       ( "Code::Config",           "config",     FolderRoots::ProjectCode );
  const FolderSpec Code::OUTPUT       ( "Code::Output",           "output",     FolderRoots::ProjectCode );

  const ExtensionSpec Code::HEADER_FILE_EXTENSION   ( "Code::HEADER_FILE_EXTENSION", "Header File",      "h" );

  void Code::Init()
  {
    // awesome
    std::string value;
    if (Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"GAME", value ))
    {
      {
        // awesome++
        FolderSpec& folder = (FolderSpec&)Code::PACKAGES;
        folder.SetValue( value + '/' + folder.GetValue() );
      }

      {
        // awesome++
        FolderSpec& folder = (FolderSpec&)Code::CONFIG;
        folder.SetValue( value + '/' + folder.GetValue() );
      }
    }
    else
    {
      throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"GAME is not defined in the environment" );
    }
  }
}
