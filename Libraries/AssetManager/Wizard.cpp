#include "stdafx.h"
#include "Wizard.h"

#include "ManagedAsset.h"

#include "FileLocationPage.h"
#include "GridPage.h"
#include "SummaryPage.h"
#include "PerforcePage.h"

#include "Common/CommandLine.h"
#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetTemplate.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "RCS/RCS.h"



///////////////////////////////////////////////////////////////////////////////
// Using
///////////////////////////////////////////////////////////////////////////////
using namespace AssetManager;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Wizard::Wizard( wxWindow* parent, OperationFlag operationFlag ) 
: UIToolKit::Wizard( parent, wxID_ANY, "Asset Manager Wizard" )
, m_OperationFlag( operationFlag )
, m_Action( PerforceUI::Actions::Submit )
, m_ErrorOccurred( false )
, m_UseNewPath( false )
, m_CurrentTemplate( NULL )
{
  if ( Nocturnal::GetCmdLineFlag( File::Args::NoAutoSubmit ) )
  {
    m_Action = PerforceUI::Actions::Update;
  }

  m_UseNewPath = ( ( operationFlag == OperationFlags::Rename ) || ( operationFlag == OperationFlags::Duplicate ) );

  m_PerforcePage = wxDynamicCast( PerforcePage::Create( this ), PerforcePage );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Wizard::~Wizard()
{
}







///////////////////////////////////////////////////////////////////////////////
// Returns the rename asset object that this wizard is operating on.
// 
ManagedAsset* Wizard::GetManagedAsset() const
{
  NOC_ASSERT( m_ManagedAsset.ReferencesObject() );
  return m_ManagedAsset;
}


///////////////////////////////////////////////////////////////////////////////
// As this wizard progresses, pages can access the asset class and make changes
// to it.  When the wizard completes, call this function to get the asset class
// that was constructed by the wizard.
// 
Asset::AssetClass* Wizard::GetAssetClass() const
{
  NOC_ASSERT( m_ManagedAsset.ReferencesObject() );
  return m_ManagedAsset->m_Asset;
}


///////////////////////////////////////////////////////////////////////////////
void Wizard::SetAssetClass( Asset::AssetClass* assetClass )
{
  // create a new managed asset
  m_ManagedAsset = new ManagedAsset( assetClass, m_OperationFlag );


  //HACKS!
  int templateOffset = 0;

  Asset::EngineType type = assetClass->GetEngineType();
  switch ( type )
  {
  default:
    templateOffset = 0;
    break;

  case Asset::EngineTypes::Moby:
    templateOffset = 1;
    break;

  case Asset::EngineTypes::Ufrag:
    templateOffset = 2;
    break;
  }

  SetCurrentTemplate( (Asset::AssetTemplate*)((GetAssetTemplates().at(templateOffset)).Ptr()) );
}


///////////////////////////////////////////////////////////////////////////////
// When renaming or duplicating an asset, this will indicate where the new asset
// should be saved.
// 
const std::string& Wizard::GetNewFileLocation() const
{
  NOC_ASSERT( m_ManagedAsset.ReferencesObject() );
  return m_ManagedAsset->m_NewPath;
}

///////////////////////////////////////////////////////////////////////////////
// Allows wizard pages to set the location of where a new asset file will be
// saved (for rename/duplicate).
// 
void Wizard::SetNewFileLocation( const std::string& file )
{
  m_ManagedAsset->m_NewPath = file;
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
OperationFlag Wizard::GetOperationFlag() const 
{ 
  return m_OperationFlag; 
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void Wizard::SetAction( PerforceUI::Action action )
{
  m_Action = action;
}


///////////////////////////////////////////////////////////////////////////////
// Gets the available templates for the given asset type
const Reflect::V_Element& Wizard::GetAssetTemplates()
{
  if ( m_AssetTemplates.empty() )
  {
    Asset::AssetTemplate::GetAssetTemplates( GetAssetClass()->GetType(), m_AssetTemplates );
  }

  return m_AssetTemplates;
}

///////////////////////////////////////////////////////////////////////////////
// Specify which of the asset's templates is currently being used.  This may
// cause the path to the asset to change.  It may also cause the current 
// attributes on the asset class to change.
// 
void Wizard::SetCurrentTemplate( const Asset::AssetTemplate* assetTemplate )
{
  if ( assetTemplate != m_CurrentTemplate )
  {
    m_CurrentTemplate = assetTemplate;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the template currently being used by this wizard.  The template can
// be changed during the course of the wizard running.
// 
const Asset::AssetTemplate* Wizard::GetCurrentTemplate() const
{
  return m_CurrentTemplate;
}

///////////////////////////////////////////////////////////////////////////////
// Runs the wizard.  Returns true if the wizard is completed successfully or
// false if the user cancels the wizard.  Make sure you set up the first page
// of the wizard before calling this function.
// 
bool Wizard::Run()
{
  NOC_ASSERT( GetFirstPage() );
  return ( RunWizard( GetFirstPage() ) && !HasErrorOccurred() );
}


///////////////////////////////////////////////////////////////////////////////
// Called when the wizard completes.  Makes sure that the asset class is ready
// to be passed back to the caller.
// 
void Wizard::OnWizardFinished( wxWizardEvent& args )
{
  if ( m_Changeset.m_Id == RCS::DefaultChangesetId )
  {
    m_Changeset.Create();
  }

  File::GlobalManager().SetWorkingChangeset( m_Changeset );

  File::GlobalManager().BeginTrans( File::ManagerConfigs::Default, false );

  try
  {
    ProcessOperation();

    // Traverses through the wizard pages (in order), calling Finished on each one.
    __super::OnWizardFinished( args );
  }
  catch ( const Nocturnal::Exception& e )
  {
    std::string error;
    error = "Error occurred while managing the asset: ";
    error += m_ManagedAsset->m_Path;
    error += ".\n";
    error += e.Get();
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );

    File::GlobalManager().RollbackTrans();
    
    ErrorOccurred();

    m_Changeset.Revert();
    m_Changeset.Clear();
    File::GlobalManager().SetWorkingChangeset( m_Changeset );
    return;
  }

  // assume that if an error occured in a Finished call, the user has been informed
  // of the errors by that page
  if ( HasErrorOccurred() )
  {
    File::GlobalManager().RollbackTrans();

    m_Changeset.Revert();
    m_Changeset.Clear();
    File::GlobalManager().SetWorkingChangeset( m_Changeset );
    return;
  }

  try
  {
    // only do a partial commit, that way if this fails it wont leave the user's DB in a bad state
    File::GlobalManager().CommitTrans( true );

    // here's where some additional work needs to take place
    // so we call into the wizard specific virtual update functions
    PostProcessUpdate();
  }
  catch ( const Nocturnal::Exception& e )
  {
    std::string error;
    error = "Error occurred while managing the asset: ";
    error += m_ManagedAsset->m_Path;
    error += ".\n";
    error += e.Get();
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    
    File::GlobalManager().RollbackTrans();

    ErrorOccurred();

    m_Changeset.Revert();
    m_Changeset.Clear();
    File::GlobalManager().SetWorkingChangeset( m_Changeset );
    return;
  }

  try
  {
    File::GlobalManager().CommitTrans();
  }
  catch ( const Nocturnal::Exception& e )
  {
    std::string error;
    error = "Failed to commit transaction";
    if ( !e.Get().empty() )
    {
      error += ": " + e.Get();
    }
    wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    ErrorOccurred();

    m_Changeset.Revert();
    m_Changeset.Clear();
    File::GlobalManager().SetWorkingChangeset( m_Changeset );
    return;
  }

  // don't auto submit if the user specified not to
  if ( m_PerforcePage && !Nocturnal::GetCmdLineFlag( File::Args::NoAutoSubmit ) )
  {
    m_PerforcePage->CommitChangelist();
    m_Changeset.Clear();
    File::GlobalManager().SetWorkingChangeset( m_Changeset );
  }
}
