#include "EditorPch.h"
#include "TaskInit.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Object.h"

using namespace Helium;
using namespace Helium::Editor;

static Helium::InitializerStack g_TaskInitStack;

void Editor::TaskInitialize()
{
    if ( g_TaskInitStack.Increment() == 1 )
    {
        g_TaskInitStack.Push( Asset::Initialize, Asset::Cleanup );
    }
}

void Editor::TaskCleanup()
{
    g_TaskInitStack.Decrement();
}
