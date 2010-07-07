#include "Precompile.h"
#include "TaskInit.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"

using namespace Luna;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Luna::TaskInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
  }
}

void Luna::TaskCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
