#include "PipelinePch.h"
#include "CoreInit.h"

#include "Foundation/InitializerStack.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Project.h"

using namespace Helium;

static Helium::InitializerStack g_CoreInitStack;

void Helium::CoreInitialize()
{
    if ( g_CoreInitStack.Increment() == 1 )
    {
        g_CoreInitStack.Push( &Asset::Initialize,       &Asset::Cleanup );
        g_CoreInitStack.Push( Reflect::RegisterClassType< Project >( TXT( "Project" ) ) );
    }
}

void Helium::CoreCleanup()
{
    g_CoreInitStack.Decrement();
}