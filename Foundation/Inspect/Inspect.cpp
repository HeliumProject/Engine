#include "FoundationPch.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Script.h"
#include "Foundation/Inspect/Controls.h"

#include "Foundation/Reflect/Registry.h"

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
