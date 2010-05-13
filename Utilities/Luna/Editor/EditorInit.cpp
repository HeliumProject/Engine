#include "Precompile.h"
#include "EditorInit.h"

#include "ApplicationPreferences.h"
#include "EditorState.h"
#include "EditorInfo.h"
#include "Document.h"
#include "MRUData.h"
#include "Preferences.h"
#include "PreferencesBase.h"
#include "SessionVersion.h"
#include "SessionState.h"
#include "WindowSettings.h"

#include "Common/InitializerStack.h"
#include "Perforce/Perforce.h"
#include "Reflect/Registry.h"
#include "Finder/Finder.h"

using namespace Luna;

static i32 g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Luna::EditorInitialize()
{
  if ( ++g_InitCount == 1 )
  {
    // initialize core tools libraries before luna tools libraries
    g_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Finder::Initialize, Finder::Cleanup );

    // then register reflect stuff
    g_InitializerStack.Push( Reflect::RegisterEnumeration<Luna::FilePathOptions::FilePathOption>( &Luna::FilePathOptions::FilePathOptionEnumerateEnumeration, "FilePathOption" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<EditorTypes::EditorType>( &EditorTypes::EditorTypeEnumerateEnumeration, "EditorType" ) );

    g_InitializerStack.Push( Reflect::RegisterClass<SessionVersion>( "SessionVersion" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<EditorState>( "EditorState" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<SessionState>( "SessionState" ) );

    g_InitializerStack.Push( Document::InitializeType, Document::CleanupType );
    g_InitializerStack.Push( PreferencesBase::InitializeType, PreferencesBase::CleanupType );
    g_InitializerStack.Push( WindowSettings::InitializeType, WindowSettings::CleanupType );
    g_InitializerStack.Push( MRUData::InitializeType, MRUData::CleanupType );
    g_InitializerStack.Push( Preferences::InitializeType, Preferences::CleanupType );
    g_InitializerStack.Push( ApplicationPreferences::InitializeType, ApplicationPreferences::CleanupType );
  }
}

void Luna::EditorCleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();
  }
}
