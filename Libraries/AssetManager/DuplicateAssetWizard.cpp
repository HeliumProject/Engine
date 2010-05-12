#include "stdafx.h"

#include "DuplicateAssetWizard.h"
#include "ManagedAsset.h"

#include "FileLocationPage.h"
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
DuplicateAssetWizard::DuplicateAssetWizard( wxWindow* parent, Asset::AssetClass* assetClass ) 
: Wizard( parent, OperationFlags::Duplicate )
{
  SetTitle( "Duplicate Asset" );

  // create a new rename asset
  SetAssetClass( assetClass );

  // set up the pages
  UIToolKit::WizardPage* fileLocPage   = FileLocationPage::Create( this );
  
  GridPage* gridPage      = wxDynamicCast( GridPage::Create( this ), GridPage );

  V_string propertyLabels;
  propertyLabels.push_back( "Duplicate" );
  propertyLabels.push_back( "Original Path" );
  propertyLabels.push_back( "New Path" );
  gridPage->SetPropertyLabels( propertyLabels );

  UIToolKit::WizardPage* summaryPage   = SummaryPage::Create( this );

  fileLocPage->SetNext( gridPage ); 
  gridPage->SetNext( summaryPage );
  
  SetFirstPage( fileLocPage );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
DuplicateAssetWizard::~DuplicateAssetWizard()
{
}


///////////////////////////////////////////////////////////////////////////////
// Returns the new asset class that this wizard has created
// 
Asset::AssetClass* DuplicateAssetWizard::GetNewAssetClass() const
{
  return m_NewAssetClass;
}


///////////////////////////////////////////////////////////////////////////////
// 
void DuplicateAssetWizard::ProcessOperation()
{
  ProcessDuplicateAsset( GetManagedAsset() );
}

void DuplicateAssetWizard::PostProcessUpdate()
{
  m_NewAssetClass = UpdateDuplicateAsset( GetManagedAsset() ) ;
}


///////////////////////////////////////////////////////////////////////////////
// Create the default changelist description
//
const std::string& DuplicateAssetWizard::GetChangeDescription()
{
  if ( m_Changeset.m_Description.empty() )
  {
    std::stringstream description;
    description << "Duplicating Asset";
    description << " - From: " << GetManagedAsset()->m_Path;
    description << " To: " << GetManagedAsset()->m_NewPath; 

    m_Changeset.m_Description = description.str();
  }

  return m_Changeset.m_Description;
}