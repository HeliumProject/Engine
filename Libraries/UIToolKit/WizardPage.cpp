#include "StdAfx.h"
#include "WizardPage.h"
#include "Wizard.h"

using namespace UIToolKit;

///////////////////////////////////////////////////////////////////////////////
// Protected constructor - use a derived class's static creator function.
//
WizardPage::WizardPage( Wizard* wizard )
: wxWizardPage( wizard )
, m_Wizard( wizard )
, m_Prev( NULL )
, m_Next( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
//
WizardPage::~WizardPage()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the wizard that this page belongs to.
// 
Wizard* WizardPage::GetWizard() const
{
  return m_Wizard;
}

///////////////////////////////////////////////////////////////////////////////
// When the Finish button on the wizard is clicked, the wizard will call this
// function for each page that is accessible.  Derived classes can NOC_OVERRIDE
// this function to do some final work.
// 
void WizardPage::Finished()
{
  // Override in derived class if you need to do something.
}

///////////////////////////////////////////////////////////////////////////////
// Set the previous page
//
void WizardPage::SetPrev( WizardPage* page )
{
  m_Prev = page;
}

///////////////////////////////////////////////////////////////////////////////
// Set the next page (also sets this to be the page's previous page automatically)
//
void WizardPage::SetNext( WizardPage* page )
{
  m_Next = page;
  if ( page )
  {
    page->SetPrev( this );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Return the previous page
//
wxWizardPage* WizardPage::GetPrev() const
{
  return m_Prev;
}

///////////////////////////////////////////////////////////////////////////////
// Return the next page
//
wxWizardPage* WizardPage::GetNext() const
{
  return m_Next;
}