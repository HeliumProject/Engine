#include "AppUtils/AppUtils.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "NSL/NSL.h"

#include "Console/Console.h"

#include "Common/Types.h"

#include <cstring>

void PrintUsage()
{
  Console::Print( "Usage: NSCopy [options] <source1> <source2> <source3> <...>\n" );
  Console::Print( "\t-p|--put: Copy local->network\n" );
  Console::Print( "\t-g|--get: Copy network-local\n" );
  Console::Print( "\t-f|--force: force a copy (copies even if the destination has been modified more recently than the source)\n" );
  Console::Print( "\t-k|--key: only copy if the key file has been modified more recently than the matching key file in the destination\n" );
}

///////////////////////////////////////////////////////////////////////////////
int Main (int argc, const char** argv)
{
  if ( argc < 3 )
  {
    PrintUsage();
    return Console::GetErrorCount();
  }

  bool copyToNetwork = true;
  bool directionIsSet = false;
  bool force = false;
  std::string keyFile;

  std::string projectRoot = Finder::ProjectAssets();
  FileSystem::Win32Name( projectRoot );

  V_string sources;
  for ( int i = 1;i < argc; ++i )
  {
    if ( !stricmp( argv[ i ], "--force" ) || !stricmp( argv[ i ], "-f" ) )
    {
      force = true;
    }
    else if ( !stricmp( argv[ i ], "--key" ) || !stricmp( argv[ i ], "-k" ) )
    {
      if ( i < argc - 2 )
      {
        ++i;
        keyFile = argv[ i ];
      }
      else
      {
        throw Nocturnal::Exception( "--key or -k, but no key file was specified.\n" );
      }
    }
    else if ( !stricmp( argv[ i ], "--put" ) || !stricmp( argv[ i ], "-p" ) )
    {
      copyToNetwork = true;
      directionIsSet = true;
    }
    else if ( !stricmp( argv[ i ], "--get" ) || !stricmp( argv[ i ], "-g" ) )
    {
      copyToNetwork = false;
      directionIsSet = true;
    }
    else
    {
      std::string path( argv[ i ] );
      FileSystem::Win32Name( path );
      sources.push_back( path );
    }
  }

  if ( sources.empty() )
  {
    throw Nocturnal::Exception( "No sources specified\n" );
  }

  if ( !directionIsSet )
  {
    throw Nocturnal::Exception( "No direction was set!  Use \"--put\" to copy local->network, or \"--get\" to copy network->local\n" );
  }

  if ( copyToNetwork )
  {
    NSL::CopyToNetwork( sources, keyFile, force );
  }
  else
  {
    NSL::CopyFromNetwork( sources, keyFile, force );
  }

  return Console::GetErrorCount();
}

int main(int argc, const char** argv)
{
  return AppUtils::StandardMain( &Main, argc, argv );
}
