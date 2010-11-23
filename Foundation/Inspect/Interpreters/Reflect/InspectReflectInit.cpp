#include "InspectReflectInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Container.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectBitfieldInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectVectorInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectColorInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectStlVectorInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectStlSetInterpreter.h"

#include "Foundation/Inspect/Interpreters/Reflect/ReflectPathInterpreter.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectPathContainerInterpreter.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_ABSTRACT( Inspect::ClientDataFilter );

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

        g_IntializerStack.Push( Reflect::RegisterClassType<ClientData>( TXT( "Inspect::ClientData" ) ) );
        g_IntializerStack.Push( Reflect::RegisterClassType<ClientDataFilter>( TXT( "Inspect::ClientDataFilter" ) ) );

        // scalars
        ReflectFieldInterpreterFactory::Register<ReflectBitfieldInterpreter>( Reflect::GetType<Reflect::BitfieldData>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector2Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetType<Reflect::Vector4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::Color3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::HDRColor3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::Color4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetType<Reflect::HDRColor4Data>() );

        // containers
        ReflectFieldInterpreterFactory::Register<ReflectStlVectorInterpreter>( Reflect::GetType<Reflect::StlVectorData>() );
        ReflectFieldInterpreterFactory::Register<ReflectStlSetInterpreter>( Reflect::GetType<Reflect::StlSetData>() );

        // paths
        ReflectFieldInterpreterFactory::Register<PathInterpreter>( Reflect::GetType<Reflect::PathData>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetType<Reflect::PathStlVectorData>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetType<Reflect::PathStlSetData>() );
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
