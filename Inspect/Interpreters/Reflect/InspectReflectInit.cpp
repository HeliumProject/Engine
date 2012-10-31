#include "InspectPch.h"
#include "InspectReflectInit.h"

#include "Application/InitializerStack.h"
#include "Inspect/Inspect.h"
#include "Inspect/Container.h"

#include "Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectBitfieldInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectVectorInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectColorInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectStlVectorInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectStlSetInterpreter.h"

#include "Inspect/Interpreters/Reflect/ReflectPathInterpreter.h"
#include "Inspect/Interpreters/Reflect/ReflectPathContainerInterpreter.h"

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
#ifdef REFLECT_REFACTOR
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector2Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectVectorInterpreter>( Reflect::GetClass<Reflect::Vector4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::Color3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::HDRColor3Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::Color4Data>() );
        ReflectFieldInterpreterFactory::Register<ReflectColorInterpreter>( Reflect::GetClass<Reflect::HDRColor4Data>() );
#endif

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
