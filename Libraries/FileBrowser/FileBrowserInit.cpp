#include "stdafx.h"
#include "FileBrowserInit.h"
#include "Common/InitializerStack.h"
#include "File/Manager.h"
#include "FileInfo.h"

namespace FileBrowser
{
    static i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_RegisteredTypes;
}

void FileBrowser::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    File::Initialize();

    g_RegisteredTypes.Push( Reflect::RegisterClass<File::FileInfo>( "FileInfo" ) );
  }
}

void FileBrowser::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_RegisteredTypes.Cleanup();
    File::Cleanup();
  }
}