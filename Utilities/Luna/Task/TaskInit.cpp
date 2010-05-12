#include "Precompile.h"
#include "TaskInit.h"

#include "Build.h"
#include "Export.h"

#include "Asset/AssetInit.h"
#include "AssetBuilder/AssetBuilder.h"
#include "Common/InitializerStack.h"
#include "Reflect/Registry.h"

using namespace Luna;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Luna::TaskInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
    g_InitializerStack.Push( Build::Initialize, Build::Cleanup );
    g_InitializerStack.Push( Export::Initialize, Export::Cleanup );
  }
}

void Luna::TaskCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
