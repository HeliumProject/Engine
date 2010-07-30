#include "Precompile.h"
#include "TaskInit.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"

using namespace Helium;
using namespace Helium::Editor;

static i32 g_InitCount = 0;
static Helium::InitializerStack g_InitializerStack;

void Editor::TaskInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
  }
}

void Editor::TaskCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
