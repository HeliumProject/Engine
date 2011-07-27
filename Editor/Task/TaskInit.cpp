#include "EditorPch.h"
#include "TaskInit.h"

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

    }
}

void Editor::TaskCleanup()
{
    g_TaskInitStack.Decrement();
}
