#include "Windows/Windows.h"
#include "BuilderInterface.h"

#include "Console/Console.h"
#include "Windows/Error.h"
#include "Asset/AssetClass.h"

using namespace Asset;
using namespace AssetBuilder;

typedef std::map<EngineType, HMODULE> M_Library;
typedef std::map<EngineType, AllocateBuildersFunc> M_Allocator;

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
void BuilderInterface::AllocateBuilders( EngineType engineType, AssetBuilder::V_IBuilder& builders )
{
  {
    M_Allocator::const_iterator found = g_Allocators.find( engineType );
    if ( found != g_Allocators.end() )
    {
      found->second( builders );

      return;
    }
  }

  std::string builderDLL = AssetClass::GetEngineTypeBuilderDLL( engineType );
  if ( builderDLL.empty() )
  {
    throw AssetBuilder::Exception( "Cannot find builder module for asset type %d", engineType );
  }

  HMODULE lib = LoadLibrary( builderDLL.c_str() );
  if( lib == NULL )
  {
    DWORD err = ::GetLastError();
    throw AssetBuilder::Exception( "Error loading library %s: (%d) %s\n", builderDLL.c_str(), err, Windows::GetErrorString(err).c_str() );
  }

  void (*init)() = (void (*)())GetProcAddress( lib, InitializeEntryPoint );
  if( init == NULL )
  {
    FreeLibrary( lib );
    throw AssetBuilder::Exception( "Cannot load builder interface from lib: %s", builderDLL.c_str() );
  }

  init();

  AllocateBuildersFunc func = ( AllocateBuildersFunc )( GetProcAddress( lib, AllocateEntryPoint ) );
  if( func == NULL )
  {
    FreeLibrary( lib );
    throw AssetBuilder::Exception( "Cannot load builder interface from lib: %s", builderDLL.c_str() );
  }

  g_Libraries[engineType] = lib;
  g_Allocators[engineType] = func;

  func( builders );
}

AssetBuilder::BuildJob::BuildJob()
: m_Asset( NULL )
, m_Options( NULL )
, m_Builder( NULL )
, m_Flags( 0 )
, m_OriginalFlags( 0 )
, m_Result( JobResults::Unknown )
, m_WarningCount( 0 )
, m_ErrorCount( 0 )
{

}

AssetBuilder::BuildJob::BuildJob( const Asset::AssetClassPtr& asset, const BuilderOptionsPtr& options, IBuilder* builder, u32 flags )
: m_Asset( asset )
, m_Options( options )
, m_Builder( builder )
, m_Flags( flags )
, m_OriginalFlags( flags )
, m_Result( JobResults::Unknown )
, m_WarningCount( 0 )
, m_ErrorCount( 0 )
{

}