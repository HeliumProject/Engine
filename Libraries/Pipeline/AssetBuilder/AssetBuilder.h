#pragma once

#include "Pipeline/API.h"
#include "BuilderOptions.h"
#include "BuilderInterface.h"
#include "Foundation/Container/Insert.h"

#include "Foundation/TUID.h"
#include "Pipeline/Asset/AssetClass.h"

namespace AssetBuilder
{
  struct PIPELINE_API CommandArgs
  {
    static const char* HaltOnError;
  };

  //
  // Event for receiving notifications whenever an asset is done building
  //

  class PIPELINE_API AssetBuiltArgs : public Reflect::Element
  {
  private:
    REFLECT_DECLARE_CLASS( AssetBuiltArgs, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetBuiltArgs>& comp );

  public:
    u64 m_AssetId;
    JobResult m_JobResult;

    AssetBuiltArgs()
      : m_AssetId( 0 )
      , m_JobResult( JobResults::Unknown )
    {
    }

    AssetBuiltArgs( u64 assetId, JobResult result )
      : m_AssetId( assetId )
      , m_JobResult( result )
    {
    }
  };
  typedef Nocturnal::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;

  // the event delegate for the print event
  typedef Nocturnal::Signature<void, const AssetBuiltArgsPtr&> AssetBuiltSignature;

  PIPELINE_API void AddAssetBuiltListener(const AssetBuiltSignature::Delegate& listener);
  PIPELINE_API void RemoveAssetBuiltListener(const AssetBuiltSignature::Delegate& listener);

  //
  // BuiltCacheReference manages the success and failure of builds across build levels
  //

  // Map from build identifier (AssetClass + build string) to the highest build level it was registered at
  typedef std::map< std::pair< Asset::AssetClassPtr, std::string >, u32 > M_BuildIdentifier;

  // init and cleanup work
  PIPELINE_API void Initialize();
  PIPELINE_API void Cleanup();

  // build assets
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, const Asset::AssetClassPtr& assetClass, const BuilderOptionsPtr& options = NULL );
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, const Asset::AssetClassPtr& assetClass, const V_string& options );
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, Nocturnal::Path& path, const BuilderOptionsPtr& options = NULL );
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, Nocturnal::Path& path, const V_string& options );
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, const BuildJobPtr& job );
  PIPELINE_API void Build( Dependencies::DependencyGraph& graph, V_BuildJob& jobs, i32 nice = -1 );
  
  // query build results
  PIPELINE_API bool DidBuildFail( const tuid assetId );
};