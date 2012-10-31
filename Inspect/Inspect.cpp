#include "InspectPch.h"
#include "Application/InitializerStack.h"
#include "Inspect/Inspect.h"
#include "Inspect/Script.h"
#include "Inspect/Controls.h"

#include "Reflect/Registry.h"

using namespace Helium;
using namespace Helium::Inspect;

EditFilePathSignature::Event Inspect::g_EditFilePath;

static Helium::InitializerStack g_InspectInitStack;

void Inspect::Initialize()
{
    if ( g_InspectInitStack.Increment() == 1 )
    {
        g_InspectInitStack.Push( Script::Initialize, Script::Cleanup );
    }
}

void Inspect::Cleanup()
{
    g_InspectInitStack.Decrement();
}
