#include "InspectContentInit.h"

#include "Application/Inspect/Interpreters/Content/Key.h"
#include "Application/Inspect/Interpreters/Content/KeyClipboardData.h"

#include "Foundation/InitializerStack.h"
#include "Application/Inspect/Controls/Container.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Interpreters/Reflect/InspectReflectInit.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#include "ParametricKeyInterpreter.h"

using namespace Helium;
using namespace Helium::Inspect;

namespace Helium
{
    namespace InspectContent
    {
        i32 g_InitCount = 0;
        Helium::InitializerStack g_InitializerStack;
    }
}

void InspectContent::Initialize()
{
    bool result = true;

    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( Inspect::Initialize, InspectReflect::Cleanup );
        g_InitializerStack.Push( InspectReflect::Initialize, Inspect::Cleanup );

        g_InitializerStack.Push( Reflect::RegisterClassType<ParametricKeyControl>( TXT( "InspectParametricKeyControl" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Key>( TXT( "InspectKey" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<KeyClipboardData>( TXT( "InspectKeyClipboardData" ) ) );

        //ReflectFieldInterpreterFactory::Register<ParametricKeyInterpreter>( Reflect::GetType<Reflect::ElementArraySerializer>(), Content::ContentFlags::ParametricKey );
    }
}

void InspectContent::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
