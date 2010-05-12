#pragma once

#include "API.h"
#include "BuilderOptions.h"
#include "BuilderInterface.h"
#include "Common/Container/Insert.h"

#include "TUID/TUID.h"
#include "Asset/AssetClass.h"

namespace AssetBuilder
{
  struct ASSETBUILDER_API CommandArgs
  {
    static const char* HaltOnError;
  };

  //
  // Event for receiving notifications whenever an asset is done building
  //

  class ASSETBUILDER_API AssetBuiltArgs : public Reflect::Element
  {
  private:
    REFLECT_DECLARE_CLASS( AssetBuiltArgs, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetBuiltArgs>& comp );

  public:
    tuid m_AssetId;
    JobResult m_JobResult;

    AssetBuiltArgs()
      : m_AssetId( 0 )
      , m_JobResult( JobResults::Unknown )
    {
    }

    AssetBuiltArgs( tuid assetId, JobResult result )
      : m_AssetId( assetId )
      , m_JobResult( result )
    {
    }
  };
  typedef Nocturnal::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;

  // the event delegate for the print event
  typedef Nocturnal::Signature<void, const AssetBuiltArgsPtr&> AssetBuiltSignature;

  ASSETBUILDER_API void AddAssetBuiltListener(const AssetBuiltSignature::Delegate& listener);
  ASSETBUILDER_API void RemoveAssetBuiltListener(const AssetBuiltSignature::Delegate& listener);

  //
  // BuiltCacheReference manages the success and failure of builds across build levels
  //

  // Map from build identifier (AssetClass + build string) to the highest build level it was registered at
  typedef std::map< std::pair< Asset::AssetClassPtr, std::string >, u32 > M_BuildIdentifier;

  // init and cleanup work
  ASSETBUILDER_API void Initialize();
  ASSETBUILDER_API void Cleanup();

  // build assets
  ASSETBUILDER_API void Build( const Asset::AssetClassPtr& assetClass, const BuilderOptionsPtr& options = NULL );
  ASSETBUILDER_API void Build( const Asset::AssetClassPtr& assetClass, const V_string& options );
  ASSETBUILDER_API void Build( const tuid assetId, const BuilderOptionsPtr& options = NULL );
  ASSETBUILDER_API void Build( const tuid assetId, const V_string& options );
  ASSETBUILDER_API void Build( const BuildJobPtr& job );
  ASSETBUILDER_API void Build( V_BuildJob& jobs, i32 nice = -1 );
  
  // view assets, region argument is only used for levels
  ASSETBUILDER_API void View( const Asset::AssetClassPtr& assetClass, const std::string& region );

  // query build results
  ASSETBUILDER_API bool DidBuildFail( const tuid assetId );
};