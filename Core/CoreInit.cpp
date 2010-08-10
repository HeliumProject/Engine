#include "CoreInit.h"
#include "Content/ContentInit.h"
#include "Asset/AssetInit.h"
#include "Project.h"

#include "Foundation/InitializerStack.h"

using namespace Helium;
using namespace Helium::Core;

static u32 g_InitCount = 0;
static Helium::InitializerStack g_InitStack;

void Core::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitStack.Push( &Content::Initialize, &Content::Cleanup );
        g_InitStack.Push( &Asset::Initialize, &Asset::Cleanup );
        g_InitStack.Push( Reflect::RegisterClass<Core::ProjectFile>( TXT("ProjectFile") ) );
        g_InitStack.Push( Reflect::RegisterClass<Core::ProjectFolder>( TXT("ProjectFolder") ) );
        g_InitStack.Push( Reflect::RegisterClass<Core::Project>( TXT("Project") ) );
    }
}

void Core::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitStack.Cleanup();
    }
}