#include "FoundationPch.h"
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

REFLECT_DEFINE_ABSTRACT( Helium::Inspect::ClientDataFilter );

using namespace Helium;
using namespace Helium::Inspect;

static Helium::InitializerStack g_InspectReflectInitStack;

void InspectReflect::Initialize()
{
    if ( g_InspectReflectInitStack.Increment() == 1 )
    {
        g_InspectReflectInitStack.Push( Inspect::Initialize, Inspect::Cleanup );

        // scalars
        ReflectFieldInterpreterFactory::Register<ReflectBitfieldInterpreter>( Reflect::GetClass<Reflect::BitfieldData>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector2Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::Color3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::HDRColor3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::Color4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::HDRColor4Data>() );

        // containers
        ReflectFieldInterpreterFactory::Register<ReflectStlVectorInterpreter>( Reflect::GetClass<Reflect::StlVectorData>() );
        ReflectFieldInterpreterFactory::Register<ReflectStlSetInterpreter>( Reflect::GetClass<Reflect::StlSetData>() );

        // paths
        ReflectFieldInterpreterFactory::Register<PathInterpreter>( Reflect::GetClass<Reflect::PathData>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetClass<Reflect::PathStlVectorData>() );
        ReflectFieldInterpreterFactory::Register<PathContainerInterpreter>( Reflect::GetClass<Reflect::PathStlSetData>() );
    }
}

void InspectReflect::Cleanup()
{
    if ( g_InspectReflectInitStack.Decrement() == 0 )
    {
        ReflectFieldInterpreterFactory::Clear();
    }
}
