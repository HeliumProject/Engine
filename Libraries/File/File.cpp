#include "StdAfx.h"

#include "File.h"
#include "Reference.h"

#include "Common/InitializerStack.h"
#include "Common/Environment.h"

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

namespace File
{
  static Resolver* g_GlobalResolver = NULL;

  int g_InitRef = 0;
  Nocturnal::InitializerStack g_InitializerStack;

  void Initialize()
  {
    if ( ++g_InitRef == 1 ) 
    {
      g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
      g_InitializerStack.Push( Reflect::RegisterClass< Reference >( "Reference" ) );

      std::string resolverDatabaseFile;
      if ( !Nocturnal::GetEnvVar( "PROJECT_RESOLVER_DB_FILE", resolverDatabaseFile ) )
      {
        throw Exception( "No resolver database file defined.  PROJECT_RESOLVER_DB_FILE must be set." );
      }

      std::string resolverConfigPath;
      if ( !Nocturnal::GetEnvVar( "PROJECT_RESOLVER_CONFIG_PATH", resolverConfigPath ) )
      {
        throw Exception( "No resolver config path defined.  PROJECT_RESOLVER_CONFIG_PATH must be set." );
      }

      g_GlobalResolver = new Resolver( resolverDatabaseFile, resolverConfigPath );
    }
  }

  void Cleanup()
  {
    if ( --g_InitRef == 0 )
    {
      delete g_GlobalResolver;
      g_GlobalResolver = NULL;

      g_InitializerStack.Cleanup();
    }
  }

  Resolver& GlobalResolver()
  {
    if ( !g_GlobalResolver )
    {
      throw Exception( "GlobalResolver is not initialized, must call File::Initialize() first." );
    }

    return *g_GlobalResolver;
  }

}