#include "InspectFileInit.h"

#include "Foundation/InitializerStack.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Interpreters/Reflect/InspectReflectInit.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#include "FileDialogButton.h"
#include "FileBrowserButton.h"

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

    g_RegisteredTypes.Push( Reflect::RegisterClassType<FileDialogButton>( TXT( "InspectFileDialogButton" ) ) );
    g_RegisteredTypes.Push( Reflect::RegisterClassType<FileBrowserButton>( TXT( "InspectFileBrowserButton" ) ) );

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
