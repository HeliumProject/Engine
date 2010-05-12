#pragma once

#include "API.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>


namespace File
{
  /////////////////////////////////////////////////////////////////////////////
  // Property class that is connected to a ManagedFileProperty for use in a 
  // property grid.  Provides a string property that should be the path to a
  // file.  A button is available, and when clicked, a ManagedFileDialog is
  // shown to allow the user to select a file and change the value of this
  // property.
  // 
  class FILEUI_API ManagedFileProperty : public wxLongStringProperty 
  { 
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS(ManagedFileProperty) 
  private:
    long m_Flags;
    wxString m_Title;
    wxString m_Filter;

  public: 
    ManagedFileProperty( const wxString& name = wxPG_LABEL, const wxString& label = wxPG_LABEL, const wxString& value = wxEmptyString); 
    virtual ~ManagedFileProperty(); 
    virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value ); 
    virtual bool DoSetAttribute( const wxString& name, wxVariant& value );
    virtual const wxPGEditor* DoGetEditorClass() const;
    WX_PG_DECLARE_VALIDATOR_METHODS() 
  };
}
