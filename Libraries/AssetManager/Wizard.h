#pragma once

#include "API.h"
#include "AssetManager.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"

#include "UIToolKit/Wizard.h"
#include "PerforceUI/Panel.h"

// Forwards
namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
  class AssetTemplate;
}

namespace AssetManager
{
  class PerforcePage;

  class ManagedAsset;
  typedef Nocturnal::SmartPtr< ManagedAsset > ManagedAssetPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Wizard for duplicating an asset.
  //
  class ASSETMANAGER_API Wizard NOC_ABSTRACT : public UIToolKit::Wizard 
  {
  protected:
    // protected ctor, only called by subclasses
    Wizard( wxWindow* parent, OperationFlag operationFlag );
  
  public:
    virtual ~Wizard();

    virtual bool Run();

    virtual const std::string&  GetNewFileLocation() const;
    virtual void                SetNewFileLocation( const std::string& file );

    ManagedAsset*               GetManagedAsset() const; // ManagedAsset is automatically set by SetAssetClass

    virtual Asset::AssetClass*  GetAssetClass() const;
    void                        SetAssetClass( Asset::AssetClass* assetClass );

    virtual void                ProcessOperation() = 0;
    virtual void                PostProcessUpdate() = 0;

    PerforceUI::Action          GetAction() const { return m_Action; }
    void                        SetAction( PerforceUI::Action action = PerforceUI::Actions::Cancel );

    virtual const std::string&  GetChangeDescription() = 0;
    void                        SetChangeDescription( const std::string& description ) { m_Changeset.m_Description = description; }

    PerforcePage*               GetPerforcePage() { return m_PerforcePage; }

    OperationFlag               GetOperationFlag() const;
    bool                        ShouldUseNewPath() { return m_UseNewPath; }

    bool                        HasErrorOccurred() const { return m_ErrorOccurred; }
    void                        ErrorOccurred() { m_ErrorOccurred = true; }

    const RCS::Changeset&   GetChangeset() { return m_Changeset; }

    virtual void                SetCurrentTemplate( const Asset::AssetTemplate* assetTemplate );
    const Asset::AssetTemplate* GetCurrentTemplate() const;   

    const Reflect::V_Element&   GetAssetTemplates();  

  protected:
    virtual void OnWizardFinished( wxWizardEvent& args ) NOC_OVERRIDE;

  protected:
    ManagedAssetPtr        m_ManagedAsset;

    OperationFlag          m_OperationFlag;
    bool                   m_UseNewPath;
    bool                   m_ErrorOccurred;
    
    PerforcePage*          m_PerforcePage;

    PerforceUI::Action     m_Action;
    RCS::Changeset     m_Changeset;

    Reflect::V_Element     m_AssetTemplates;
    const Asset::AssetTemplate* m_CurrentTemplate;
  };
}
