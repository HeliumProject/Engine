#include "InspectReflectInit.h"

#include "Foundation/InitializerStack.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Controls/Container.h"

#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectBitfieldInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectVectorInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectColorInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectArrayInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectSetInterpreter.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectMapInterpreter.h"

#pragma TODO( "Why is RegisterClass defined?" )
#undef RegisterClass

using namespace Inspect;

namespace InspectReflect
{
    i32 g_InitCount = 0;
}

Helium::InitializerStack g_IntializerStack;

void InspectReflect::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_IntializerStack.Push( Inspect::Initialize, Inspect::Cleanup );

    g_IntializerStack.Push( Reflect::RegisterClass<ClientDataControl>( TXT( "InspectClientDataControl" ) ) );
    g_IntializerStack.Push( Reflect::RegisterClass<ClientDataFilter>( TXT( "InspectClientDataFilter" ) ) );

    g_IntializerStack.Push( Reflect::RegisterClass<ReflectBitfieldCheckBox>( TXT( "InspectReflectBitfieldCheckBox" ) ) );

    // scalars
    ReflectFieldInterpreterFactory::Register<ReflectBitfieldInterpreter>( Reflect::GetType<Reflect::BitfieldSerializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector2Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector3Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector4Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::Color3Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::HDRColor3Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::Color4Serializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::HDRColor4Serializer>() );

    // containers
    ReflectFieldInterpreterFactory::Register<ReflectArrayInterpreter>( Reflect::GetType<Reflect::ArraySerializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectSetInterpreter>( Reflect::GetType<Reflect::SetSerializer>() );
    ReflectFieldInterpreterFactory::Register<ReflectMapInterpreter>( Reflect::GetType<Reflect::MapSerializer>() );
  }
}

void InspectReflect::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    ReflectFieldInterpreterFactory::Clear();
    g_IntializerStack.Cleanup();
  }
}
