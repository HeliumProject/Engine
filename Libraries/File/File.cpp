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
        }
    }

    void Cleanup()
    {
        if ( --g_InitRef == 0 )
        {
            if ( g_GlobalResolver )
            {
                delete g_GlobalResolver;
                g_GlobalResolver = NULL;
            }

            g_InitializerStack.Cleanup();
        }
    }

    void InitGlobalResolver( const std::string& resolverDatabaseFile, const std::string& resolverConfigPath )
    {
        g_GlobalResolver = new Resolver( resolverDatabaseFile, resolverConfigPath );
    }

    Resolver* GlobalResolver()
    {
        if ( !g_GlobalResolver )
        {
            throw Exception( "GlobalResolver is not initialized, must initialize it first." );
        }

        return g_GlobalResolver;
    }

}