#include "Precompile.h"
#include "CoreInit.h"

#include "Foundation/InitializerStack.h"
#include "Application/Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectContent/InspectContentInit.h"
#include "InspectFile/InspectFileInit.h"

#include "Object.h"
#include "Selectable.h"
#include "Persistent.h"
#include "Enumerator.h"

#include "Foundation/InitializerStack.h"

using namespace Luna;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Luna::CoreInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    // Libraries
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Inspect::Initialize, Inspect::Cleanup );
    g_InitializerStack.Push( InspectReflect::Initialize, InspectReflect::Cleanup );
    g_InitializerStack.Push( InspectContent::Initialize, InspectContent::Cleanup );
    g_InitializerStack.Push( InspectFile::Initialize, InspectFile::Cleanup );

    // Types
    g_InitializerStack.Push( Object::InitializeType, Object::CleanupType );
    g_InitializerStack.Push( Selectable::InitializeType, Selectable::CleanupType );
    g_InitializerStack.Push( Persistent::InitializeType, Persistent::CleanupType );
    g_InitializerStack.Push( Enumerator::Initialize, Enumerator::Cleanup );
  }
}

void Luna::CoreCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
