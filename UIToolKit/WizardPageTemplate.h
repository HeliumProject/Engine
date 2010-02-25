#pragma once

#include "WizardPage.h"

namespace UIToolKit
{
  // Forwards
  class Wizard;

  ///////////////////////////////////////////////////////////////////////////
  // Templated base class for generating wizard pages and associating them
  // with wxFormBuilder constructed panels.
  // 
  template < class TPanel >
  class WizardPageTemplate NOC_ABSTRACT : public WizardPage
  {
  protected:
    TPanel* m_Panel;

  protected:
    // Protected constructor - use a derived class's static creator function.
    WizardPageTemplate( Wizard* wizard )
    : WizardPage( wizard )
    {
      m_Panel = new TPanel( this );
      wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
      sizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );
    	
      SetSizer( sizer );
      sizer->Fit( this );
    }

  public:
    // Destructor
    virtual ~WizardPageTemplate()
    {
    }
  };
}
