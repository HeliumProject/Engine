#include "stdafx.h"

#include "DeleteAssetWizard.h"
#include "ManagedAsset.h"

#include "GridPage.h"
#include "SummaryPage.h"
#include "PerforcePage.h"

#include "Asset/AssetClass.h"
#include "FileSystem/FileSystem.h"
#include "RCS/RCS.h"


using namespace AssetManager;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
DeleteAssetWizard::DeleteAssetWizard( wxWindow* parent, Asset::AssetClass* assetClass ) 
: Wizard( parent, OperationFlags::Delete )
{
  SetTitle( "Delete Asset" );

  // create a new rename asset
  SetAssetClass( assetClass );

  // set up the pages
  GridPage* gridPage      = wxDynamicCast( GridPage::Create( this ), GridPage );

  V_string propertyLabels;
  propertyLabels.push_back( "Delete" );
  propertyLabels.push_back( "File Path" );
  gridPage->SetPropertyLabels( propertyLabels );

  UIToolKit::WizardPage* summaryPage   = SummaryPage::Create( this );

  gridPage->SetNext( summaryPage );
  
  SetFirstPage( gridPage );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
DeleteAssetWizard::~DeleteAssetWizard()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
void DeleteAssetWizard::ProcessOperation()
{
  ProcessDeleteAsset( GetManagedAsset() );
}

void DeleteAssetWizard::PostProcessUpdate()
{
  // nothing to post process here
}

///////////////////////////////////////////////////////////////////////////////
// Create the default changelist description
//
const std::string& DeleteAssetWizard::GetChangeDescription()
{
  if ( m_Changeset.m_Description.empty() )
  {
    std::stringstream description;
    description << "Deleting Asset";
    description << " " << GetManagedAsset()->m_Path;

    m_Changeset.m_Description = description.str();
  }

  return m_Changeset.m_Description;
}