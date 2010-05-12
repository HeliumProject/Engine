#pragma once

#include "API.h"
#include "Wizard.h"

// Forwards
namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
};

namespace AssetManager
{
  /////////////////////////////////////////////////////////////////////////////
  // Wizard for renaming/moving an asset.
  //
  class ASSETMANAGER_API DeleteAssetWizard : public Wizard 
  {  
  public:
    DeleteAssetWizard( wxWindow* parent, Asset::AssetClass* assetClass );
    virtual ~DeleteAssetWizard();

    virtual void                ProcessOperation() NOC_OVERRIDE;
    virtual void                PostProcessUpdate() NOC_OVERRIDE;

    virtual const std::string&  GetChangeDescription() NOC_OVERRIDE;
  };
}
