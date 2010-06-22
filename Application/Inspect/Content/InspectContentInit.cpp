#include "InspectContentInit.h"

#include "Application/Inspect/Content/Key.h"
#include "Application/Inspect/Content/KeyClipboardData.h"

#include "Foundation/InitializerStack.h"
#include "Application/Inspect/Widgets/Container.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Reflect/InspectReflectInit.h"
#include "Application/Inspect/Reflect/ReflectInterpreter.h"

#include "ParametricKeyInterpreter.h"

using namespace Inspect;

#pragma TODO( "W T F? Why is RegisterClass defined as RegisterClassA?" )
#undef RegisterClass

namespace InspectContent
{
    i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_InitializerStack;
}

void InspectContent::Initialize()
{
  bool result = true;

  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Inspect::Initialize, InspectReflect::Cleanup );
    g_InitializerStack.Push( InspectReflect::Initialize, Inspect::Cleanup );

    g_InitializerStack.Push( Reflect::RegisterClass<ParametricKeyControl>( "InspectParametricKeyControl" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Key>( "InspectKey" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<KeyClipboardData>( "InspectKeyClipboardData" ) );

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
