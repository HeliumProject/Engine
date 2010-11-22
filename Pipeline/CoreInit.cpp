#include "CoreInit.h"

#include "Foundation/InitializerStack.h"
#include "Asset/AssetInit.h"

#include "Pipeline/SceneGraph/SceneGraphInit.h"
#include "Pipeline/SettingsManager.h"
#include "Pipeline/Project.h"

using namespace Helium;

static uint32_t g_InitCount = 0;
static Helium::InitializerStack g_InitStack;

void Helium::CoreInitialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitStack.Push( &Asset::Initialize,       &Asset::Cleanup );
        g_InitStack.Push( &SceneGraph::Initialize,  &SceneGraph::Cleanup );

        g_InitStack.Push( Reflect::RegisterClassType< Project >( TXT("Project") ) );
        g_InitStack.Push( Reflect::RegisterClassType< SettingsManager >( TXT("SettingsManager") ) ); 
    }
}

void Helium::CoreCleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitStack.Cleanup();
    }
}