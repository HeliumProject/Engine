#include "InspectReflectInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Container.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectBitfieldInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectVectorInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectColorInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectArrayInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectSetInterpreter.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectPathInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectPathContainerInterpreter.h"

using namespace Helium;
using namespace Helium::Inspect;

namespace Helium
{
    namespace InspectReflect
    {
        int32_t g_InitCount = 0;
    }
}

Helium::InitializerStack g_IntializerStack;

void InspectReflect::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_IntializerStack.Push( Inspect::Initialize, Inspect::Cleanup );

        g_IntializerStack.Push( Reflect::RegisterClassType<ClientData>( TXT( "InspectClientDataControl" ) ) );
        g_IntializerStack.Push( Reflect::RegisterClassType<ClientDataFilter>( TXT( "InspectClientDataFilter" ) ) );

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

        // paths
        ReflectFieldInterpreterFactory::Register<PathInterpreter>( Reflect::GetType<Reflect::PathSerializer>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetType<Reflect::PathArraySerializer>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetType<Reflect::PathSetSerializer>() );
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
