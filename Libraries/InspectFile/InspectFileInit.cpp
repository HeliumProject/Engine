#include "StdAfx.h"
#include "InspectFileInit.h"

#include "Common/InitializerStack.h"
#include "Inspect/Container.h"
#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectReflect/ReflectInterpreter.h"

#include "File/Manager.h"
#include "FileDialogButton.h"
#include "FileBrowserButton.h"

#include "FileInterpreter.h"
#include "FileContainerInterpreter.h"

using namespace Inspect;

i32 g_InitCount = 0;
Nocturnal::InitializerStack g_RegisteredTypes;

void InspectFile::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    File::Initialize();
    Inspect::Initialize();

    g_RegisteredTypes.Push( Reflect::RegisterClass<FileDialogButton>( "InspectFileDialogButton" ) );
    g_RegisteredTypes.Push( Reflect::RegisterClass<FileBrowserButton>( "InspectFileBrowserButton" ) );

    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::StringArraySerializer>(), Reflect::FieldFlags::FilePath );
    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::TUIDArraySerializer>(), Reflect::FieldFlags::FileID );
    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::U64ArraySerializer>(), Reflect::FieldFlags::FileID );
    ReflectFieldInterpreterFactory::Register<FileContainerInterpreter>( Reflect::GetType<Reflect::U64SetSerializer>(), Reflect::FieldFlags::FileID );
    ReflectFieldInterpreterFactory::Register<FileInterpreter>( Reflect::GetType<Reflect::StringSerializer>(), Reflect::FieldFlags::FilePath );
    ReflectFieldInterpreterFactory::Register<FileInterpreter>( Reflect::GetType<Reflect::TUIDSerializer>(), Reflect::FieldFlags::FileID );
    ReflectFieldInterpreterFactory::Register<FileInterpreter>( Reflect::GetType<Reflect::U64Serializer>(), Reflect::FieldFlags::FileID );
  }
}

void InspectFile::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_RegisteredTypes.Cleanup();
    Inspect::Cleanup();
    File::Cleanup();
  }
}
