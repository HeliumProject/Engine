#include "StdAfx.h"
#include "InspectReflectInit.h"

#include "Common/InitializerStack.h"
#include "Inspect/InspectInit.h"
#include "Inspect/Container.h"

#include "ReflectInterpreter.h"
#include "ReflectBitfieldInterpreter.h"
#include "ReflectVectorInterpreter.h"
#include "ReflectColorInterpreter.h"
#include "ReflectArrayInterpreter.h"
#include "ReflectSetInterpreter.h"
#include "ReflectMapInterpreter.h"

using namespace Inspect;

i32 g_InitCount = 0;
Nocturnal::InitializerStack g_IntializerStack;

void InspectReflect::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_IntializerStack.Push( Inspect::Initialize, Inspect::Cleanup );

    g_IntializerStack.Push( Reflect::RegisterClass<ClientDataControl>( "InspectClientDataControl" ) );
    g_IntializerStack.Push( Reflect::RegisterClass<ClientDataFilter>( "InspectClientDataFilter" ) );

    g_IntializerStack.Push( Reflect::RegisterClass<ReflectBitfieldCheckBox>( "InspectReflectBitfieldCheckBox" ) );

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
