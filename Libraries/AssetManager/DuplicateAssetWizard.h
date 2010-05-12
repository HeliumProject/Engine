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
  // Wizard for duplicating an asset.
  //
  class ASSETMANAGER_API DuplicateAssetWizard : public Wizard 
  {  
  public:
    DuplicateAssetWizard( wxWindow* parent, Asset::AssetClass* assetClass );
    virtual ~DuplicateAssetWizard();

    Asset::AssetClass* GetNewAssetClass() const;

    //
    // Wizard API that needs to be implemented by subclasses
    //
    virtual void                ProcessOperation() NOC_OVERRIDE;
    virtual void                PostProcessUpdate() NOC_OVERRIDE;

    virtual const std::string&  GetChangeDescription() NOC_OVERRIDE;

  private:
    Asset::AssetClassPtr   m_NewAssetClass;
  };
}
