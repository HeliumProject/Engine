#include "CoreInit.h"
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