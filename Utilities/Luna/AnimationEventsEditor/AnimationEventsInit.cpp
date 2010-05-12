#include "Precompile.h"
#include "AnimationEventsInit.h"
#include "AnimationEventsEditor.h"
#include "AnimationEventsDocument.h"
#include "AnimationEventsPreferences.h"

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
  namespace LunaAnimationEvents
  {
    i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_InitializerStack;
    Nocturnal::InitializerStack g_RegisteredTypes;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Prepares this module for first use.  Only the first call to this function
// does something.  All subsequent calls just increment an internal counter and
// must be matched by a call to CleanupModule.
// 
void LunaAnimationEvents::InitializeModule()
{ 
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
    g_InitializerStack.Push( Conduit::Initialize, Conduit::Cleanup );
    g_InitializerStack.Push( AnimationEventsDocument::InitializeType, AnimationEventsDocument::CleanupType );
    g_InitializerStack.Push( AnimationEventsEditor::InitializeEditor, AnimationEventsEditor::CleanupEditor );
    g_InitializerStack.Push( AnimationEventsPreferences::InitializeType, AnimationEventsPreferences::CleanupType );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Decrements an internal counter.  Once the counter reaches zero, the module
// is unloaded.
// 
void LunaAnimationEvents::CleanupModule()
{
  if ( --g_InitCount == 0 )
  {
    g_RegisteredTypes.Cleanup();
    g_InitializerStack.Cleanup();
  }
}
