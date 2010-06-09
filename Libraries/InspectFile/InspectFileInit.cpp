#include "StdAfx.h"
#include "InspectFileInit.h"

#include "Common/InitializerStack.h"
#include "Inspect/Container.h"
#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectReflect/ReflectInterpreter.h"

#include "FileDialogButton.h"
#include "FileBrowserButton.h"

#include "FileInterpreter.h"
#include "FileContainerInterpreter.h"

using namespace Inspect;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_RegisteredTypes;

void InspectFile::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    Inspect::Initialize();

    g_RegisteredTypes.Push( Reflect::RegisterClass<FileDialogButton>( "InspectFileDialogButton" ) );
    g_RegisteredTypes.Push( Reflect::RegisterClass<FileBrowserButton>( "InspectFileBrowserButton" ) );

    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::StringArraySerializer>(), Reflect::FieldFlags::FilePath );
    ReflectFieldInterpreterFactory::Register<FileInterpreter>( Reflect::GetType<Reflect::StringSerializer>(), Reflect::FieldFlags::FilePath );
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
