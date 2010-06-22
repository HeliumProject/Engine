#include "Platform/Windows/Windows.h"
#include "BuilderInterface.h"

#include "Foundation/Log.h"
#include "Foundation/Exception.h"
#include "Pipeline/Asset/AssetClass.h"

using namespace Asset;
using namespace AssetBuilder;

typedef std::map<AssetType, HMODULE> M_Library;
typedef std::map<AssetType, AllocateBuildersFunc> M_Allocator;

static const char* InitializeEntryPoint = "InitializeBuilderModule";
static const char* CleanupEntryPoint = "CleanupBuilderModule";
static const char* AllocateEntryPoint = "AllocateBuilders";

static M_Library g_Libraries;
static M_Allocator g_Allocators;

void BuilderInterface::Initialize()
{
}

void BuilderInterface::Cleanup()
{
  M_Library::iterator itor;
  M_Library::iterator end = g_Libraries.end();

  for( itor = g_Libraries.begin(); itor != end; ++itor )
  {
    void (*cleanup)() = (void (*)())GetProcAddress( itor->second, CleanupEntryPoint );
    if( cleanup != NULL )
    {
      cleanup();
    }

    FreeLibrary( itor->second );
  }

  g_Libraries.clear();
  g_Allocators.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// AllocateBuilder
//
// Loads the appropriate builder .dll and finds and returns the .dll's Builder interface
// Note: cache's each loaded Builder
//
///////////////////////////////////////////////////////////////////////////////////////////
void BuilderInterface::AllocateBuilders( AssetType assetType, AssetBuilder::V_IBuilder& builders )
{
  {
    M_Allocator::const_iterator found = g_Allocators.find( assetType );
    if ( found != g_Allocators.end() )
    {
      found->second( builders );

      return;
    }
  }

  std::string builder = AssetClass::GetAssetTypeBuilder( assetType );
  if ( builder.empty() )
  {
    throw AssetBuilder::Exception( "Cannot find builder module for asset type %d", assetType );
  }

  HMODULE lib = LoadLibrary( builder.c_str() );
  if( lib == NULL )
  {
    DWORD err = ::GetLastError();
    throw AssetBuilder::Exception( "Error loading library %s: (%d) %s\n", builder.c_str(), err, Platform::GetErrorString(err).c_str() );
  }

  void (*init)() = (void (*)())GetProcAddress( lib, InitializeEntryPoint );
  if( init == NULL )
  {
    FreeLibrary( lib );
    throw AssetBuilder::Exception( "Cannot load builder interface from lib: %s", builder.c_str() );
  }

  init();

  AllocateBuildersFunc func = ( AllocateBuildersFunc )( GetProcAddress( lib, AllocateEntryPoint ) );
  if( func == NULL )
  {
    FreeLibrary( lib );
    throw AssetBuilder::Exception( "Cannot load builder interface from lib: %s", builder.c_str() );
  }

  g_Libraries[assetType] = lib;
  g_Allocators[assetType] = func;

  func( builders );
}

AssetBuilder::BuildJob::BuildJob()
: m_DependencyGraph( NULL )
, m_Asset( NULL )
, m_Options( NULL )
, m_Builder( NULL )
, m_Flags( 0 )
, m_OriginalFlags( 0 )
, m_Result( JobResults::Unknown )
, m_WarningCount( 0 )
, m_ErrorCount( 0 )
{

}

AssetBuilder::BuildJob::BuildJob( Dependencies::DependencyGraph* depGraph, const Asset::AssetClassPtr& asset, const BuilderOptionsPtr& options, IBuilder* builder, u32 flags )
: m_DependencyGraph( depGraph )
, m_Asset( asset )
, m_Options( options )
, m_Builder( builder )
, m_Flags( flags )
, m_OriginalFlags( flags )
, m_Result( JobResults::Unknown )
, m_WarningCount( 0 )
, m_ErrorCount( 0 )
{

}