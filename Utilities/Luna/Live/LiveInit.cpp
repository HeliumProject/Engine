#include "Precompile.h"
#include "LiveInit.h"
#include "LiveManager.h"
#include "RuntimeConnection.h"

#include "Common/InitializerStack.h"
#include "Dependencies/Dependencies.h"
#include "TargetManager/TargetManager.h"

using namespace Luna;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Luna::LiveInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Dependencies::Initialize, Dependencies::Cleanup );
    g_InitializerStack.Push( TargetManager::Initialize, TargetManager::Cleanup );
    g_InitializerStack.Push( RuntimeConnection::Initialize, RuntimeConnection::Cleanup );
    g_InitializerStack.Push( Live::Initialize, Live::Cleanup );
  }
}

void Luna::LiveCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
