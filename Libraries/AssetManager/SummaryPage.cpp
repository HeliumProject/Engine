#include "stdafx.h"
#include "SummaryPage.h"

#include "WizardPanels.h"
#include "PerforcePage.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "File/Manager.h"
#include "RCS/RCS.h"

#include "PerforceUI/Panel.h"
#include "PerforceUI/Dialog.h"

// Using
using namespace AssetManager;


static const char* s_PageTitle = "Summary";
static const char* s_PageDescription = "Please review this summary carefully before continuing onto the next page. If you have made any mistakes, now is the time to go back and correct them. Oh who am I kidding, no one reads this message anyways.";


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SummaryPage::SummaryPage( Wizard* wizard )
: WizardPage< SummaryPanel >( wizard )
{
  // Set up title of page
  m_Panel->m_Title->SetLabel( s_PageTitle );
  m_Panel->m_Description->SetLabel( s_PageDescription );

  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );
  m_Panel->Layout();

  SetNext( GetWizard()->GetPerforcePage() );
}


///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SummaryPage::~SummaryPage()
{
}


///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* SummaryPage::Create( Wizard* wizard )
{
  return new SummaryPage( wizard );
}


///////////////////////////////////////////////////////////////////////////////
// Called just before the window is shown.  Populates the UI.
// 
bool SummaryPage::TransferDataToWindow()
{
  // Only do validation if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    return true;
  }

  wxBusyCursor bc;

  // Clear the summary info
  m_Panel->m_SummaryTextBox->Clear();

  // Print the summary info
  if ( GetWizard()->GetOperationFlag() == OperationFlags::Create )
  {
    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_TitleTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << GetWizard()->GetAssetClass()->GetClass()->m_UIName << " will be created at:\n";

    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_DefaultTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << "  o " << GetWizard()->GetNewFileLocation() << "\n" ;
  }
  else if ( GetWizard()->ShouldUseNewPath() )
  {
    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_TitleTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << GetWizard()->GetTitle() << ":\n";

    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_DefaultTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << " From: " << GetWizard()->GetAssetClass()->GetFilePath() << "\n";
    ( *m_Panel->m_SummaryTextBox ) << " To: " << GetWizard()->GetNewFileLocation() << "\n\n";
  }
  else
  {
    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_TitleTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << GetWizard()->GetTitle() << ":\n";

    m_Panel->m_SummaryTextBox->SetDefaultStyle( m_DefaultTextAttr );
    ( *m_Panel->m_SummaryTextBox ) << "  o " << GetWizard()->GetAssetClass()->GetFilePath() << "\n\n";
  }


  // iterate over the other pages and append the page summary info
  WizardPage* currentPage = wxDynamicCast( GetWizard()->GetFirstPage(), WizardPage );
  while ( currentPage )
  {
    for each ( const StyledText& styledText in currentPage->GetPageSummary() )
    {
      m_Panel->m_SummaryTextBox->SetDefaultStyle( *styledText.m_Style );
      ( *m_Panel->m_SummaryTextBox ) << styledText.m_Text.c_str();
    }

    ( *m_Panel->m_SummaryTextBox ) << "\n";

    currentPage = wxDynamicCast( currentPage->GetNext(), WizardPage );
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Called when switching pages.  Transfers data from the UI to the data managed
// by the wizard class.
//
// Set the perforce changelist description and the summary page info
// 
bool SummaryPage::TransferDataFromWindow()
{
  return true;
}