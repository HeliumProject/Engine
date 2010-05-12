#include "stdafx.h"

#include "RenameAssetWizard.h"
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
RenameAssetWizard::RenameAssetWizard( wxWindow* parent, Asset::AssetClass* assetClass ) 
: Wizard( parent, OperationFlags::Rename )
{
  SetTitle( "Rename Asset" );

  // create a new rename asset
  SetAssetClass( assetClass );

  // set up the pages
  UIToolKit::WizardPage* fileLocPage   = FileLocationPage::Create( this );
  
  GridPage* gridPage      = wxDynamicCast( GridPage::Create( this ), GridPage );
  
  V_string propertyLabels;
  propertyLabels.push_back( "Rename" );
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
RenameAssetWizard::~RenameAssetWizard()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
void RenameAssetWizard::ProcessOperation()
{
  ProcessRenameAsset( GetManagedAsset() );
}


void RenameAssetWizard::PostProcessUpdate()
{
  // nothing to post process here
}


///////////////////////////////////////////////////////////////////////////////
// Create the default changelist description
//
const std::string& RenameAssetWizard::GetChangeDescription()
{
  if ( m_Changeset.m_Description.empty() )
  {
    std::stringstream description;
    description << "Renaming Asset";
    description << " - From: " << GetManagedAsset()->m_Path;
    description << " To: " << GetManagedAsset()->m_NewPath; 

    m_Changeset.m_Description = description.str();
  }

  return m_Changeset.m_Description;
}