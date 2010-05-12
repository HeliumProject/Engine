#pragma once

#include "API.h"
#include "BuilderInterface.h"
#include "Asset/SkyAsset.h"
#include "Texture/ColorFormats.h"

namespace IGPS3
{
  struct SkyDefs;
}

namespace IG
{
  class MipSet;
}

namespace AssetBuilder
{
  /////////////////////////////////////////////////////////////////////////////
  // Exposed interface to the sky builder (for real-time update).
  //
  class ASSETBUILDER_API ISkyBuilder NOC_ABSTRACT : public IBuilder
  {
  public:
    ISkyBuilder();
    virtual ~ISkyBuilder();

    // Derived class must implement conversion from Asset::SkyAsset to IGPS3::SkyDefs.
    virtual void MakeRuntimeDefs( const Asset::SkyAssetPtr& skyClass, IGPS3::SkyDefs& engineSky ) = 0;

    // Derived class must implement generation of mip settings from a texture path.
    // NOTE: Caller must delete the returned MipSet since it is allocated on the heap.
    virtual IG::MipSet* GenerateMips( const char *path, IG::OutputColorFormat output_format, bool expand_range, bool generate_mips, bool convert_to_linear ) = 0;

    //
    virtual void SyncShells( const Asset::SkyAssetPtr& skyClass ) = 0;

    //
    virtual bool BuildLooseGraphShader(  const Asset::AssetClassPtr& assetClass, const tuid& filePtr, std::string& return_path) = 0;

  };

  typedef Nocturnal::SmartPtr<ISkyBuilder> ISkyBuilderPtr;
}
