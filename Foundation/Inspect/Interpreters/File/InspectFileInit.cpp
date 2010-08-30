#include "InspectFileInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Inspect/Controls/InspectContainer.h"
#include "Foundation/Inspect/InspectInit.h"
#include "Foundation/Inspect/Interpreters/Reflect/InspectReflectInit.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#include "FileInterpreter.h"
#include "FileContainerInterpreter.h"

using namespace Helium;
using namespace Helium::Inspect;

static i32 g_InitCount = 0;
static Helium::InitializerStack g_RegisteredTypes;

void InspectFile::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    Inspect::Initialize();

    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::PathArraySerializer>() );
    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::PathSetSerializer>() );
    ReflectFieldInterpreterFactory::Register<FileInterpreter>( Reflect::GetType<Reflect::PathSerializer>() );
  }
}

void InspectFile::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_RegisteredTypes.Cleanup();
    Inspect::Cleanup();
  }
}
