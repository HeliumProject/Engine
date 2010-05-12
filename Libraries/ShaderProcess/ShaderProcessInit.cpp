#include "ShaderProcessInit.h"
#include "ShaderRegistry.h"
#include "Common/InitializerStack.h"

namespace ShaderProcess
{
  i32 g_InitCount = 0;
  Nocturnal::InitializerStack g_InitializerStack;
}

void ShaderProcess::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( ShaderRegistry::Initialize, ShaderRegistry::Cleanup );
  }
}

void ShaderProcess::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
