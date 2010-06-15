#include "Precompile.h"
#include "Wizard.h"
#include "WizardPage.h"
#include "Platform/Assert.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Wizard::Wizard( wxWindow* parent, int id, const wxString& title, const wxBitmap& bitmap, const wxPoint& pos, long style )
: wxWizard( parent, id, title, bitmap, pos, style )
, m_Direction( PageDirections::Forward )
, m_FirstPage( NULL )
{
  // Connect listeners
  Connect( GetId(), wxEVT_WIZARD_FINISHED, wxWizardEventHandler( Wizard::OnWizardFinished ), NULL, this );
  Connect( wxID_BACKWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnBackButton ), NULL, this );
  Connect( wxID_FORWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Wizard::OnNextButton ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Wizard::~Wizard()
{
  // Disconnect listeners
  Disconnect( GetId(), wxEVT_WIZARD_FINISHED, wxWizardEventHandler( Wizard::OnWizardFinished ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the first page of the wizard.
// 
wxWizardPage* Wizard::GetFirstPage() const
{
  return m_FirstPage;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the first page of the wizard.  Derived classes should call this function
// before the wizard is shown.
// 
void Wizard::SetFirstPage( wxWizardPage* page )
{
  if ( m_FirstPage != page )
  {
    if ( m_FirstPage )
    {
      GetPageAreaSizer()->Remove( m_FirstPage ); 
    }

    m_FirstPage = page;

    if ( m_FirstPage )
    {
      // wxWizard bug: If you don't do this, the sizer becomes a memory leak.
      GetPageAreaSizer()->Add( m_FirstPage ); 

      // If you don't call Fit, the wizard size is wrong and you get an assert in debug mode.
      GetPageAreaSizer()->Fit( m_FirstPage );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the wizard pages by starting at the first page, and calling
// GetNext for each one.  Returns true if the specified page is found during
// the traversal.
// 
bool Wizard::IsPageInWizard( wxWizardPage* page )
{
  NOC_ASSERT( page );

  if ( page )
  {
    wxWizardPage* current = GetFirstPage();
    while ( current )
    {
      if ( current == page )
      {
        return true;
      }
      current = current->GetNext();
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Runs the wizard.  Returns true if the wizard is completed successfully or
// false if the user cancels the wizard.  Make sure you set up the first page
// of the wizard before calling this function.
// 
bool Wizard::Run()
{
  NOC_ASSERT( GetFirstPage() );
  return RunWizard( GetFirstPage() );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a value indicating which wizard button was last pressed by the
// user (back or next).
// 
PageDirections::PageDirection Wizard::GetDirection() const
{
  return m_Direction;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Finish button is clicked on the wizard.  Traverses 
// through the wizard pages (in order), calling Finished on each one.
// 
void Wizard::OnWizardFinished( wxWizardEvent& args )
{
  args.Skip();

  WizardPage* page = wxDynamicCast( GetFirstPage(), WizardPage );
  while ( page )
  {
    page->Finished();
    page = wxDynamicCast( page->GetNext(), WizardPage );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the back button is pressed.  Stores the page direction.
// 
void Wizard::OnBackButton( wxCommandEvent& args )
{
  args.Skip();
  m_Direction = PageDirections::Backward;
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the next button is pressed.  Stores the page direction.
// 
void Wizard::OnNextButton( wxCommandEvent& args )
{
  args.Skip();
  m_Direction = PageDirections::Forward;
}
