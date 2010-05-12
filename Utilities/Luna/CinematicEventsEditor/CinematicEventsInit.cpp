#include "Precompile.h"
#include "CinematicEventsInit.h"
#include "CinematicEventsEditor.h"
#include "CinematicEventsDocument.h"
#include "CinematicPreferences.h"

#include "Common/InitializerStack.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetVersion.h"
#include "FileSystem/FileSystem.h"
#include "Console/Console.h"
#include "Editor/SessionManager.h"
#include "Reflect/Registry.h"
#include "Conduit/ConduitInit.h"


// Using
using namespace Luna;


namespace Luna
{
  namespace LunaCinematicEvents
  {
    i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_RegisteredTypes;
    Nocturnal::InitializerStack g_InitializerStack;
  }
}


///////////////////////////////////////////////////////////////////////////////
// Prepares this module for first use.  Only the first call to this function
// does something.  All subsequent calls just increment an internal counter and
// must be matched by a call to CleanupModule.
// 
void LunaCinematicEvents::InitializeModule()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
    g_InitializerStack.Push( Conduit::Initialize, Conduit::Cleanup );
    g_InitializerStack.Push( CinematicEventsDocument::InitializeType, CinematicEventsDocument::CleanupType );
    g_InitializerStack.Push( CinematicEventsEditor::InitializeEditor, CinematicEventsEditor::CleanupEditor );
    g_InitializerStack.Push( CinematicPreferences::InitializeType, CinematicPreferences::CleanupType );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Decrements an internal counter.  Once the counter reaches zero, the module
// is unloaded.
// 
void LunaCinematicEvents::CleanupModule()
{
  if ( --g_InitCount == 0 )
  {
    g_RegisteredTypes.Cleanup();
    g_InitializerStack.Cleanup();
  }
}
