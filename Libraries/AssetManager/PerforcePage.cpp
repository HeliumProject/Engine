#include "stdafx.h"
#include "PerforcePage.h"

#include "ManagedAsset.h"
#include "WizardPanels.h"

#include "Asset/AssetInit.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"

#include "PerforceUI/Panel.h"

// Using
using namespace AssetManager;


static const char* s_PageTitle = "Perforce Changelist";
static const char* s_PageDescription = "Please review the following changelist desctiption and list of files for submission.";


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PerforcePage::PerforcePage( Wizard* wizard )
: WizardPage< PerforceUI::Panel >( wizard )
{
  m_Panel->ShowTitle( s_PageTitle, s_PageDescription );
  m_Panel->SetAction( GetWizard()->GetAction() );
  
  m_Panel->HideClientDetails();
  m_Panel->HideJobStatus();
  m_Panel->HideCommitButtons(); 

  m_Panel->Layout();
}


///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PerforcePage::~PerforcePage()
{
}


///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* PerforcePage::Create( Wizard* wizard )
{
  return new PerforcePage( wizard );
}

///////////////////////////////////////////////////////////////////////////////
// Called just before the window is shown. Populates the UI.
// 
bool PerforcePage::TransferDataToWindow()
{
  wxBusyCursor bc;


  V_string fileList;

  fileList.insert( fileList.begin(), ( GetWizard()->ShouldUseNewPath() ? GetWizard()->GetManagedAsset()->m_NewPath : GetWizard()->GetManagedAsset()->m_Path ) );

  // iterate over the other pages and append the page summary info
  WizardPage* currentPage = wxDynamicCast( GetWizard()->GetFirstPage(), WizardPage );
  while ( currentPage )
  {
    fileList.insert( fileList.end(), currentPage->GetFileList().begin(), currentPage->GetFileList().end() );

    currentPage = wxDynamicCast( currentPage->GetNext(), WizardPage );
  }

  m_Panel->SetFileList( fileList );

  m_Panel->SetChangeDescription( GetWizard()->GetChangeDescription() );
  m_Panel->SetAction( GetWizard()->GetAction() );

  return m_Panel->TransferDataToForm();
}


///////////////////////////////////////////////////////////////////////////////
// Called when switching pages.  Transfers data from the UI to the data managed
// by the wizard class and the PerforceUI::Panel will validate.
// 
bool PerforcePage::TransferDataFromWindow()
{
  // Only do get form if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    return true;
  }

  wxBusyCursor bc;

  bool result = m_Panel->TransferDataFromForm();

  if ( result )
  {
    GetWizard()->SetChangeDescription( m_Panel->GetChangeDescription() );
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
void PerforcePage::SetReopenFiles( bool reopenFiles )
{ 
  m_Panel->SetReopenFiles( reopenFiles );
}


///////////////////////////////////////////////////////////////////////////////
void PerforcePage::CommitChangelist()
{
  try
  {
    m_Panel->SetChangeset( GetWizard()->GetChangeset() );
    m_Panel->CommitChanges();
  }
  catch( const Nocturnal::Exception& ex )
  {    
    std::string error = "Some files could not be submitted; ";
    error += ex.what();
    wxMessageBox( error, "Error", wxCENTER | wxICON_ERROR | wxOK, GetParent() );
  }
}