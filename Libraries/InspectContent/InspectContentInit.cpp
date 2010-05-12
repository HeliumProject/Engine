#include "StdAfx.h"
#include "InspectContentInit.h"

#include "Key.h"
#include "KeyClipboardData.h"

#include "Common/InitializerStack.h"
#include "Inspect/Container.h"
#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectReflect/ReflectInterpreter.h"

#include "ParametricKeyInterpreter.h"

using namespace Inspect;

i32 g_InitCount = 0;
Nocturnal::InitializerStack g_InitializerStack;

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
